/*
  Copyright (C) 2025  Selwin van Dijk

  This file is part of signalbackup-viewer.

  signalbackup-viewer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  signalbackup-viewer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with signalbackup-viewer.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef EMOJITEXTDOCUMENT_H_
#define EMOJITEXTDOCUMENT_H_

#include <QByteArray>
#include <QTextDocument>
#include <QTextCursor>
#include <QString>

#include <set>
#include <unordered_set>

#include "../shared/msgrange.h"
#include "../common_fe.h"
#include "../recipientpreferences/recipientpreferences.h"

using std::literals::string_literals::operator""s;

class EmojiTextDocument : public QTextDocument
{
  //using Mention = std::tuple<long long int, long long int, long long int, QString>;

  static char const *const s_emoji_unicode_list[3781];
  static std::unordered_set<char> const s_emoji_first_bytes;
  static unsigned int constexpr s_emoji_min_size = 2;
 public:
  inline EmojiTextDocument() = default;
  inline void setHtml(std::string const &str, QByteArray const &ranges = QByteArray(),
                      QVariant const &mentions = QVariant());

 private:
  std::vector<std::pair<unsigned int, unsigned int>> getEmojiPos(std::string const &str) const;
  inline std::string HTMLescapeString(std::string const &body) const;
  void HTMLescapeString(std::string *body, std::set<int> const *const positions_excluded_from_escape = nullptr) const;
  void applyRanges(std::string *body, std::vector<Range> *ranges, std::set<int> *positions_excluded_from_escape) const;
  void prepRanges(std::vector<Range> *ranges) const;
  inline int bytesToUtf8CharSize(std::string const &body, int idx) const;
  inline int utf16CharSize(std::string const &body, int idx) const;
  inline int numBytesInUtf16Substring(std::string const &text, unsigned int idx, int length) const;
};

inline std::string EmojiTextDocument::HTMLescapeString(std::string const &body) const
{
  std::string result(body);
  HTMLescapeString(&result);
  return result;
}

inline void EmojiTextDocument::HTMLescapeString(std::string *body, std::set<int> const *const positions_excluded_from_escape) const
{
  // escape special html chars second, so the span's added by emojifinder (next) aren't escaped
  int positions_added = 0;
  for (unsigned int i = 0; i < body->length(); ++i)
  {
    //qInfo() << "I, POSITIONS_ADDED: " << i << "," << positions_added;

    if ((*body)[i] == '&')
    {
      if (!positions_excluded_from_escape ||
          positions_excluded_from_escape->find(i - positions_added) == positions_excluded_from_escape->end())
      {
        body->replace(i, 1, "&amp;");
        positions_added += 5 - 1;
        i += 5 - 1;
        //changed = true;
      }
    }
    else if ((*body)[i] == '<')
    {
      if (!positions_excluded_from_escape ||
          positions_excluded_from_escape->find(i - positions_added) == positions_excluded_from_escape->end())
      {
        body->replace(i, 1, "&lt;");
        positions_added += 4 - 1;
        i += 4 - 1;
        //changed = true;
      }
    }
    else if ((*body)[i] == '>')
    {
      if (!positions_excluded_from_escape ||
          positions_excluded_from_escape->find(i - positions_added) == positions_excluded_from_escape->end())
      {
        body->replace(i, 1, "&gt;");
        i += 4 - 1;
        positions_added += 4 - 1;
        //changed = true;
      }
    }
    else if ((*body)[i] == '"')
    {
      if (!positions_excluded_from_escape ||
          positions_excluded_from_escape->find(i - positions_added) == positions_excluded_from_escape->end())
      {
        body->replace(i, 1, "&quot;");
        i += 6 - 1;
        positions_added += 6 - 1;
        //changed = true;
      }
    }
    else if ((*body)[i] == '\'')
    {
      if (!positions_excluded_from_escape ||
          positions_excluded_from_escape->find(i - positions_added) == positions_excluded_from_escape->end())
      {
        body->replace(i, 1, "&apos;");
        i += 6 - 1;
        positions_added += 6 - 1;
        //changed = true;
      }
    }
    else if ((*body)[i] == '\n')
    {
      if (!positions_excluded_from_escape ||
          positions_excluded_from_escape->find(i - positions_added) == positions_excluded_from_escape->end())
      {
        body->replace(i, 1, "<br>");
        i += 4 - 1;
        positions_added += 4 - 1;
        //changed = true;
      }
    }
  }
}


inline void EmojiTextDocument::setHtml(std::string const &str, QByteArray const &msgrange, QVariant const &mentions)
{
  clear();

  if (str.empty())
    return;

  // prep msg body
  std::string body(str);

  std::vector<Range> ranges;

  // first, add mentions...
  if (mentions.isValid())
  {
    std::vector<bepaald::Mention> const &mentionsv = mentions.value<std::vector<bepaald::Mention>>();
    for (auto const &m : mentionsv)
    {
      std::string author = RecipientPreferences::getName(QString::number(m.recipient_id)).toStdString();
      // m1 : uuid, m2: start, m3: length
      if (!author.empty())
      {
        ranges.emplace_back(Range{m.range_start, m.range_length,
                                  (m.isquote ? "" : "<b>"),
                                  "@" + author,
                                  (m.isquote ? "" : "</b>"),
                                  true});
      }
    }
  }

  // now do other stylings?
  bool hasstyledlinks = false;
  if (!msgrange.isEmpty())
  {
    BodyRanges brsproto(reinterpret_cast<unsigned char const *>(msgrange.constData()), msgrange.size());

    //qInfo() << "printing rangedata";
    //brsproto.print();

    auto brs = brsproto.getField<1>();
    for (auto const &br : brs)
    {
      int start = br.getField<1>().value_or(0);
      int length = br.getField<2>().value_or(0);
      if (!length) // maybe legal? no length == rest of string? (like no start is beg)
        continue;

      /*
      // get mention
      std::string mentionuuid = br.getField<3>().value_or(std::string());
      if (!mentionuuid.empty())
      {
        long long int authorid = getRecipientIdFromUuidMapped(mentionuuid, nullptr);
        std::string author = getRecipientInfoFromMap(recipient_info, authorid).display_name;
        if (!author.empty())
          ranges.emplace_back(Range{start, length,
                                    (isquote ? "" : "<span class=\"mention-"s + (incoming ? "in" : "out") + "\">"),
                                    "@" + author,
                                    (isquote ? "" : "</span>"),
                                    true});
      }
      */

      // get style
      int style = br.getField<4>().value_or(-1);

      // get link
      std::string link = br.getField<5>().value_or(std::string());

      if (style > -1)
      {
        //std::cout << "Adding style to range [" << start << "-" << start+length << "] : " << style << std::endl;
        switch (style)
        {
          case 0: // BOLD
          {
            ranges.emplace_back(Range{start, length, "<b>", "", "</b>", false});
            break;
          }
          case 1: // ITALIC
          {
            ranges.emplace_back(Range{start, length, "<i>", "", "</i>", false});
            break;
          }
          case 2: // SPOILER
          {
            ranges.emplace_back(Range{start, length, "<span class=\"spoiler\">", "", "</span>", true});
            break;
          }
          case 3: // STRIKETHROUGH
          {
            ranges.emplace_back(Range{start, length, "<s>", "", "</s>", false}); // or <del>? or <span class="strikthrough">?
            break;
          }
          case 4: // MONOSPACE
          {
            ranges.emplace_back(Range{start, length, "<span class=\"monospace\">", "", "</span>", false});
            break;
          }
          default:
          {
            Logger::warning("Unsupported range-style: ", style);
          }
        }
      }
      if (!link.empty())
      {
        //std::cout << "Adding link to range [" << start << "-" << start+length << "] '" << link << "'" << std::endl;
        ranges.emplace_back(Range{start, length, "<a class=\"styled-link\" href=\"" + link + "\">", "", "</a>", true});
        hasstyledlinks = true;
      }
    }
  }


  /*
  // scan for links (somehow skipping the styled links above!), then
  // ranges.emplace_back(Range{start, length, <a href=\"" + link + "\">", "", "</a>", true});
  if (linkify && !hasstyledlinks)
  HTMLLinkify(*body, &ranges);
  */

  // apply...
  std::set<int> positions_excluded_from_escape;
  applyRanges(&body, &ranges, &positions_excluded_from_escape);

  //qInfo() << body;
  HTMLescapeString(&body, &positions_excluded_from_escape);
  //qInfo() << body;

  // now do the emoji
  std::vector<std::pair<unsigned int, unsigned int>> emoji_pos = getEmojiPos(body);

  // check if body is only emoji
  bool all_emoji = true;
  if (emoji_pos.size() > 5)
    all_emoji = false; // could technically still be only emoji, but it gets a bubble in html
  else
  {
    for (unsigned int i = 0, posidx = 0; i < body.size(); ++i)
    {
      if (posidx >= emoji_pos.size() || i != emoji_pos[posidx].first)
      {
        if (body[i] == ' ') // spaces dont count
          continue;
        all_emoji = false;
        break;
      }
      else // body[i] == pos[posidx].first
        i += emoji_pos[posidx++].second - 1; // minus 1 for the ++i in loop
    }
  }

  // surround emoji with span
  std::string pre = "<span class=\"" + (all_emoji ? "jumbo-"s : "") + "emoji\">";
  std::string post = "</span>";
  int moved = 0;
  for (auto const &p : emoji_pos)
  {
    if (positions_excluded_from_escape.find(p.first) == positions_excluded_from_escape.end()) [[likely]]
    {
      body.insert(p.first + moved, pre);
      body.insert(p.first + p.second + pre.size() + moved, post);
      moved += pre.size() + post.size();
    }
  }

  //qInfo() << body;

  //QTextCursor cursor(this);
  //cursor.insertText(QString::fromStdString(body));
  QTextDocument::setHtml(QString::fromStdString(body));
}

inline int EmojiTextDocument::bytesToUtf8CharSize(std::string const &body, int idx) const
{
  if ((static_cast<uint8_t>(body[idx]) & 0b10000000) == 0b00000000)
    return 1;
  else if ((static_cast<uint8_t>(body[idx]) & 0b11100000) == 0b11000000) // 2 byte char
    return 2;
  else if ((static_cast<uint8_t>(body[idx]) & 0b11110000) == 0b11100000) // 3 byte char
    return 3;
  else if ((static_cast<uint8_t>(body[idx]) & 0b11111000) == 0b11110000) // 4 byte char
    return 4;
  else
    return 1;
}

inline int EmojiTextDocument::utf16CharSize(std::string const &body, int idx) const
{
  // get code point
  uint32_t codepoint = 0;
  if ((static_cast<uint8_t>(body[idx]) & 0b11111000) == 0b11110000) // 4 byte char
    /*
    codepoint =
      (static_cast<uint8_t>(body[idx]) & 0b00000111) << 18 |
      (static_cast<uint8_t>(body[idx + 1]) & 0b00111111) << 12 |
      (static_cast<uint8_t>(body[idx + 2]) & 0b00111111) << 6 |
      (static_cast<uint8_t>(body[idx + 3]) & 0b00111111);
    */
    return 2; // all 4 byte utf8 chars are 2 bytes in utf16
  else if ((static_cast<uint8_t>(body[idx]) & 0b11110000) == 0b11100000) // 3 byte char
    codepoint =
      (static_cast<uint8_t>(body[idx]) & 0b00001111) << 12 |
      (static_cast<uint8_t>(body[idx + 1]) & 0b00111111) << 6 |
      (static_cast<uint8_t>(body[idx + 2]) & 0b00111111);
  /*
  else if ((static_cast<uint8_t>(body[idx]) & 0b11100000) == 0b11000000) // 2 byte char
    codepoint =
      (static_cast<uint8_t>(body[idx]) & 0b00011111) << 6 |
      (static_cast<uint8_t>(body[idx + 1]) & 0b00111111);
  else
    codepoint = static_cast<uint8_t>(body[idx]);
  */
  else // all 1 and two byte utf-8 chars are 1 utf-16 char (max is 0b11111111111 which < 0x10000)
    return 1;

  return codepoint >= 0x10000 ? 2 : 1;
}

inline int EmojiTextDocument::numBytesInUtf16Substring(std::string const &text, unsigned int idx, int length) const
{
  int utf16count = 0;
  int bytecount = 0;

  while (utf16count < length && idx < text.size())
  {
    utf16count += utf16CharSize(text, idx);
    int utf8size = bytesToUtf8CharSize(text, idx);
    bytecount += utf8size;
    idx += utf8size;
  }
  return bytecount;
}

#endif

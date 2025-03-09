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

#ifndef SEARCHHIGHLIGHTER_H_
#define SEARCHHIGHLIGHTER_H_

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class SearchHighlighter : public QSyntaxHighlighter
{
  QTextCharFormat d_highlightformat;
  QRegularExpression d_regex;
 public:
  inline SearchHighlighter(QTextDocument *doc, QRegularExpression const &regex);
  //inline SearchHighlighter(QTextDocument *doc, QString const &regex);
  //inline SearchHighlighter(QTextDocument *doc);
  void setRegex(QString const &regex);
 protected:
  void highlightBlock(const QString &text);
};

inline SearchHighlighter::SearchHighlighter(QTextDocument *doc, QRegularExpression const &regex)
  :
  QSyntaxHighlighter(doc),
  d_regex(regex)
{
  //d_highlightformat.setFontWeight(QFont::Bold);
  d_highlightformat.setBackground(Qt::yellow);
  d_highlightformat.setForeground(Qt::black);
  d_highlightformat.setFontUnderline(true);
}

// inline SearchHighlighter::SearchHighlighter(QTextDocument *doc, QString const &regex)
//   :
//   SearchHighlighter(doc, QRegularExpression{regex, QRegularExpression::CaseInsensitiveOption})
// {}

// inline SearchHighlighter::SearchHighlighter(QTextDocument *doc)
//   :
//   SearchHighlighter(doc, "")
// {}

inline void SearchHighlighter::setRegex(QString const &regex)
{
  d_regex = QRegularExpression(regex, QRegularExpression::CaseInsensitiveOption);
}

#endif

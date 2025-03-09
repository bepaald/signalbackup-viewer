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

#ifndef COMMON_FE_H_
#define COMMON_FE_H_

#include <string>
#include <array>
#include <tuple>
#include <memory>
#include <utility>

#include <Qt>
#include <QStandardItem>

#include "mediaplayer/mediaplayer.h"
#include "msgattachment/msgattachment.h"

//class MutexProtectedItem;

namespace bepaald
{
  struct Quote
  {
    uint64_t id;
    QString author;
    QString body;
    //int64_t attachment;
    int64_t missing;
  };

  struct AttachmentData
  {
    QStandardItem *item; // warning, maybe switch this to normal item?, then remove forward dec above
    QVariant data;
  };

  struct BinaryFileData
  {
    unsigned char *data;
    uint64_t size;
  };

  struct GenericFile
  {
    QString filename;
    QString size;
    QString extension;
  };

  enum MSGDIRECTION : int
  {
   INCOMING,
   OUTGOING
  };

  enum
  {
   // shared
   IsGroupThreadRole = Qt::UserRole + 1,
   MsgTypeRole,

   // thread
   PartDataRole,
   AttachmentRole, // this can go

   QuoteAttachmentRole,
   ReadReceiptCountRole,
   DeliveryReceiptCountRole,
   DateTimeRole,
   DateMessageRole,
   QuoteRole,
   MmsIdRole,
   SmsIdRole,
   ItemCompleteRole, // this can go?
   MsgDeletedRole,

   // thread selection
   AvatarRole,
   SnippetRole,
   MultiMediaSnippetRole,
   ThreadIdRole,
   RecipientIdRole,


   AttachmentRowIdRole,
   //AttachmentUniqueIdRole,
   AttachmentTypeRole,
   AttachmentFilenameRole,
   AttachmentSizeRole,
   AttachmentQuoteRole,
   AttachmentCaptionRole,
   AttachmentDataCountRole,
   QueuedRole,
   AttachmentDataRole // QPixmap, QMovie, whatever... ALWAYS LAST
   // NOTHING FOLLOWS DATAROLE!!!
  };

  std::array<std::string_view, 15> constexpr color_palette_names
  {{
      "red",
      "pink",
      "purple",
      "deep_purple",
      "indigo",
      "blue",
      "light_blue",
      "cyan",
      "teal",
      "green",
      "light_green",
      "orange",
      "deep_orange",
      "amber",
      "blue_grey"
  }};

  std::array<char[8], 15> constexpr color_palette_dark
  {
      "#D32F2F", // RED_700
      "#C2185B", // PINK_700
      "#7B1FA2", // PURPLE_700
      "#512DA8", // DEEP_PURPLE_700
      "#303F9F", // INDIGO_700
      "#1976D2", // BLUE_700
      "#0288D1", // LIGHT_BLUE_700
      "#0097A7", // CYAN_700
      "#00796B", // TEAL_700
      "#388E3C", // GREEN_700
      "#689F38", // LIGHT_GREEN_700
      "#F57C00", // ORANGE_700
      "#E64A19", // DEEP_ORANGE_700
      "#FFA000", // AMBER_700
      "#455A64"  // BLUE_GREY_700
  };

  std::array<char[8], 15> constexpr color_palette
  {
      "#EF5350", // RED_400
      "#EC407A", // PINK_400
      "#AB47BC", // PURPLE_400
      "#7E57C2", // DEEP_PURPLE_400
      "#5C6BC0", // INDIGO_400
      "#2196F3", // BLUE_500
      "#03A9F4", // LIGHT_BLUE_500
      "#00BCD4", // CYAN_500
      "#009688", // TEAL_500
      "#4CAF50", // GREEN_500
      "#7CB342", // LIGHT_GREEN_600
      "#FF9800", // ORANGE_500
      "#FF5722", // DEEP_ORANGE_500
      "#FFB300", // AMBER_600
      "#607D8B"  // BLUE_GREY_500
  };
  char constexpr GREY[] = "#9E9E9E"; // GREY_500
  char constexpr quote_overlay_incoming[] = "#aaffffff";

  // outgoing quote background: overlay #44BDBDBD

  inline int32_t javaStringHash(std::string const &str);
  inline QString getColor(QString const &name, bool dark = false);
  inline QString getColorByName(QString const &colorname, bool dark = false);
}

//Q_DECLARE_METATYPE(unsigned char*) // maybe not nec?
//Q_DECLARE_METATYPE(MsgAttachment)
Q_DECLARE_METATYPE(std::shared_ptr<MediaPlayer>)  // used for audio attachment
Q_DECLARE_METATYPE(bepaald::Quote)
Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QList<QString>)
Q_DECLARE_METATYPE(QList<QSize>)
Q_DECLARE_METATYPE(QList<bool>)
Q_DECLARE_METATYPE(QVector<QVariant>)
Q_DECLARE_METATYPE(std::shared_ptr<bepaald::AttachmentData>) // used for all attachments
Q_DECLARE_METATYPE(std::shared_ptr<QMovie>) // used for gif attachments
Q_DECLARE_METATYPE(bepaald::BinaryFileData) // used for binary file attachment

inline int32_t bepaald::javaStringHash(std::string const &str)
{
  int32_t h = 0;
  for (char const c : str)
  {
    h = ((h << 5) - h) + c;
    //h &= h; // Convert to 32bit integer???? is this not a noop?
  }
  return h;
}

inline QString bepaald::getColor(QString const &name, bool dark)
{
  return (name.isEmpty()) ?
    GREY :
    dark ? color_palette_dark[std::abs(javaStringHash(name.toStdString()) % static_cast<int>(bepaald::color_palette.size()))]
    : color_palette[std::abs(javaStringHash(name.toStdString()) % static_cast<int>(bepaald::color_palette.size()))];
}

inline QString bepaald::getColorByName(QString const &colorname, bool dark)
{
  for (uint i = 0; i < color_palette_names.size(); ++i)
    if (color_palette_names[i] == colorname.toStdString())
      return dark ? color_palette_dark[i] : color_palette[i];
  return "#315FF4";
}

#endif

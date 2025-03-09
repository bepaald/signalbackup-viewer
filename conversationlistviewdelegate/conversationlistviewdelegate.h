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

#ifndef CONVERSATIONLISTVIEWDELEGATE_H_
#define CONVERSATIONLISTVIEWDELEGATE_H_

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QPalette>
#include <QAbstractTextDocumentLayout>
#include <QPixmap>
#include <QMovie>

#include "../signalviewdelegatebase/signalviewdelegatebase.h"
#include "../searchhighlighter/searchhighlighter.h"
#include "../msgattachment/msgattachment.h"
//#include "../conversationsqlmodel/conversationsqlmodel.h"

class ConversationListViewDelegate : public QAbstractItemDelegate, protected SignalViewDelegateBase
{
  static bool constexpr showhelplines = false;
  //ConversationSqlModel *d_model;
  int d_radius;
  int d_headerpadding;
  int d_toppadding;
  int d_bottompadding;
  int d_leftpadding;
  int d_rightpadding;
  int d_verticalmargin;
  int d_horizontalmargin;
  int d_pointerwidth;
  int d_pointerheight;
  int d_minimumwidth;
  int d_groupavatarsize;
  QPixmap d_binarypm;
  QPixmap d_received;
  QPixmap d_sent_not_received;
  QPixmap d_status_group;
  QPixmap d_status_security;
  QPixmap d_status_call_made;
  QPixmap d_status_call_received;
  QPixmap d_status_call_missed;
  QPixmap d_status_joined_signal;
  QPixmap d_status_update_group_role;
  QPixmap d_playbutton;
  QPixmap d_pausebutton;
  int d_mediabuttonsize;
  QFont d_bodyfont;
  QFont d_statusbodyfont;
  QFont d_groupheaderfont;
  QFont d_footerfont;
  QFont d_datetimemessagefont;
  int d_checkmarkpadding;
  float d_widthfraction;
  QRegularExpression d_regex;
 public:
  explicit inline ConversationListViewDelegate(QObject *parent = nullptr);
  inline void setRegex(QRegularExpression const &regex);
  //inline void setModel(ConversationSqlModel *model);
 protected:
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const;
  QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const;
  bool editorEvent(QEvent *event, QAbstractItemModel *model, QStyleOptionViewItem const &option, QModelIndex const &index);
 private:
  inline void cropPixmapSquare(QPixmap *img) const;
  QString getStatusMessage(QModelIndex const &index) const;
  bool isOutgoing(long type) const;
  QSize datemessageSizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const;
  QSize statusmessageSizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const;
  void datemessagePaint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const;
  void statusmessagePaint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const;
  QSize quoteSizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const;
  QSize scaledToWidth(qreal target, QSize const &size) const;
};

inline ConversationListViewDelegate::ConversationListViewDelegate(QObject *parent)
  :
  QAbstractItemDelegate(parent),
  //d_model(nullptr),
  d_radius(5),
  d_headerpadding(5),
  d_toppadding(3),
  d_bottompadding(3),
  d_leftpadding(5),
  d_rightpadding(5),
  d_verticalmargin(15),
  d_horizontalmargin(10),
  d_pointerwidth(10),
  d_pointerheight(17),
  d_minimumwidth(300),
  d_groupavatarsize(51),
  d_binarypm(":/document"),
  d_received(":/received"),
  d_sent_not_received(":/sent"),
  d_status_group(":/group"), // create resource for this
  d_status_security(":/security"),
  d_status_call_made(":/call_made"),
  d_status_call_received(":/call_received"),
  d_status_call_missed(":/call_missed"),
  d_status_joined_signal(":/joined"),
  d_status_update_group_role(":/update_group"),
  d_playbutton(":/play"),
  d_pausebutton(":/pause"),
  d_mediabuttonsize(72),
  d_bodyfont("Roboto", 12),
  d_statusbodyfont("Roboto", 12, -1, true),
  d_groupheaderfont("Roboto", 9, QFont::Bold),
  d_footerfont("Roboto", 9),
  d_datetimemessagefont("Roboto", 9),
  d_checkmarkpadding(2),
  d_widthfraction(.85)
{}

inline void ConversationListViewDelegate::setRegex(QRegularExpression const &regex)
{
  d_regex = regex;
}

// inline void ConversationListViewDelegate::setModel(ConversationSqlModel *model)
// {
//   d_model = model;
// }

inline void ConversationListViewDelegate::cropPixmapSquare(QPixmap *img) const
{
  if (img->width() != img->height())
  {
    QRect crop(0, 0, img->width(), img->height());
    if (img->width() > img->height())
      crop.setRect((img->width() - img->height()) / 2, 0, img->height(), img->height());
    else // if (img->height() > img->width())
      crop.setRect(0, (img->height() - img->width()) / 2, img->width(), img->width());

    //qInfo() << "Cropping imgthumb, was: " << img->size();
    *img = std::move(img->copy(crop));
    //qInfo() << "After: " << img->size();
  }
}

#endif

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

#ifndef THREADVIEWDELEGATE_H_
#define THREADVIEWDELEGATE_H_

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QPen>

#include "../signalviewdelegatebase/signalviewdelegatebase.h"
#include "../shared/framewithattachment/framewithattachment.h"

class ThreadViewDelegate : public QAbstractItemDelegate, protected SignalViewDelegateBase
{
  int d_horizontalmargin;
  int d_verticalmargin;
  int d_avatarsize;
 public:
  explicit inline ThreadViewDelegate(QObject *parent = nullptr);
  void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const;
  inline QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const;
};

inline ThreadViewDelegate::ThreadViewDelegate(QObject *parent)
  :
  QAbstractItemDelegate(parent),
  d_horizontalmargin(5),
  d_verticalmargin(5),
  d_avatarsize(60)
{}

inline QSize ThreadViewDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &) const
{
  return QSize(option.rect.width(), 2 * d_verticalmargin + d_avatarsize);
}

#endif

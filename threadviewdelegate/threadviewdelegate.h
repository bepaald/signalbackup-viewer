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

#ifndef SIGNALLISTVIEWBASE_H_
#define SIGNALLISTVIEWBASE_H_

#include <QListView>
#include <QPersistentModelIndex>
#include <QScrollBar>
#include <QVector>
#include <QPair>
#include <QMovie>
#include <QTimer>

class SignalListViewBase : public QListView
{
  qreal d_vertmargin;
  qreal d_linerightmargin;
  qreal d_linewidth;
  qreal d_handlewidth;
  //qreal d_handleheight;
  QRectF d_scrollbarhandle;
  float d_opacity;
  int d_pressx;
  int d_pressy;
  bool d_pressed;
  bool d_dragged;
  bool d_scrollbarshouldhide;
  QTimer d_hidetimer;
  QTimer d_animationtimer;
 public:
  SignalListViewBase(QWidget *parent = nullptr);
  inline virtual ~SignalListViewBase() = default;
 private slots:
  void hideScrollBar();
 protected:
  inline virtual bool eventFilter(QObject *object, QEvent *event);
  virtual void paintEvent(QPaintEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
 private:
  void ensureScrollbar();
};

#include <QDebug>
inline bool SignalListViewBase::eventFilter(QObject *object, QEvent *event)
{
  if (event->type() == QEvent::Wheel)
    ensureScrollbar();
  else if (event->type() == QEvent::Leave)
    hideScrollBar();
  return QListView::eventFilter(object, event);
}

#endif

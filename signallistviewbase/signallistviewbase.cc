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

#include "signallistviewbase.ih"

SignalListViewBase::SignalListViewBase(QWidget *parent)
  :
  QListView(parent),
  d_vertmargin(5),
  d_linerightmargin(12),
  d_linewidth(4),
  d_handlewidth(12),
  //d_handleheight(50),
  d_scrollbarhandle(0, 0, d_handlewidth, 10),
  d_opacity(0),
  d_pressed(false),
  d_dragged(false),
  d_scrollbarshouldhide(false)
{
  setMouseTracking(true);
  setEditTriggers(NoEditTriggers);
  setLayoutMode(QListView::Batched);
  setBatchSize(50);

  setResizeMode(QListView::Adjust);
  setWordWrap(true);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  //viewport()->setAutoFillBackground(true);

  // QPalette plt = viewport()->palette();
  // plt.setColor(viewport()->backgroundRole(), QColor("#EEEEEE"));
  // viewport()->setPalette(plt);

  d_hidetimer.setSingleShot(true);

  connect(&d_hidetimer, &QTimer::timeout, this, &SignalListViewBase::hideScrollBar);
  connect(&d_animationtimer, &QTimer::timeout, viewport(), static_cast<void (QWidget::*)()>(&QWidget::update));

  viewport()->installEventFilter(this);
}

#include <QDebug>

void SignalListViewBase::paintEvent(QPaintEvent *event)
{
  QListView::paintEvent(event);

  QScrollBar *realbar = verticalScrollBar();
  if (realbar->maximum() == 0)
    return;

  qreal handleheight = std::max(static_cast<qreal>(viewport()->height() * viewport()->height()) /
                                (viewport()->height() + (realbar->maximum() - realbar->minimum())),
                                2 * d_handlewidth);

  qreal realpos = static_cast<qreal>(realbar->value() - realbar->minimum()) / (realbar->maximum() - realbar->minimum());
  qreal ypos = d_vertmargin + realpos * (viewport()->height() - 2 * d_vertmargin - handleheight); // calculate ypos

  //qDebug() << "Paint calculated position from " << realbar->value();

  //qDebug() << "Moving: " << ypos - d_scrollbarhandle.top() << " pixels";

  d_scrollbarhandle.moveTopLeft({viewport()->width() - d_linerightmargin - d_handlewidth / 2, ypos});
  d_scrollbarhandle.setHeight(handleheight);

  if (d_scrollbarshouldhide)
  {
    if (d_opacity <= 0)
    {
      d_animationtimer.stop();
      d_opacity = 0;
      d_scrollbarshouldhide = false;
    }
    else
      d_opacity -= .1;
  }

  if (d_opacity > 0)
  {
    //qDebug() << "Painting at opacity = " << d_opacity;

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(QColor(0x75, 0x75, 0x75, 0x88 * d_opacity), d_linewidth, Qt::SolidLine, Qt::RoundCap));
    // draw line above handle
    if (d_vertmargin < d_scrollbarhandle.top() - d_vertmargin)
      painter.drawLine(viewport()->width() - d_linerightmargin, d_vertmargin,
                       viewport()->width() - d_linerightmargin, d_scrollbarhandle.top() - d_vertmargin);
    //draw line below handle
    if ((viewport()->height() - d_vertmargin) > d_scrollbarhandle.bottom() + d_vertmargin)
      painter.drawLine(viewport()->width() - d_linerightmargin, d_scrollbarhandle.bottom() + d_vertmargin,
                       viewport()->width() - d_linerightmargin, viewport()->height() - d_vertmargin);

    QPainterPath handle;
    handle.addRoundedRect(d_scrollbarhandle, 5, 5);
    painter.setPen(QPen(QColor(0x50, 0x50, 0x50, 0xff * d_opacity), 1, Qt::SolidLine));
    painter.drawPath(handle);
    painter.fillPath(handle, QColor(0x75, 0x75, 0x75, 0xff * d_opacity));
  }
}

void SignalListViewBase::mouseMoveEvent(QMouseEvent *event)
{
  d_hidetimer.stop();

  if ((event->globalPosition().x() >= viewport()->mapToGlobal(d_scrollbarhandle.topLeft().toPoint()).x() &&
       event->globalPosition().x() <= viewport()->mapToGlobal(d_scrollbarhandle.topRight().toPoint()).x()))
    ensureScrollbar();

  if (d_pressed && // button was pressed
      (d_dragged || // if we're dragging, we dont care that we've left the dragger....
       ((d_pressy >= viewport()->mapToGlobal(d_scrollbarhandle.topLeft().toPoint()).y() &&     // we were not dragging yet,
         d_pressy <= viewport()->mapToGlobal(d_scrollbarhandle.bottomLeft().toPoint()).y()) && // but we are inside handle
        (d_pressx >= viewport()->mapToGlobal(d_scrollbarhandle.topLeft().toPoint()).x() &&
         d_pressx <= viewport()->mapToGlobal(d_scrollbarhandle.topRight().toPoint()).x()))))
  {

    //qDebug() << "Viewport/handle top/height: " << viewport()->height() << d_scrollbarhandle.top() << d_handleheight
    //         << "Dragging: " << (event->globalPosition().y() - d_pressy) << " pixels";
    //qDebug() << "Real val: " << verticalScrollBar()->minimum() << " < " << verticalScrollBar()->value() << " < " << verticalScrollBar()->maximum();

    d_dragged = true;

    //qDebug() << "Dragging: " << (event->globalPosition().y() - d_pressy) << " pixels";

    verticalScrollBar()->setValue(verticalScrollBar()->value() +
                                  qRound((static_cast<qreal>(verticalScrollBar()->maximum() - verticalScrollBar()->minimum()) *
                                          (event->globalPosition().y() - d_pressy)) /
                                         (viewport()->height() - 2 * d_vertmargin - d_scrollbarhandle.height())));

    viewport()->update(static_cast<int>(viewport()->width() - d_vertmargin - d_linerightmargin - d_handlewidth / 2), 0,
                       d_vertmargin + d_linerightmargin + d_handlewidth / 2, viewport()->height());

    //qDebug() << "Real val: " << verticalScrollBar()->minimum() << " < " << verticalScrollBar()->value() << " < " << verticalScrollBar()->maximum();

    d_pressy = event->globalPosition().y();
  }
  else if ((event->globalPosition().x() < viewport()->mapToGlobal(d_scrollbarhandle.topLeft().toPoint()).x() ||
            event->globalPosition().x() > viewport()->mapToGlobal(d_scrollbarhandle.topRight().toPoint()).x()))
    d_hidetimer.start(60);

  if (!d_dragged)
    QListView::mouseMoveEvent(event);
}

void SignalListViewBase::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton && // clicked left button
      (event->globalPosition().x() >= viewport()->mapToGlobal(d_scrollbarhandle.topLeft().toPoint()).x() && // and clicked in scrollbar
       event->globalPosition().x() <= viewport()->mapToGlobal(d_scrollbarhandle.topRight().toPoint()).x()))
  {
    d_pressx = event->globalPosition().x();
    d_pressy = event->globalPosition().y();
    d_pressed = true;

    int globaltop = viewport()->mapToGlobal(d_scrollbarhandle.topLeft().toPoint()).y();
    if (d_pressy < globaltop ||                                                           // not in handle, only in scrollbar ->
        d_pressy > viewport()->mapToGlobal(d_scrollbarhandle.bottomLeft().toPoint()).y()) // jump move list
    {
      verticalScrollBar()->setValue(verticalScrollBar()->value() +
                                    qRound((static_cast<qreal>(verticalScrollBar()->maximum() - verticalScrollBar()->minimum()) *
                                            (d_pressy - (globaltop + d_scrollbarhandle.height() / 2))) /
                                           (viewport()->height() - 2 * d_vertmargin - d_scrollbarhandle.height())));
      viewport()->update(static_cast<int>(viewport()->width() - d_vertmargin - d_linerightmargin - d_handlewidth / 2), 0,
                         d_vertmargin + d_linerightmargin + d_handlewidth / 2, viewport()->height());
    }
  }
  else
    QListView::mousePressEvent(event);
}

void SignalListViewBase::mouseDoubleClickEvent(QMouseEvent *event)
{
  QModelIndex idx = QListView::indexAt(event->pos());
  emit doubleClicked(idx);
}

void SignalListViewBase::mouseReleaseEvent(QMouseEvent *event)
{
  d_pressed = false;
  if (d_dragged)
  {
    d_dragged = false;

    if ((event->globalPosition().x() < viewport()->mapToGlobal(d_scrollbarhandle.topLeft().toPoint()).x() || // outside srollbar?
         event->globalPosition().x() > viewport()->mapToGlobal(d_scrollbarhandle.topRight().toPoint()).x()))
      d_hidetimer.start(60);
  }
  else
    QListView::mouseReleaseEvent(event);
}

void SignalListViewBase::ensureScrollbar()
{
  d_animationtimer.stop();
  d_scrollbarshouldhide = false;
  d_opacity = 1;
  viewport()->update(static_cast<int>(viewport()->width() - d_vertmargin - d_linerightmargin - d_handlewidth / 2), 0,
                     d_vertmargin + d_linerightmargin + d_handlewidth / 2, viewport()->height());
}

void SignalListViewBase::hideScrollBar()
{
  d_animationtimer.stop();
  d_scrollbarshouldhide = true;
  d_animationtimer.start(50);
}

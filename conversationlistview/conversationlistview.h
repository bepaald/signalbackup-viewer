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

#ifndef CONVERSATIONLISTVIEW_H_
#define CONVERSATIONLISTVIEW_H_

#include <QPainter>
//#include <QScroller>
#include <QPropertyAnimation>

#include "../signallistviewbase/signallistviewbase.h"
#include "../conversationsqlmodel/conversationsqlmodel.h"
#include "../conversationlistviewdelegate/conversationlistviewdelegate.h"
#include "../msgattachment/msgattachment.h"

class ConversationListView : public SignalListViewBase
{
  Q_OBJECT;

  ConversationSqlModel *d_model;

  bool d_listreallypainted;

  QModelIndexList d_searchresults;
  int d_currentresultsidx;
  QPropertyAnimation *d_scrollanimation;

  QList<QMetaObject::Connection> d_mediaplayerconnections;

  template <typename T>
  struct fail : std::false_type
  {};

  bool d_loading;
  ConversationListViewDelegate *d_conversationdelegate;

 public:
  explicit ConversationListView(QWidget *parent = nullptr);
  inline virtual ~ConversationListView();

  ConversationListView(ConversationListView const &other) = delete;
  ConversationListView(ConversationListView &&other) = delete;
  ConversationListView operator=(ConversationListView const &other) = delete;
  ConversationListView &operator=(ConversationListView &&other) = delete;

  inline void setConversationModel(ConversationSqlModel *model);

 public slots:
  inline void search(QRegularExpression const &);
  inline void goToPreviousSearchResult();
  inline void goToNextSearchResult();

 private slots:
  inline void updateViewport();
  void indexSelected(QModelIndex const &idx);
  void details(QModelIndex const &idx);
  void handleAttachmentView();
  inline void connectGif(std::shared_ptr<QMovie> const &gif);
  inline void mediaReachedEnd();
  inline void newConversationLoading();
  inline void newConversationSet();

 protected:
  inline virtual bool eventFilter(QObject *object, QEvent *event);
  inline void paintEvent(QPaintEvent *e);

 signals:
  void setTotal(int);
  void setCurrent(int);
  void showDetails(QVector<QString> const &, QVariantList const &);
  void clearSearch();
};

#include <QDebug>

inline ConversationListView::~ConversationListView()
{
  qInfo() << "~ConversationListView() starting.";

  d_scrollanimation->stop();
  d_scrollanimation->disconnect();
  delete d_scrollanimation;

  for (auto c : d_mediaplayerconnections)
    if (c)
    {
      qInfo() << "Disconnecting active connection!";
      QObject::disconnect(c);
      qApp->processEvents();
    }

  if (d_model)
    d_model->disconnect();

  qInfo() << "~ConversationListView() done.";
}

inline void ConversationListView::updateViewport()
{
  viewport()->update();
}

inline void ConversationListView::mediaReachedEnd()
{
  reinterpret_cast<MediaPlayer *>(QObject::sender())->stop();
  updateViewport();
}

inline void ConversationListView::goToPreviousSearchResult()
{
  if (!d_searchresults.isEmpty())
  {
    --d_currentresultsidx;
    if (d_currentresultsidx < 0)
      return;

    d_scrollanimation->stop();
    d_scrollanimation->setStartValue(verticalScrollBar()->value());
    QRect targetrect = visualRect(d_searchresults[d_currentresultsidx]);
    int targetvalue = verticalScrollBar()->value() + ((targetrect.top() + targetrect.height() / 2.) - viewport()->height() / 2.);
    d_scrollanimation->setEndValue(targetvalue);

    d_scrollanimation->setDuration(std::min(std::abs(verticalScrollBar()->value() - targetvalue) / 3, 500));

    d_scrollanimation->start();
    qDebug() << "SCROLLING FROM ConversationListView::search() to" << targetvalue << "in" << d_scrollanimation->duration() << "ms";
    qDebug() << "Targetitem: " << d_searchresults[d_currentresultsidx].data(Qt::DisplayRole);
  }
  emit setCurrent(d_currentresultsidx);
}

inline void ConversationListView::goToNextSearchResult()
{
  if (!d_searchresults.isEmpty())
  {
    ++d_currentresultsidx;
    if (d_currentresultsidx >= d_searchresults.size())
      return;

    //qDebug() << "Emitting current: " << d_currentresultsidx;
    emit setCurrent(d_currentresultsidx);

    QRect viewportrect = viewport()->visibleRegion().boundingRect();
    viewportrect.setHeight(viewportrect.height() / 2);

    // if item is not yet visible in top half of viewport
    if (!viewportrect.intersects(visualRect(d_searchresults[d_currentresultsidx])))
    {
      d_scrollanimation->stop();
      d_scrollanimation->setStartValue(verticalScrollBar()->value());
      QRect targetrect = visualRect(d_searchresults[d_currentresultsidx]);
      int targetvalue = verticalScrollBar()->value() + ((targetrect.top() + targetrect.height() / 2.) - viewport()->height() / 2.);
      d_scrollanimation->setEndValue(targetvalue);

      d_scrollanimation->setDuration(std::min(std::abs(verticalScrollBar()->value() - targetvalue) / 3, 500));

      d_scrollanimation->start();
      qDebug() << "SCROLLING FROM ConversationListView::search() to" << targetvalue << "in" << d_scrollanimation->duration() << "ms";
      qDebug() << "Targetitem: " << d_searchresults[d_currentresultsidx].data(Qt::DisplayRole);


      // qDebug() << "value" << verticalScrollBar()->value();
      // qDebug() << "viewport height" << viewport()->height();
      // qDebug() << "target rect" << targetrect;
      // qDebug() << "target val" << targetvalue;
      // scrollTo(d_searchresults[d_currentresultsidx], QListView::PositionAtCenter);
      // qDebug() << "final value: " << verticalScrollBar()->value();
    }
  }
}

inline void ConversationListView::search(QRegularExpression const &term)
{
  // visualiazation of search term in item painter
  reinterpret_cast<ConversationListViewDelegate *>(itemDelegate())->setRegex(term);
  viewport()->update();

  // search
  d_searchresults = d_model->match(term);
  //qDebug() << "Emitting total: " << d_searchresults.size();
  emit setTotal(d_searchresults.size());

  // get current top item, make sure goToNextSearchResult() goes to next search result
  QModelIndex idx = indexAt({0, 0});
  if (idx.isValid())
  {
    d_currentresultsidx = -1;
    for (int i = 0; i < d_searchresults.size(); ++i)
      if (d_searchresults[i].row() >= idx.row())
      {
        d_currentresultsidx = i - 1;
        break;
      }
  }
  goToNextSearchResult();
}


inline bool ConversationListView::eventFilter(QObject *object, QEvent *event)
{
  if (event->type() == QEvent::Resize ||
      event->type() == QEvent::Show)
    //controlMovies();
    handleAttachmentView();

  //qInfo() << "GOT EVENT: " << event;

  return SignalListViewBase::eventFilter(object, event);
}

inline void ConversationListView::setConversationModel(ConversationSqlModel *model)
{
  setModel(model);
  d_model = model;

  connect(d_model, &ConversationSqlModel::startLoadingNewConversation, this, &ConversationListView::newConversationLoading);
  connect(d_model, &ConversationSqlModel::finishedLoadingNewConversation, this, &ConversationListView::newConversationSet);
  connect(d_model, &ConversationSqlModel::gifAvailable, this, &ConversationListView::connectGif);
  connect(d_model, &ConversationSqlModel::viewportShouldUpdate, this, &ConversationListView::updateViewport);
  //d_conversationdelegate->setModel(d_model);
  //connect(d_conversationdelegate, &ConversationListViewDelegate::paintingItemAtIdx, d_model, itemInView);
}

inline void ConversationListView::connectGif(std::shared_ptr<QMovie> const &gif)
{
  connect(gif.get(), &QMovie::frameChanged, this, &ConversationListView::updateViewport);
  gif->start();
}

inline void ConversationListView::newConversationLoading()
{
  qDebug() << "Signalled of new conversation!";

  // resetting search stuff
  emit clearSearch();
  d_searchresults.clear();
  d_currentresultsidx = 0;
  reinterpret_cast<ConversationListViewDelegate *>(itemDelegate())->setRegex(QRegularExpression());
}

inline void ConversationListView::newConversationSet()
{
  qDebug() << "Signalled conversation finished loading!";
  d_listreallypainted = false;
}

inline void ConversationListView::paintEvent(QPaintEvent *e)
{
  //qInfo() << "painting: " << e->rect() << " " << e->region();
  if (d_listreallypainted == false &&
      indexAt(QPoint(0, 0)).isValid())
  {
    handleAttachmentView();
    d_listreallypainted = true;
  }

  return SignalListViewBase::paintEvent(e);
}

#endif

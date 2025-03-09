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

#ifndef SBVMAINWINDOW_H_
#define SBVMAINWINDOW_H_

#include <QMainWindow>

#include "../messagedetailswidget/messagedetailswidget.h"
#include "../searchwidget/searchwidget.h"
#include "../conversationlistview/conversationlistview.h"

class SBVMainWindow : public QMainWindow
{
  SearchWidget *d_searchwidget;
  MessageDetailsWidget *d_detailswidget;
 public:
  inline SBVMainWindow(ConversationListView *conversationlistview, QWidget *parent = nullptr);
  inline void enableSearchBox();
 protected:
  void keyPressEvent(QKeyEvent *event);
};

inline SBVMainWindow::SBVMainWindow(ConversationListView *conversationlistview, QWidget *parent)
  :
  QMainWindow(parent),
  d_searchwidget(new SearchWidget(this)),
  d_detailswidget(new MessageDetailsWidget(this))
{
  addDockWidget(Qt::BottomDockWidgetArea, d_searchwidget);
  d_searchwidget->hide();

  addDockWidget(Qt::RightDockWidgetArea, d_detailswidget);
  d_detailswidget->hide();

  connect(d_searchwidget, &SearchWidget::newSearch, conversationlistview, &ConversationListView::search);
  connect(d_searchwidget, &SearchWidget::next, conversationlistview, &ConversationListView::goToNextSearchResult);
  connect(d_searchwidget, &SearchWidget::prev, conversationlistview, &ConversationListView::goToPreviousSearchResult);

  connect(conversationlistview, &ConversationListView::setTotal, d_searchwidget, &SearchWidget::setTotal);
  connect(conversationlistview, &ConversationListView::setCurrent, d_searchwidget, &SearchWidget::setCurrent);
  connect(conversationlistview, &ConversationListView::clearSearch, d_searchwidget, &SearchWidget::clearAll);

  connect(conversationlistview, &ConversationListView::showDetails, [=, this]{ d_detailswidget->show(); });
  connect(conversationlistview, &ConversationListView::showDetails, d_detailswidget, &MessageDetailsWidget::setData);
}

inline void SBVMainWindow::enableSearchBox()
{
  d_searchwidget->enableSearchBox();
}

#endif

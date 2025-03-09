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

#include "sbvmainwindow.ih"

void SBVMainWindow::keyPressEvent(QKeyEvent *event)
{
  if (event->matches(QKeySequence::Find))
    d_searchwidget->show();
  else if (event->matches(QKeySequence::Cancel))
    d_searchwidget->hide();
  else
    QMainWindow::keyPressEvent(event);
}

#include "messagedetailswidget.ih"

MessageDetailsWidget::MessageDetailsWidget(QWidget *parent)
  :
  QDockWidget(parent),
  d_layout(new QGridLayout(this))
{
  setAllowedAreas(Qt::RightDockWidgetArea);
  setFeatures(QDockWidget::DockWidgetClosable);
  QWidget *mainwidget = new QWidget(this);
  mainwidget->setLayout(d_layout);
  setWidget(mainwidget);
}

void MessageDetailsWidget::setData(QVector<QString> const &fields, QVariantList const &values)
{
  // clear layout
  while (auto item = d_layout->takeAt(0))
    delete item;

  // set new data
  for (int i = 0; i < values.size(); ++i)
  {
    d_layout->addWidget(new QLabel(fields[i], this), i, 0);
    d_layout->addWidget(new QLabel(values[i].toString(), this), i, 1);
  }
}

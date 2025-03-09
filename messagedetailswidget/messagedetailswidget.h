#ifndef MESSAGEDETAILSWIDGET_H_
#define MESSAGEDETAILSWIDGET_H_

#include <QDockWidget>

class QGridLayout;

class MessageDetailsWidget : public QDockWidget
{
  Q_OBJECT;

  QGridLayout *d_layout;

 public:
  MessageDetailsWidget(QWidget *parent = nullptr);

 public slots:
  void setData(QVector<QString> const &fields, QVariantList const &values);
};

#endif

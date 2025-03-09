#ifndef CONVERSATIONMODELITEM_H_
#define CONVERSATIONMODELITEM_H_

#include <QStandardItem>
#include <QMap>
#include <QVariant>

#include "common_fe.h"

class ConversationModelItem : public QStandardItem
{
  QMap<int, QVariant> d_customdata;
 public:
  //inline virtual ~ConversationModelItem();
  virtual QVariant data(int role = Qt::UserRole + 1) const;
  virtual void setData(QVariant const &value, int role = Qt::UserRole + 1);
};

/*
inline ConversationModelItem::~ConversationModelItem()
{
  qInfo() << "Destroying myitem";
  if (d_customdata.contains(Qt::UserRole + 5))
    delete d_customdata[Qt::UserRole + 5].value<MyCustomObject *>();
}
*/

#endif

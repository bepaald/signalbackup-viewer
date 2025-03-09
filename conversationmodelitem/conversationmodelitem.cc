#include "conversationmodelitem.ih"

inline QVariant ConversationModelItem::data(int role) const
{
  if (role >= bepaald::QueuedRole)
    return d_customdata[role];
  return QStandardItem::data(role);
}

inline void ConversationModelItem::setData(QVariant const &value, int role)
{
  //qInfo() << "Calling ConversationModelItem::setData()";
  if (role >= bepaald::QueuedRole)
  {
    //if (role == Qt::UserRole + 5) // this role holds pointer data
    //  if (d_customdata.contains(Qt::UserRole + 5))
    //    delete d_customdata[Qt::UserRole + 5].value<MyCustomObject *>();
    d_customdata[role] = value;
    return;
  }
  QStandardItem::setData(value, role);
}

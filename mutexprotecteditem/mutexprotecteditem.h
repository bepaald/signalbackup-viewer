#ifndef MUTEXPROTECTEDITEM_H_
#define MUTEXPROTECTEDITEM_H_

#include <QStandardItem>
#include <QMutex>
#include <QMutexLocker>

class MutexProtectedItem
{
  QMutex d_mutex;
  QStandardItem *d_item;
 public:
  inline MutexProtectedItem();
  inline MutexProtectedItem(QStandardItem *item);
  // inline ~MutexProtectedItem(); ???
  inline QStandardItem *standardItem() const;
  inline void setData(QVariant const &data, int role);
  inline QVariant data(int role);
  inline void setEnabled(bool val);
  inline int row();
};

inline MutexProtectedItem::MutexProtectedItem()
  :
  d_item(new QStandardItem)
{}

inline MutexProtectedItem::MutexProtectedItem(QStandardItem *item)
  :
  d_item(item)
{}

inline QStandardItem *MutexProtectedItem::standardItem() const
{
  return d_item;
}

inline void MutexProtectedItem::setData(QVariant const &data, int role)
{
  d_mutex.lock();
  d_item->setData(data, role);
  d_mutex.unlock();
}

inline QVariant MutexProtectedItem::data(int role)
{
  QMutexLocker locker(&d_mutex);
  return d_item->data(role);
}

inline void MutexProtectedItem::setEnabled(bool val)
{
  d_mutex.lock();
  d_item->setEnabled(val);
  d_mutex.unlock();
}

inline int MutexProtectedItem::row()
{
  QMutexLocker locker(&d_mutex);
  return d_item->row();
}
#endif

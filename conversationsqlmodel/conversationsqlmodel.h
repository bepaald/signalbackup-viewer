#ifndef CONVERSATIONSQLMODEL_H_
#define CONVERSATIONSQLMODEL_H_

#include <string>

#include <QStandardItemModel>

#include <QStandardItem>

#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>
#include <QMovie>
#include <QPair>
#include <QVector>
#include <QBuffer>
#include <QThread>
#include <QQueue>
#include <QMutex>

#include "../conversationmodelitem/conversationmodelitem.h"
#include "../mutexprotecteditem/mutexprotecteditem.h"
#include "../msgattachment/msgattachment.h"
#include "../mediaplayer/mediaplayer.h"
#include "../shared/backupframe/backupframe.h"
#include "../shared/framewithattachment/framewithattachment.h"
#include "../common_fe.h"
#include "../shared/common_be.h"

#include <QDebug>
#include <chrono>

class ConversationSqlModel : public QStandardItemModel
{
  Q_OBJECT;

  /*  enum SQLFIELDS
  {
   SMS_ID = 0, SMS_THREAD_ID, SMS_ADDRESS, SMS_ADDRESS_DEVICE_ID, SMS_PERSON, SMS_DATE, SMS_DATE_SENT, SMS_PROTOCOL, SMS_READ, SMS_STATUS, SMS_TYPE, SMS_REPLY_PATH_PRESENT, SMS_DELIVERY_RECEIPT_COUNT, SMS_SUBJECT, SMS_BODY, SMS_MISMATCHED_IDENTITIES, SMS_SERVICE_CENTER, SMS_SUBSCRIPTION_ID, SMS_EXPIRES_IN, SMS_EXPIRE_STARTED, SMS_NOTIFIED, SMS_READ_RECEIPT_COUNT,
   MMS_ID, MMS_THREAD_ID, MMS_DATE, MMS_DATE_RECEIVED, MMS_MSG_BOX, MMS_READ, MMS_M_ID, MMS_SUB, MMS_SUB_CS, MMS_BODY, MMS_PART_COUNT, MMS_CT_T, MMS_CT_L, MMS_ADDRESS, MMS_ADDRESS_DEVICE_ID, MMS_EXP, MMS_M_CLS, MMS_M_TYPE, MMS_V, MMS_M_SIZE, MMS_PRI, MMS_RR, MMS_RPT_A, MMS_RESP_ST, MMS_ST, MMS_TR_ID, MMS_RETR_ST, MMS_RETR_TXT, MMS_RETR_TXT_CS, MMS_READ_STATUS, MMS_CT_CLS, MMS_RESP_TXT, MMS_D_TM, MMS_DELIVERY_RECEIPT_COUNT, MMS_MISMATCHED_IDENTITIES, MMS_NETWORK_FAILURES, MMS_D_RPT, MMS_SUBSCRIPTION_ID, MMS_EXPIRES_IN, MMS_EXPIRE_STARTED, MMS_NOTIFIED, MMS_READ_RECEIPT_COUNT, MMS_QUOTE_ID, MMS_QUOTE_AUTHOR, MMS_QUOTE_BODY, MMS_QUOTE_ATTACHMENT, MMS_SHARED_CONTACTS, MMS_QUOTE_MISSING,
   PART_ID, PART_MID, PART_SEQ, PART_CT, PART_NAME, PART_CHSET, PART_CD, PART_FN, PART_CIN, PART_CL, PART_CTT_S, PART_CTT_T, PART_ENCRYPTED, PART_PENDING_PUSH, PART_DATA, PART_DATA_SIZE, PART_FILE_NAME, PART_THUMBNAIL, PART_ASPECT_RATIO, PART_UNIQUE_ID, PART_DIGEST, PART_FAST_PREFLIGHT_ID, PART_VOICE_NOTE, PART_DATA_RANDOM, PART_THUMBNAIL_RANDOM, PART_WIDTH, PART_HEIGHT, PART_QUOTE
   //,
   UNION_DATE, UNION_TYPE, UNION_BODY, UNION_ADDRESS, UNION_DELIVERY_RECEIPT_COUNT, UNION_READ_RECEIPT_COUNT
  };*/

  enum MSG_FIELDS : int
  {
    UNION_DATE = 0, UNION_DISPLAY_DATE, UNION_TYPE, UNION_BODY, UNION_ADDRESS, UNION_DELIVERY_RECEIPT_COUNT, UNION_READ_RECEIPT_COUNT, SMS_ID, MMS_ID
  };

  enum PART_FIELDS : int
  {
    PART_ROW_ID = 0, PART_CT, PART_FILE_NAME, PART_WIDTH, PART_HEIGHT
  };

  struct GifData
  {
    QBuffer *d_buffer;
    //QStandardItem *d_item;
    //int d_index;
    //QString d_filename;
  };
  QVector<GifData> d_gifdata;
  QVector<std::shared_ptr<QMovie>> d_giflist;
  //QVector<GifData> d_gifdata_quote;
  //QVector<MediaPlayer *> d_medialist; // delete this? it seems to be unused...
  //QVector<QPair<Something, QPersistentModelIndex>> d_medialist2;
  std::map<uint64_t, std::unique_ptr<BackupFrame>> *d_attachments; // maps row_id -> attachmentdata
  QVector<QString> d_sqlfields;
  int d_lastsmsidx;
  int d_firstmmsidx;
  int d_lastmmsidx;
  unsigned int d_rowcount;
  //QString d_loadingmsg;
  bool d_groupthread;
  QList<QModelIndex> d_indecesinview;
  QMap<QStandardItem *, int> d_attachmentsqueued;
  QThread *d_setqueryworker;
  QVector<QThread *> d_threadpool;
  QQueue<std::tuple<QStandardItem *, int, QVector<QVariantList>>> d_jobqueue2;
  QMutex d_jobqueuemutex;
  QMutex d_medialistmutex;
  QMutex d_gifdatamutex;
  std::atomic<int> d_working;
  std::atomic<bool> d_cancel;

 public:
  explicit inline ConversationSqlModel(/*QVector<QPair<QMovie *, QPersistentModelIndex>> *giflist,*/
                                       std::map<uint64_t, std::unique_ptr<BackupFrame>> *attachments,
                                       QObject *parent = nullptr);
  inline ~ConversationSqlModel();
  bool setConversation(QString const &thread_id, bool isgroupthread);
  //inline unsigned int numGifs() const;
  inline QString loadingMsg() const;
  inline unsigned int rowCount() const;
  QModelIndexList match(QRegularExpression const &regex) const;
  inline QVector<QString> const &fields() const;
  void showDetails(QModelIndex const &idx) const;
  //void itemInView(QModelIndex const &itemsinview);
  void itemsInView(QList<QModelIndex> const &itemsinview);
  void shortInfo(QModelIndex const &idx) const;

 private:
  bool setQuery(QString const &mainquerystring, QString const &partsquerystring);
  void setQueryWorker(QSqlDatabase database, QString const &mainquerstringy, QString const &partsquerystring);
  inline void disconnectGifs();
  inline void cleanUp();
  inline void cancelAllThreads();
  void deleteAttData(QStandardItem *item);

  // only for use from handleMultiMediaPart
  void t_watchJobQueueV2();
  void t_handleMultiMediaPartV2(QVariantList const &, int curidx, QStandardItem *item);
  inline unsigned char *t_rawAttachmentData(QVariantList const &partdata) const;
  inline uint64_t t_rawAttachmentSize(QVariantList const &partdata) const;

 private slots:
  inline void addAvailableItem(QStandardItem *item);
  inline void setAttachmentData(std::shared_ptr<bepaald::AttachmentData> att_data, int idx, int gifidx = -1);
 signals:
  void gifAvailable(std::shared_ptr<QMovie>);
  void viewportShouldUpdate();
  void attachmentDataAvailable(std::shared_ptr<bepaald::AttachmentData> att_data, int idx, int gifidx = -1) const;
  void startLoadingNewConversation();
  void finishedLoadingNewConversation();
  void itemAvailable(QStandardItem *item);
};

inline ConversationSqlModel::ConversationSqlModel(/*QVector<QPair<QMovie *, QPersistentModelIndex>> *giflist,*/
                                                  std::map<uint64_t, std::unique_ptr<BackupFrame>> *attachments,
                                                  QObject *parent)
  :
  QStandardItemModel(parent),
  //d_giflist(giflist),
  d_attachments(attachments),
  d_lastsmsidx(0),
  d_firstmmsidx(0),
  d_lastmmsidx(0),
  d_rowcount(0),
  d_groupthread(false),
  d_setqueryworker(nullptr),
  d_working(0),
  d_cancel(false)
{
  // the itemAvailable signal is emitted by the worker thread when new item is ready to be appended to the model by the main thread
  connect(this, &ConversationSqlModel::itemAvailable, this, &ConversationSqlModel::addAvailableItem, Qt::QueuedConnection);
  // emitted by jobqueue thread (hanleMutiMediaPart()) when attachment data is loaded
  connect(this, &ConversationSqlModel::attachmentDataAvailable, this, &ConversationSqlModel::setAttachmentData, Qt::QueuedConnection);
}

inline ConversationSqlModel::~ConversationSqlModel()
{
  qInfo() << "~ConversationSqlModel() starting.";
  disconnectGifs();
  cancelAllThreads();
  cleanUp();
  qInfo() << "~ConversationSqlModel() done.";
}

inline void ConversationSqlModel::disconnectGifs()
{
  qInfo() << "Disconnecting gifs";
  for (auto i : d_giflist)
    i->disconnect();

  qApp->processEvents();
}

inline void ConversationSqlModel::cleanUp()
{
  qInfo() << "Clean up!";

  d_jobqueuemutex.lock();
  d_jobqueue2.clear();
  d_jobqueuemutex.unlock();

  d_giflist.clear();

  d_gifdatamutex.lock();
  for (auto gd : d_gifdata)
  {
    delete &(gd.d_buffer->buffer()); // delete the qbytearray, as qbuffer has not taken ownership
    delete gd.d_buffer;
  }
  d_gifdata.clear();
  /*
  for (auto i : d_gifdata_quote)
  {
    delete &(i.d_buffer->buffer()); // delete the qbytearray, as qbuffer has not taken ownership
    delete i.d_buffer;
  }
  d_gifdata_quote.clear();
  */
  d_gifdatamutex.unlock();

  for (auto const &[id, frame] : *d_attachments)
    reinterpret_cast<FrameWithAttachment *>(frame.get())->clearData();

  d_indecesinview.clear();

  clear();
  d_rowcount = 0;
}

//#include <iostream>

// inline unsigned int ConversationSqlModel::numGifs() const
// {
//   return d_giflist.size();
// }

inline QString ConversationSqlModel::loadingMsg() const
{
  return "Loading " + QString::number(d_rowcount) + " messages...";
}

inline unsigned int ConversationSqlModel::rowCount() const
{
  return d_rowcount;
}

inline QVector<QString> const &ConversationSqlModel::fields() const
{
  return d_sqlfields;
}

inline void ConversationSqlModel::cancelAllThreads()
{
  qInfo() << "Cancelling threads!";
  d_cancel = true;
  for (auto t : d_threadpool)
    t->wait();
  if (d_setqueryworker)
    d_setqueryworker->wait();
  d_cancel = false;
  qInfo() << "Threads stopped";

  qInfo() << "clearing threadpool";
  for (QThread *t : d_threadpool)
    if (t)
      delete t;
  d_threadpool.clear();

  qInfo() << "Deleting querythread";
  if (d_setqueryworker)
    delete d_setqueryworker;

  qInfo() << "processEvents()";

  qApp->processEvents();

  qInfo() << "done!";

}

inline void ConversationSqlModel::setAttachmentData(std::shared_ptr<bepaald::AttachmentData> att_data, int idx, int gifidx)
{
  qInfo() << "Set item data from thread " << QThread::currentThreadId() << "...";

  // decrease attachments-left count, if zero remove+set unqueued, if first items add to map if multi-att
  if (d_attachmentsqueued.contains(att_data->item))
  {
    d_attachmentsqueued[att_data->item] -= 1;
    if (d_attachmentsqueued[att_data->item] == 0)
    {
      qInfo() << "All attachments for item have been handled, setting QueuedRole = false";
      d_attachmentsqueued.remove(att_data->item);
      att_data->item->setData(false, bepaald::QueuedRole);
    }
  }
  else
  {
    if (att_data->item->data(bepaald::AttachmentDataCountRole).toInt() > 1)
      d_attachmentsqueued[att_data->item] = att_data->item->data(bepaald::AttachmentDataCountRole).toInt() - 1;
    else // only one attachment, would be decremented immediately
    {
      qInfo() << "All attachments for item have been handled, setting QueuedRole = false";
      att_data->item->setData(false, bepaald::QueuedRole);
    }
  }

  if (gifidx != -1) // we are adding a gif at idx from buffer at gifidx,
  {                 // create Qmovie in main thread as it cant be moved from worker thread
    std::shared_ptr<QMovie> gif = std::make_shared<QMovie>(d_gifdata[gifidx].d_buffer);
    d_giflist.append(gif);
    gif->jumpToFrame(0);
    att_data->data = QVariant::fromValue(gif);
  }

  qInfo() << "INSERTING MSGATTACHMENT AT POSITION" << idx;
  att_data->item->setData(att_data->data, bepaald::AttachmentDataRole + idx);
  emit viewportShouldUpdate();

  if (gifidx != -1)
    emit gifAvailable(d_giflist.back());
}

inline void ConversationSqlModel::addAvailableItem(QStandardItem *item)
{
  if (item)
    appendRow(item);
  else
    emit finishedLoadingNewConversation();
}

// only for use from handleMultiMediaPart
unsigned char *ConversationSqlModel::t_rawAttachmentData(QVariantList const &partdata) const
{
  return reinterpret_cast<FrameWithAttachment *>((*d_attachments)[partdata[PART_ROW_ID].toULongLong()].get())->attachmentData();
}

// only for use from handleMultiMediaPart
uint64_t ConversationSqlModel::t_rawAttachmentSize(QVariantList const &partdata) const
{
  return reinterpret_cast<FrameWithAttachment *>((*d_attachments)[partdata[PART_ROW_ID].toULongLong()].get())->attachmentSize();
}

#endif

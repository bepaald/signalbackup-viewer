#include "conversationsqlmodel.ih"

#include "../columnnames/columnnames.h"

#include <QSqlDriver>
#include <QSqlError>
#include <QDebug>

#include <chrono>

#include <iostream>
#include <fstream>

/*
  sms.date(_sent)/mms.date(_received) union_(display_)date
  sms.type/mms.msg_box         union_type
  *ms.body                     union_body
  *ms.address                  union_address
  *ms.delivery_receipt_count   union_delivery_receipt_count
  *ms.read_receipt_count       union_read_receipt_count "
  sms._id
  mms._id

  FOR NEWER DB VERSIONS:

  mms.quote_id
  mms.quote_author
  mms.quote_body
  // mms.quote_attachment -> always -1, removed in dbv166
  mms.quote_missing
*/

/*
  part.unique_id
  part.ct
  part.file_name

  FOR NEWER DB VERSIONS:

  part.quote
*/

bool ConversationSqlModel::setConversation(QString const &thread_id, bool isgroupthread)
{
  d_groupthread = isgroupthread;

  QString query;
  if (!ColumnNames::d_sms_record.isEmpty())
  {
    query = "SELECT "
      "sms." + ColumnNames::d_sms_date_received + " AS union_date, " // not sure for outgoing
      "sms.date_sent AS union_display_date, "
      "sms.type AS union_type, "
      "sms.body AS union_body, "
      "sms." + ColumnNames::d_sms_recipient_id + " AS union_address, "
      "sms.delivery_receipt_count AS union_delivery_receipt_count, "
      "sms.read_receipt_count AS union_read_receipt_count, "
      "sms._id AS [sms._id], "
      "'' AS [mms._id] ";
    if (ColumnNames::d_message_record.contains(ColumnNames::d_mms_table + ".quote_id"))
      query += ","
        "'' AS [mms.quote_id], "
        "'' AS [mms.quote_author], "
        "'' AS [mms.quote_body], "
        "'' AS [mms.quote_missing] ";
    if (ColumnNames::d_message_record.contains(ColumnNames::d_mms_table + ".remote_deleted"))
      query += ","
        "'' AS [mms.deleted] ";
    query +=
      "FROM 'sms' WHERE " + thread_id + " = sms.thread_id "
      "UNION ";
  }
  query += "SELECT " +
    ColumnNames::d_mms_table + ".date_received AS union_date, " + // not sure for outgoing
    ColumnNames::d_mms_table + "." + ColumnNames::d_mms_date_sent + " AS union_display_date, " +
    ColumnNames::d_mms_table + "." + ColumnNames::d_mms_type + " AS union_type, " +
    ColumnNames::d_mms_table + ".body AS union_body, " +
    ColumnNames::d_mms_table + "." + ColumnNames::d_mms_recipient_id + " AS union_address, " +
    ColumnNames::d_mms_table + "." + ColumnNames::d_mms_delivery_receipts + " AS union_delivery_receipt_count, " +
    ColumnNames::d_mms_table + "." + ColumnNames::d_mms_read_receipts + " AS union_read_receipt_count, " +
    "'' AS [sms._id], " +
    ColumnNames::d_mms_table + "._id AS [mms._id] ";
  if (ColumnNames::d_message_record.contains(ColumnNames::d_mms_table + ".quote_id"))
    query += "," +
      ColumnNames::d_mms_table + ".quote_id AS [mms.quote_id], " +
      ColumnNames::d_mms_table + ".quote_author AS [mms.quote_author], " +
      ColumnNames::d_mms_table + ".quote_body AS [mms.quote_body], " +
      ColumnNames::d_mms_table + ".quote_missing AS [mms.quote_missing] ";
  if (ColumnNames::d_message_record.contains(ColumnNames::d_mms_table + ".remote_deleted"))
    query += "," +
      ColumnNames::d_mms_table + ".remote_deleted AS [mms.deleted] ";
  query +=
    "FROM '" + ColumnNames::d_mms_table + "' WHERE " + thread_id + " = " + ColumnNames::d_mms_table + ".thread_id "
    "ORDER BY union_date ASC, union_display_date DESC";

  qDebug() << query;

  QString pq = "SELECT " +
    ColumnNames::d_part_table + "._id, " +
    //ColumnNames::d_part_table + ".unique_id, " + // was never necessary to get attachments (or match them to message). removed in later databases
    ColumnNames::d_part_table + "." + ColumnNames::d_part_ct + ", " +
    ColumnNames::d_part_table + ".file_name, " +
    ColumnNames::d_part_table + ".width, " +
    ColumnNames::d_part_table + ".height ";
  if (ColumnNames::d_attachment_record.contains("part.quote"))
    pq += ", " + ColumnNames::d_part_table + ".quote ";
  if (ColumnNames::d_attachment_record.contains("part.caption"))
    pq += ", " + ColumnNames::d_part_table + ".caption ";
  pq +=
    "FROM '" + ColumnNames::d_part_table + "' WHERE " + ColumnNames::d_part_table + "." + ColumnNames::d_part_mid + " = ";

  return setQuery(query, pq);
}

bool ConversationSqlModel::setQuery(QString const &mainquerystring, QString const &partsquerystring)
{
  auto start = std::chrono::system_clock::now();

  // cancel & destroy all running threads & processEvents()
  cancelAllThreads();
  cleanUp();

  // create new threads
  if (QSqlDatabase::contains("Main thread's connection to database"))
    QSqlDatabase::removeDatabase("Main thread's connection to database");
  QSqlDatabase cloneddb = QSqlDatabase::cloneDatabase(QSqlDatabase::database(), "Main thread's connection to database");
  //d_threadpool.append(QThread::create([=, this]{setQueryWorker(cloneddb, mainquerystring, partsquerystring);}));
  d_setqueryworker = QThread::create([=, this]{setQueryWorker(cloneddb, mainquerystring, partsquerystring);});

  for (int i = 0; i < std::max(QThread::idealThreadCount(), 1); ++i)
    d_threadpool.append(QThread::create([=, this]{t_watchJobQueueV2();}));

  emit startLoadingNewConversation();

  qInfo() << "Starting setQueryWorker thread";
  d_setqueryworker->start();
  // qDebug() << "Starting" << d_threadpool.size() << "threads!";
  // for (auto t : d_threadpool)
  //   t->start();
  // qDebug() << "Threads started";

  std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
  std::cout << "SET QUERY TOOK: " << std::fixed << std::setprecision(3) << diff.count() << " SECONDS" << std::endl;

  //emit QuerySet(); // this causes gifs to be found and controlled in the view
  // currently this ONLY connects the gifs, but since the threads are
  // still doing their thing with them, its way to early

  return true;
}

void ConversationSqlModel::setQueryWorker(QSqlDatabase db, QString const &mainquerystring, QString const &partsquerystring)
{
  qDebug() << "Opening cloned db";
  if (!db.open())
  {
    // emit something;
    return;
  }

  qDebug() << "Querying";

  QSqlQuery q(db);
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
  q.setForwardOnly(true);
#endif
  if (!q.exec(mainquerystring))
  {
    qDebug() << q.lastError().text();

    // emit error?

    // emit done!

    db.close();
    return;
  }

  QString prevday = "";

  //QList<QStandardItem *> items;

  qInfo() << "RESULTS: " << q.size();

  while (q.next())
  {
    if (d_cancel)
    {
      db.close();
      return;
    }

    //qDebug() << "Got: " << q.record();

    // if day of current message != previous msg
    ConversationModelItem *extraitem = nullptr;
    if (QDateTime::fromMSecsSinceEpoch(q.value(UNION_DATE).toULongLong()).toString("yyyy-MM-dd") != prevday)
    {
      prevday = QDateTime::fromMSecsSinceEpoch(q.value(UNION_DATE).toULongLong()).toString("yyyy-MM-dd");
      extraitem = new ConversationModelItem;
      extraitem->setEnabled(false);
      extraitem->setData(QLocale::c().toString(QDateTime::fromMSecsSinceEpoch(q.value(UNION_DATE).toULongLong()), "ddd, MMM dd, yyyy"), bepaald::DateMessageRole);
      extraitem->setData(true, bepaald::ItemCompleteRole);
      ++d_rowcount;
    }

    // create index
    ConversationModelItem *item = new ConversationModelItem;
    item->setEnabled(false);
    ++d_rowcount;

    //qDebug() << "Got: " << q.record();

    //quote?
    if (q.value("mms.quote_id").isValid() && q.value("mms.quote_id").toULongLong() != 0)
    {
      bepaald::Quote quote{q.value("mms.quote_id").toULongLong(),
                           q.value("mms.quote_author").toString(),
                           q.value("mms.quote_body").toString(),
                           q.value("mms.quote_missing").isValid() ? q.value("mms.quote_missing").toLongLong() : 0};

      item->setData(QVariant::fromValue(quote), bepaald::QuoteRole);

      // qDebug() << "QUOTE:";
      // qDebug() << " - id:" << quote.id;
      // qDebug() << " - author:" << quote.author;
      // qDebug() << " - body:" << quote.body;
      // qDebug() << " - attachment:" << quote.attachment;
      // qDebug() << " - missing:" << quote.missing;
      // qDebug() << "   (msg body):" << q.value(UNION_BODY);
    }

    // set mms id
    item->setData(QVariant::fromValue(q.value(MMS_ID).toULongLong()), bepaald::MmsIdRole);

    // set sms id
    item->setData(QVariant::fromValue(q.value(SMS_ID).toULongLong()), bepaald::SmsIdRole);

    // set recipient id
    item->setData(QVariant::fromValue(q.value(UNION_ADDRESS).toString()), bepaald::RecipientIdRole);

    // set message type (in/out, sending/sent/received
    item->setData(QVariant::fromValue(q.value(UNION_TYPE).toULongLong()), bepaald::MsgTypeRole);

    // set if group thread
    item->setData(QVariant::fromValue(d_groupthread), bepaald::IsGroupThreadRole);

    // set receiptcount
    item->setData(QVariant::fromValue(q.value(UNION_READ_RECEIPT_COUNT).toULongLong()), bepaald::ReadReceiptCountRole);
    item->setData(QVariant::fromValue(q.value(UNION_DELIVERY_RECEIPT_COUNT).toULongLong()), bepaald::DeliveryReceiptCountRole);

    // message
    //qDebug() << "Preparing message: " << q.value(UNION_BODY).toString();
    //QString msgbody = prepareMessage(q.value(UNION_BODY).toString());
    //item->setData(QVariant::fromValue(msgbody), Qt::DisplayRole);

    // set message body
    item->setData(q.value(UNION_BODY).toString(), Qt::DisplayRole);

    // set isdeleted
    if (ColumnNames::d_message_record.contains(ColumnNames::d_mms_table + ".remote_deleted"))
      item->setData(QVariant::fromValue(q.value("mms.deleted").toBool()), bepaald::MsgDeletedRole);
    else
      item->setData(QVariant::fromValue(false), bepaald::MsgDeletedRole);

    // date time
    item->setData(QVariant::fromValue(QDateTime::fromMSecsSinceEpoch(q.value(UNION_DISPLAY_DATE).toULongLong()).toString("yyyy-MM-dd HH:mm:ss")), bepaald::DateTimeRole);

    // get partdata belonging to current message
    QVector<QVariantList> partdata;

    QList<int> rowids;
    //QList<int> uniqueids;
    QList<QString> types;
    QList<QString> filenames;
    QList<QSize> sizes;
    QList<bool> quotes;
    QList<QString> captions;


    if (q.value(MMS_ID).isValid() && !q.value(MMS_ID).toString().isEmpty())
    {
      //qInfo() << "QUERYING FOR PARTS";
      //qInfo() << partsquerystring + q.value(MMS_ID).toString();

      QSqlQuery partsquery(db);
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
  partsquery.setForwardOnly(true);
#endif
      if (!partsquery.exec(partsquerystring + q.value(MMS_ID).toString()))
        qDebug() << partsquery.lastError().text();
      else
      {
        while (partsquery.next())
        {
          partdata.append(QVariantList{partsquery.value(PART_ROW_ID),
                                       //partsquery.value(PART_UNIQUE_ID),
                                       partsquery.value(PART_CT),
                                       partsquery.value(PART_FILE_NAME),
                                       partsquery.value(PART_WIDTH),
                                       partsquery.value(PART_HEIGHT),
                                       ColumnNames::d_attachment_record.contains("part.quote") ? partsquery.value("part.quote") : false,
                                       ColumnNames::d_attachment_record.contains("part.caption") ? partsquery.value("part.caption") : ""});

          rowids.append(partsquery.value(PART_ROW_ID).toInt());
          //uniqueids.append(partsquery.value(PART_UNIQUE_ID).toInt());
          types.append(partsquery.value(PART_CT).toString());
          filenames.append(partsquery.value(PART_FILE_NAME).toString());
          sizes.append({partsquery.value(PART_WIDTH).toInt(), partsquery.value(PART_HEIGHT).toInt()});
          quotes.append(ColumnNames::d_attachment_record.contains("part.quote") ? partsquery.value("part.quote").toBool() : false);
          captions.append(ColumnNames::d_attachment_record.contains("part.caption") ? partsquery.value("part.caption").toString() : QString());
        }
      }
    }

    // save attachment info, but dont parse it

    // remove this, just switch over to separate things...
    item->setData(QVariant::fromValue<QVector<QVariantList>>(partdata), bepaald::PartDataRole);

    item->setData(QVariant::fromValue<int>(rowids.size()), bepaald::AttachmentDataCountRole);
    item->setData(QVariant::fromValue<QList<int>>(rowids), bepaald::AttachmentRowIdRole);
    //item->setData(QVariant::fromValue<QList<int>>(uniqueids), bepaald::AttachmentUniqueIdRole);
    item->setData(QVariant::fromValue<QList<QString>>(types), bepaald::AttachmentTypeRole);
    item->setData(QVariant::fromValue<QList<QString>>(filenames), bepaald::AttachmentFilenameRole);
    item->setData(QVariant::fromValue<QList<QSize>>(sizes), bepaald::AttachmentSizeRole);
    item->setData(QVariant::fromValue<QList<bool>>(quotes), bepaald::AttachmentQuoteRole);
    item->setData(QVariant::fromValue<QList<QString>>(captions), bepaald::AttachmentCaptionRole);

    //item->setData(true, bepaald::ItemCompleteRole);

    //appendRow(item);

    if (extraitem)
      emit itemAvailable(extraitem);
    emit itemAvailable(item);
  }

  qDebug() << "Starting" << d_threadpool.size() << "threads!";
  for (auto t : d_threadpool)
    t->start();
  qDebug() << "Threads started";

  db.close();

  emit itemAvailable(nullptr);
}

QModelIndexList ConversationSqlModel::match(QRegularExpression const &regex) const
{
  QModelIndexList results;
  if (!regex.pattern().isEmpty())
    for (uint i = 0; i < rowCount(); ++i)
      if (index(i, 0).isValid() &&
          !Types::isStatusMessage(index(i, 0).data(bepaald::MsgTypeRole).toULongLong()) &&
          regex.match(index(i, 0).data(Qt::DisplayRole).toString()).hasMatch())
        results.append(index(i, 0));
  return results;
}

void ConversationSqlModel::showDetails(QModelIndex const &idx) const
{

  qDebug() << idx.data(bepaald::MmsIdRole);
  qDebug() << idx.data(bepaald::SmsIdRole);

  QString query;
  unsigned long long mmsid = 0;
  unsigned long long smsid = 0;

  if (idx.data(bepaald::MmsIdRole).isValid() && (mmsid = idx.data(bepaald::MmsIdRole).toULongLong()) > 0)
  {
    query = "SELECT * FROM '" + ColumnNames::d_mms_table + "' WHERE " + ColumnNames::d_mms_table + "._id = " + QString::number(mmsid);
  }
  else if (idx.data(bepaald::SmsIdRole).isValid() && (smsid = idx.data(bepaald::SmsIdRole).toULongLong()) > 0)
  {
    query = "SELECT * FROM 'sms' WHERE sms._id = " + QString::number(smsid);
  }

  if (!smsid && !mmsid)
    return;

  QSqlQuery detailsquery;
  if (!detailsquery.exec(query))
    qDebug() << detailsquery.lastError().text();
  else
  {
    while (detailsquery.next())
    {
      for (int i = 0; i < (mmsid ? ColumnNames::d_message_record.count() : ColumnNames::d_sms_record.count()); ++i)
      {
        QString fieldname = mmsid ? ColumnNames::d_message_record.fieldName(i) : ColumnNames::d_sms_record.fieldName(i);
        qDebug() << fieldname << detailsquery.value(fieldname);
      }
    }
  }

}

void ConversationSqlModel::deleteAttData(QStandardItem *item)
{
  qInfo() << "Cleaner dealing with item: " << item->data(Qt::DisplayRole);

  if (item->data(bepaald::QueuedRole).toBool())
  {
    // item is still being handled by worker thread, re-add to d_indecesinview so it can be cleaned
    // up later.
    qInfo() << "returning(1)";
    return;
  }

  if (!item->data(bepaald::AttachmentDataRole).isValid()) // not queued, no attdata, no worries
  {
    qInfo() << "returning(2)";
    return;
  }

  qInfo() << "Cleaning up data";
}

void ConversationSqlModel::itemsInView(QList<QModelIndex> const &itemsinview)
{
  for (int i = 0; i < d_indecesinview.count(); ++i)
  {
    if (!itemsinview.contains(d_indecesinview[i]))
    {
      qInfo() << "ITEM NO LONGER IN VIEW, DELETING ATT_DATA!";
      deleteAttData(itemFromIndex(d_indecesinview[i]));
    }
  }
  d_indecesinview = std::move(itemsinview);

  for (int i = 0; i < d_indecesinview.count(); ++i)
  {
    //qInfo() << "Items in view: " << d_indecesinview[i].data(Qt::DisplayRole).toString();
    if (d_indecesinview[i].data(bepaald::PartDataRole).value<QVector<QVariantList>>().size() > 0)
    {
      if (d_indecesinview[i].data(bepaald::AttachmentDataRole).isValid())
      {
        qInfo() << "Skipping because attachment data is already set!";
        continue;
      }

      if (d_indecesinview[i].data(bepaald::QueuedRole).toBool())
      {
        qInfo() << "Skipping because item is already in multimedia thread queue";
        continue;
      }

      qInfo() << "Handling attachment";

      // set attachment:
      d_jobqueuemutex.lock();
      QStandardItem *it = itemFromIndex(d_indecesinview[i]);
      it->setData(true, bepaald::QueuedRole);
      //d_jobqueue.enqueue(QPair<QStandardItem *,
      //                   QVector<QVariantList>>(it, d_indecesinview[i].data(bepaald::PartDataRole).value<QVector<QVariantList>>()));
      d_jobqueue2.enqueue(std::tuple<QStandardItem *, int, QVector<QVariantList>>
                          (it, 0, d_indecesinview[i].data(bepaald::PartDataRole).value<QVector<QVariantList>>()));
      qDebug() << "Pushed job! Now " << d_jobqueue2.size() << " jobs";
      d_jobqueuemutex.unlock();
    }
  }
}

void ConversationSqlModel::shortInfo(QModelIndex const &idx) const
{
  qDebug() << idx.data(Qt::DisplayRole).toString() << " (Attachment: " << idx.data(bepaald::AttachmentRole) << ")";
}

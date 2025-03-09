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

#include "conversationsqlmodel.ih"

void ConversationSqlModel::t_watchJobQueueV2()
{
  while (!d_cancel) // have some condition for this...
  {
    std::tuple<QStandardItem *, int, QVariantList> job{nullptr, 0, {}};
    if (d_jobqueuemutex.tryLock(0)) // try in milliseconds
    {
      if (!d_jobqueue2.isEmpty())
      {
        // get job
        job = {std::get<0>(d_jobqueue2.head()), std::get<1>(d_jobqueue2.head()),
               std::get<2>(d_jobqueue2.head())[std::get<1>(d_jobqueue2.head())]};

        // up curidx
        ++std::get<1>(d_jobqueue2.head());

        if (std::get<1>(d_jobqueue2.head()) >= std::get<2>(d_jobqueue2.head()).size())
          d_jobqueue2.dequeue();
      }
      d_jobqueuemutex.unlock();
      // got job if queue was not empty
      if (std::get<0>(job))
      {
        qDebug() << QThread::currentThread() << " got job to do in row: " << std::get<0>(job)->row() << "!";
        ++d_working;
        //auto start = std::chrono::system_clock::now();
        t_handleMultiMediaPartV2(std::get<2>(job), std::get<1>(job), std::get<0>(job));
        //if (std::get<0>(job)->row() == 13)
        //{
        //  std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
        //  std::cout << " *** SET ROW 13 TOOK: " << std::fixed << std::setprecision(3) << diff.count() << " SECONDS" << std::endl;
        //}
        --d_working;
      }
      else
      {
        //qDebug() << QThread::currentThread() << " waiting for jobs!";
        QThread::msleep(50); // in milliseconds
      }
    }
    else
    {
      //qDebug() << QThread::currentThread() << " waiting for lock!";
      QThread::msleep(50); // in milliseconds
    }
  }
}

//#include <QRandomGenerator>

void ConversationSqlModel::t_handleMultiMediaPartV2(QVariantList const &partdata, int curidx, QStandardItem *item)
{
  qInfo() << "Starting handleMultiMediaPart";// with 3 second sleep!";
  //QThread::msleep(QRandomGenerator::global()->bounded(3000, 9000));

  if (d_cancel)
    return;

  if (auto ait = d_attachments->find(partdata[PART_ROW_ID].toULongLong()); ait != d_attachments->end())
  {

    QString contenttype = partdata[PART_CT].toString();

    if (contenttype == "image/gif")
    {

      QByteArray *rawdata = new QByteArray;
      rawdata->setRawData(reinterpret_cast<char *>(t_rawAttachmentData(partdata)), t_rawAttachmentSize(partdata));
      d_gifdatamutex.lock();
      d_gifdata.append({new QBuffer(rawdata)}); // QBuffer does not take ownership of qbytearray
      d_gifdatamutex.unlock();

      // actual gif movie must be constructed in main thread as it cannot be move-to-thread() later...
      emit attachmentDataAvailable(std::make_shared<bepaald::AttachmentData>(item, QVariant()), curidx, d_gifdata.size() - 1);
      return;
    }
    else if (contenttype.left(6) == "image/")
    {
      QByteArray rawdata = QByteArray::fromRawData(reinterpret_cast<char *>(t_rawAttachmentData(partdata)), t_rawAttachmentSize(partdata));
      QBuffer buffer(&rawdata); // QBuffer does not take ownership of qbytearray
      QImageReader ir(&buffer);
      ir.setAutoTransform(true);
      QPixmap pm = QPixmap::fromImageReader(&ir);
      reinterpret_cast<FrameWithAttachment *>(ait->second.get())->clearData();

      // if (partdatavector.size() > 10)
      // {
      //   for (uint i = 0; i < 5; ++i)
      //   {
      //     QString filename = QString::number(i) + ".png";
      //     QFileInfo fi(filename);
      //     if (!fi.exists())
      //     {
      //       qDebug() << "SAVING IMAGE ATTACHMENT: " << filename;
      //       pm.save(filename);
      //       break;
      //     }
      //   }
      // }

      if (!pm.isNull())
        emit attachmentDataAvailable(std::make_shared<bepaald::AttachmentData>(item, QVariant::fromValue(std::move(pm))), curidx);
      else
        qDebug() << "Error: unsupported image format:" << partdata[PART_CT].toString();
      return;
    }

    else if (contenttype.left(6) == "video/")
    {
      MediaPreviewGenerator mpg(t_rawAttachmentData(partdata), t_rawAttachmentSize(partdata));
      VideoContext *videoctx = mpg.getRgb();
      if (videoctx)
      {
        QImage image(videoctx->firstFrame(), videoctx->width(), videoctx->height(), QImage::Format_RGB888);
        if (!image.isNull())
        {
          if (videoctx->rotation())
          {
            QTransform tr;
            tr.rotate(videoctx->rotation());
            image = image.transformed(tr);
          }


          QPixmap pm = QPixmap::fromImage(std::move(image));
          if (!pm.isNull())
            //qInfo() <<  "emitting data";
            emit attachmentDataAvailable(std::make_shared<bepaald::AttachmentData>(item, QVariant::fromValue(std::move(pm))), curidx);
        }
      }
      //qInfo() << " clearing video data:";
      reinterpret_cast<FrameWithAttachment *>(ait->second.get())->clearData();

      return;
      //// save video
      // std::string name = partdata[PART_FILE_NAME].toString().toStdString();
      // if (name.empty())
      // {
      //   name = partdata[PART_UNIQUE_ID].toString().toStdString() + ".mp4";
      // }
      // unsigned char *data = rawAttachmentData(partdata);
      // uint64_t size = rawAttachmentSize(partdata);
      // std::ofstream att(name);
      // std::cout << "Saving video attachment to " << name << std::endl;
      // att.write(reinterpret_cast<char *>(data), size);
    }

    else if (contenttype.left(6) == "audio/")
    {
      // maybe dont do this? only the mimetype is really necesary for painting... (maybe getLength -> clearData);


      // std::cout << "/********************************************************************************" << std::endl;
      // std::cout << " *         HANDLING AUDIO ATTACHMENT" << std::endl;
      // std::cout << " *        " << bepaald::bytesToHexString(t_rawAttachmentData(partdata), std::min(t_rawAttachmentSize(partdata), (uint64_t)10))  << std::endl;
      // std::cout << " *" << std::endl;
      // std::cout << "/********************************************************************************" << std::endl;

      std::shared_ptr<MediaPlayer> mp = std::make_shared<MediaPlayer>(t_rawAttachmentData(partdata), t_rawAttachmentSize(partdata));


      emit attachmentDataAvailable(std::make_shared<bepaald::AttachmentData>(item, QVariant::fromValue(mp)), curidx);

      //appendAttachment(item, MsgAttachment(QVariant::fromValue(mp), MsgAttachment::ATTACHMENTTYPE::AUDIO,
      //                                     partdata[PART_FILE_NAME].toString()), partdata[PART_QUOTE].toBool());

      //d_medialistmutex.lock();
      //d_medialist.append(mp);
      //d_medialistmutex.unlock();
      // // save audio
      // std::string name = partdata[PART_FILE_NAME].toString().toStdString();
      // if (name.empty())
      // {
      //   name = partdata[PART_UNIQUE_ID].toString().toStdString() + ".aac";
      // }
      // unsigned char *data = rawAttachmentData(partdata);
      // uint64_t size = rawAttachmentSize(partdata);
      // std::ofstream att(name);
      // std::cout << "Saving audio attachment to " << name << std::endl;
      // att.write(reinterpret_cast<char *>(data), size);
      return;
    }

    else
    {
      //qDebug() << " *** NOT YET IMPLEMENTED FILETYPE AT IDX " << item->row() << "! : "
      //         << partdata[PART_FILE_NAME].toString() << " ("
      //         << partdata[PART_CT].toString() << ")";

      emit attachmentDataAvailable(std::make_shared<bepaald::AttachmentData>
                                   (item, QVariant::fromValue(std::move(bepaald::BinaryFileData{t_rawAttachmentData(partdata),
                                                                                                  t_rawAttachmentSize(partdata)}))), curidx);
      return;
    }
  }
  else
  {
    qDebug() << "ERROR FINDING ATTACHMENT DATA FOR PART! (key" << partdata[PART_ROW_ID].toLongLong() << ")";
    // for (auto i : fulldata)
    //   qDebug() << i;

    // for (auto i = d_attachments->begin(); i != d_attachments->end(); ++i)
    //   qDebug() << QString::fromStdString(i->first.first) << "," << QString::fromStdString(i->first.second);
  }
}

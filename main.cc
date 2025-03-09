/*
  Copyright (C) 2025  Selwin van Dijk

  This file is part of signalbackup-viewer0.12.

  signalbackup-viewer0.12 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  signalbackup-viewer0.12 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with signalbackup-viewer0.12.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <QApplication>
//#include <QTableView>
#include <QtSql>
#include <QStringList>
//#include <QSqlQueryModel>
//#include <QTableView>
//#include <QListView>
#include <QGridLayout>
#include <QMainWindow>
#include <QProgressDialog>
#include <QProgressBar>

#include "columnnames/columnnames.h"
#include "recipientpreferences/recipientpreferences.h"
#include "threadlistview/threadlistview.h"
#include "threadviewdelegate/threadviewdelegate.h"
#include "threadsqlmodel/threadsqlmodel.h"
#include "conversationlistviewdelegate/conversationlistviewdelegate.h"
#include "conversationsqlmodel/conversationsqlmodel.h"
#include "conversationlistview/conversationlistview.h"
#include "sbvmainwindow/sbvmainwindow.h"

#include <memory>
#include <string>
#include <map>

#include "shared/filedecryptor/filedecryptor.h"
#include "shared/backupframe/backupframe.h"
#include "shared/databaseversionframe/databaseversionframe.h"
#include "shared/attachmentframe/attachmentframe.h"
#include "shared/avatarframe/avatarframe.h"
#include "shared/sqlstatementframe/sqlstatementframe.h"

#include "common_fe.h"

int main(int argc, char *argv[])
{
  std::string signalfile;
  std::string passphrase;
  std::string database = "file::memory:";
  if (argc >= 3)
  {
    signalfile = argv[1];
    passphrase = argv[2];
    if (argc == 4)
    {
      database = argv[3];
    }
  }
  else
  {
    std::cout << "Usage: " << argv[0] << " [signal backup file] [STRING containing exactly 30 digits]" << std::endl;
    return 1;
  }

  FileDecryptor signalbackup(signalfile, passphrase, false);
  if (!signalbackup.ok())
    return 1;
  uint64_t totalsize = signalbackup.total();
  uint32_t databaseversion = 0;

  //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // default in QT6
  QApplication app(argc, argv);

  // create sql database
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
  db.setDatabaseName(QString::fromStdString(database));
  //db.setDatabaseName("SIGNAL.db");
  if (!db.open())
    std::cout << "Failed to open database!" << std::endl;
  QSqlQuery query;
  if (!query.exec("PRAGMA page_size = 4096;") ||
      !query.exec("PRAGMA cache_size = 16384;") ||
      !query.exec("PRAGMA temp_store = MEMORY;") ||
      !query.exec("PRAGMA synchronous = OFF") ||
      !query.exec("PRAGMA journal_mode = MEMORY"))
  {
    std::cout << "Error setting db parameters" << std::endl;
    return 1;
  }

  // QProgressBar loading;//("Loading database...", "Cancel", 0, 0);
  // loading.setRange(0, 0);

  std::map<std::string, std::unique_ptr<BackupFrame>> avatars;
  std::map<uint64_t, std::unique_ptr<BackupFrame>> attachments; // maps <part._id, part.unique_id> -> attachmentdata

  std::ifstream backupfile(signalfile);
  std::unique_ptr<BackupFrame> frame(nullptr);
  while ((frame = signalbackup.getFrame(backupfile)))
  {
    //if (loading.wasCanceled())
    //return 0;

    //loading.setValue(signalbackup.curFilePos());

    std::cout << "\rDecrypting database file: "
              << std::fixed << std::setprecision(1) << std::setw(5) << std::setfill('0')
              << (static_cast<float>(backupfile.tellg()) / totalsize) * 100 << "% ..."
              << std::defaultfloat << std::flush;

    //frame->printInfo();

    // if (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
    // {
    //   frame->printInfo();
    // }

    if (frame->frameType() == BackupFrame::FRAMETYPE::SQLSTATEMENT)
    {
      SqlStatementFrame *f = reinterpret_cast<SqlStatementFrame *>(frame.get());
      std::string sqlstatement(f->statement());

      //std::cout << sqlstatement << std::endl;

      // if (sqlstatement.find("CREATE TABLE part") != std::string::npos)
      //   std::cout << std::endl << sqlstatement << std::endl;
      //if (sqlstatement.find("INSERT INTO recipient") != std::string::npos)
      //  std::cout << std::endl << sqlstatement << std::endl;
      //if (sqlstatement.find("CREATE TABLE thread") != std::string::npos)
      //{
      //  std::cout << std::endl << sqlstatement << std::endl;
      //  return 1;
      //}
      //if (sqlstatement.find("INSERT INTO part") != std::string::npos)
      //  std::cout << std::endl << sqlstatement << std::endl;
      // if (sqlstatement.find("INSERT INTO sms") != std::string::npos)
      //   std::cout << std::endl << sqlstatement << std::endl;
      // if (sqlstatement.find("INSERT INTO mms") != std::string::npos)
      //   std::cout << std::endl << sqlstatement << std::endl;
      //if (sqlstatement.find("INSERT INTO thread") != std::string::npos)
      //  std::cout << std::endl << sqlstatement << std::endl;


      if (sqlstatement.empty())
      {
        std::cout << "Error empty statement!" << std::endl;
        return 1;
      }
      if (!sqlstatement.starts_with("CREATE TABLE sqlite_"))
      {
        if (!query.exec(QString::fromStdString(sqlstatement)))
        {
          std::cout << "ERROR executing query: \"" << sqlstatement << "\"" << std::endl;
          std::cout << query.lastError().text().toStdString() << std::endl;
          // return 1;
        }
      }
      continue;
    }
    //std::cout << "" << std::endl;

    //loading.setMaximum(totalsize);

    if (frame->frameType() == BackupFrame::FRAMETYPE::ATTACHMENT)
    {
      //frame->printInfo();
      AttachmentFrame *a = reinterpret_cast<AttachmentFrame *>(frame.get());
      attachments.emplace(a->rowId(), std::move(frame));
      continue;
    }

    if (frame->frameType() == BackupFrame::FRAMETYPE::AVATAR)
    {
      //frame->printInfo();
      AvatarFrame *a = reinterpret_cast<AvatarFrame *>(frame.get());
      if (databaseversion <= 27)
      {
        std::cout << "Adding avatar to map: " << std::string(a->name()) << std::endl;
        avatars.emplace(std::move(std::string(a->name())), std::move(frame));
      }
      else
      {
        std::cout << "Adding avatar to map: " << std::string(a->recipient()) << std::endl;
        avatars.emplace(std::move(std::string(a->recipient())), std::move(frame));
      }
      continue;
    }

    if (frame->frameType() == BackupFrame::FRAMETYPE::HEADER)
    {
      frame->printInfo();
      continue;
    }

    if (frame->frameType() == BackupFrame::FRAMETYPE::DATABASEVERSION)
    {
      frame->printInfo();
      databaseversion = reinterpret_cast<DatabaseVersionFrame *>(frame.get())->version();
      continue;
    }

    if (frame->frameType() == BackupFrame::FRAMETYPE::END ||
        frame->frameType() == BackupFrame::FRAMETYPE::SHAREDPREFERENCE ||
        frame->frameType() == BackupFrame::FRAMETYPE::KEYVALUE ||
        frame->frameType() == BackupFrame::FRAMETYPE::STICKER)
      continue;

    std::cout << "UNHANDLED FRAMETYPE!" << std::endl;
  }
  std::cout << "done!" << std::endl;

  ColumnNames::init();

  RecipientPreferences::setInfo(databaseversion, &avatars);

  ConversationSqlModel *conversationmodel = new ConversationSqlModel(&attachments);

  ThreadSqlModel *threadmodel = new ThreadSqlModel(conversationmodel, databaseversion);
  QString tq = "SELECT * FROM 'thread' ORDER BY date DESC";
  threadmodel->setQuery(tq);

  QWidget *centralwidget = new QWidget;
  QGridLayout *layout = new QGridLayout;

  ThreadListView *threadview = new ThreadListView;
  threadview->setItemDelegate(new ThreadViewDelegate(threadview)); // maybe do this in View cstor?
  threadview->setThreadModel(threadmodel);
  threadmodel->setParent(threadview);
  layout->addWidget(threadview, 0, 0);

  ConversationListView *conversationview = new ConversationListView;
  conversationview->setConversationModel(conversationmodel);
  conversationmodel->setParent(conversationview);
  layout->addWidget(conversationview, 0, 1);

  //QObject::connect(threadview, &QListView::clicked, threadmodel, &ThreadSqlModel::setConversation);

  SBVMainWindow mainwindow(conversationview);
  centralwidget->setLayout(layout);
  mainwindow.setCentralWidget(centralwidget);
  mainwindow.resize(QSize(1135, 1066));
  mainwindow.show();

  QObject::connect(threadmodel, &ThreadSqlModel::conversationSet, &mainwindow, &SBVMainWindow::enableSearchBox);

  return app.exec();
}

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

#include "threadsqlmodel.ih"

#include "../columnnames/columnnames.h"

void ThreadSqlModel::setQuery(QString const &query)
{
  /*
    In database versions <= 23: recipients_ids were phone numbers (eg "+31601513210" or "__textsecure_group__!...")
                                Avatars were linked to these contacts by AvatarFrame::name() which was also phone number -> "+31601513210"
                                Groups had avatar data inside sqltable (groups.avatar, groups.group_id == "__textsecure_group__!...")
                23 < dbv <= 27: recipient ids were just id (eg "4")
                                Avatars were still identified by AvatarFrame::name() -> phone number
                                Groups had avatar data inside sqltable (groups.avatar, groups.group_id == "__textsecure_group__!...")
                 27 < dbv < 54: recipient ids were just id (eg "4")
                                Avatars were identified by AvatarFrame::recipient() -> "4"
                                Groups had avatar data inside sqltable (groups.avatar, groups.group_id == "__textsecure_group__!...")
                         >= 54: Same, groups have avatar in separate AvatarFrame, linked via groups.group_id == recipient.group_id -> recipient._id == AvatarFrame.recipient()
  */



  clear();

  qInfo() << "Query: " << query;

  QString recipient_table = "recipient";
  if (d_databaseversion <= 23)
    recipient_table = "recipient_preferences";

  QSqlQuery q;
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
  q.setForwardOnly(true);
#endif
  if (!q.exec(query))
  {
    qDebug() << q.lastError().text();
    return;
  }
  while (q.next())
  {
    qInfo() << "Got: " << q.record();

    // get id, up to dbv 23 == "+316...", after 23 == "num"
    QString id = q.value(ColumnNames::d_thread_recipient_id).toString(); // upto dbv 23 == "+316...", after 23 == "num"

    // get snippet
    bool mmsnippet = false;
    QString snippet(q.value("thread.snippet").toString());
    if (Types::isIdentityUpdate(q.value("thread.snippet_type").toULongLong()))
      snippet = "Safety number changed";
    else if (snippet.isEmpty() && !q.value("thread.snippet_uri").toString().isEmpty())
    {
      mmsnippet = true;
      snippet = "Media message";
    }

    // create index
    QStandardItem *item = new QStandardItem;
    item->setEnabled(false);
    item->setData(QVariant::fromValue(RecipientPreferences::getName(id)), Qt::DisplayRole);
    item->setData(QVariant::fromValue(q.value("thread.snippet_type").toULongLong()), bepaald::MsgTypeRole);
    item->setData(QVariant::fromValue(snippet), bepaald::SnippetRole);
    item->setData(QVariant::fromValue(mmsnippet), bepaald::MultiMediaSnippetRole);
    item->setData(QVariant::fromValue(q.value("thread._id").toString()), bepaald::ThreadIdRole);
    item->setData(QVariant::fromValue(id), bepaald::RecipientIdRole);
    item->setData(QVariant::fromValue(RecipientPreferences::isGroup(id)), bepaald::IsGroupThreadRole);
    appendRow(item);

  }
}

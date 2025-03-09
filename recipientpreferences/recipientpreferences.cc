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

#include "recipientpreferences.ih"

#include "../columnnames/columnnames.h"

QString RecipientPreferences::getGroupName(QString const &gid)
{
  QSqlQuery gq("SELECT title FROM groups WHERE group_id IS '" + gid + "'");
  if (!gq.exec())
  {
    qDebug() << gq.lastError().text();
    return QString();
  }
  while (gq.next())
  {
    //qInfo() << "Got: " << gq.record();
    return gq.value("title").toString();
  }
  return QString();
}

bool RecipientPreferences::getGroupAvatar(QString const &gid, QPixmap *avatar)
{
  QSqlQuery q("SELECT avatar_id, avatar FROM groups WHERE group_id IS '" + gid + "'");

  if (!q.exec())
  {
    qDebug() << q.lastError().text();
    return false;
  }
  while (q.next())
  {
    //qInfo() << "Got: " << q.record();
    if (q.value("avatar_id").toInt() != 0)
    {
      avatar->loadFromData(q.value("avatar").toByteArray());
      return true;
    }
  }
  return false;
}

void RecipientPreferences::loadDefaultGroupAvatar(QPixmap *avatar)
{
  *avatar = QPixmap(60, 60);
  avatar->fill(bepaald::getColorByName("light_blue"));
  QPixmap anongroup(":/group");
  QPainter painter(avatar);
  painter.drawPixmap(avatar->width() / 2 - anongroup.width() / 2, avatar->height() / 2 - anongroup.height() / 2, anongroup);
}

void RecipientPreferences::loadData()
{
  QSqlQuery recipientsquery;

  QString query = "SELECT * FROM recipient";
  if (s_databaseversion <= 23)
    query = "SELECT * FROM recipient_preferences LEFT JOIN groups ON recipient_preferences.recipient_ids = groups.group_id";

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
  recipientsquery.setForwardOnly(true);
#endif

  if (!recipientsquery.exec(query))
  {
    qDebug() << recipientsquery.lastError().text();
    return;
  }
  while (recipientsquery.next())
  {
    qInfo() << "Got: " << recipientsquery.record();

    QString id = s_databaseversion >= 24 ? recipientsquery.value("_id").toString() : recipientsquery.value("recipient_ids").toString();
    QString name = "";
    QPixmap avatar;
    QColor color(Qt::black);
    QColor darkcolor(Qt::black);
    bool isgroup = false;

    QString groupid = s_databaseversion >= 24 ? recipientsquery.value("recipient.group_id").toString() : recipientsquery.value("groups.group_id").toString();

    if (!groupid.isEmpty()) // is group
    {
      name = getGroupName(groupid);
      isgroup = true;

      if (s_databaseversion >= 54) // from this version, avatars are in separate avatar frames
      {
        if (s_avatars->find(id.toStdString()) != s_avatars->end())
          avatar.loadFromData(reinterpret_cast<FrameWithAttachment *>((*s_avatars)[id.toStdString()].get())->attachmentData(),
                              reinterpret_cast<FrameWithAttachment *>((*s_avatars)[id.toStdString()].get())->attachmentSize());
      }
      else // avatar in dbv < 54 was in groups table
        getGroupAvatar(groupid, &avatar);
      if (avatar.isNull())
      {
        loadDefaultGroupAvatar(&avatar);
        color = bepaald::getColorByName("light_blue");
      }
      else
        color = bepaald::getColor(name);
    }
    else // not a group
    {
      if (name.isEmpty() && ColumnNames::d_recipient_record.contains("nickname_joined_name"))
        name = recipientsquery.value("nickname_joined_name").toString();

      if (name.isEmpty() && ColumnNames::d_recipient_record.contains(ColumnNames::d_recipient_system_joined_name))
        name = recipientsquery.value(ColumnNames::d_recipient_system_joined_name).toString();

      if (name.isEmpty() && ColumnNames::d_recipient_record.contains(ColumnNames::d_recipient_profile_given_name))
        name = recipientsquery.value(ColumnNames::d_recipient_profile_given_name).toString();

      // no name set, using phone number
      if (name.isEmpty())
      {
        if (s_databaseversion > 23)
          name = recipientsquery.value(ColumnNames::d_recipient_e164).toString();
        else // old version
          name = recipientsquery.value("recipient_ids").toString();
      }

      if (name.isEmpty())
        name = "contact_" + recipientsquery.value("_id").toString();

      color = bepaald::getColor(name);
      darkcolor = bepaald::getColor(name, true);
      if (!recipientsquery.value(ColumnNames::d_recipient_avatar_color).toString().isEmpty())
      {
        color = bepaald::getColorByName(recipientsquery.value(ColumnNames::d_recipient_avatar_color).toString());
        darkcolor = bepaald::getColorByName(recipientsquery.value(ColumnNames::d_recipient_avatar_color).toString(), true);
      }


      for (auto it = s_avatars->begin(); it != s_avatars->end(); ++it)
        qInfo() << QString::fromStdString(it->first);

      if (s_databaseversion >= 33) // both avatar's and everything else in db is referenced by recipient._id
      {
        qInfo() << "Looking for avatar with id " << id;
        if (s_avatars->find(id.toStdString()) != s_avatars->end())
        {
          qInfo() << "FOUND!";
          avatar.loadFromData(reinterpret_cast<FrameWithAttachment *>((*s_avatars)[id.toStdString()].get())->attachmentData(),
                              reinterpret_cast<FrameWithAttachment *>((*s_avatars)[id.toStdString()].get())->attachmentSize());
        }
      }
      else if (s_databaseversion >= 24) // the transition period, dbv 24-33, avatars still id'd by phonenum, but rest of db uses recipient._id
      {
        if (s_avatars->find(recipientsquery.value(ColumnNames::d_recipient_e164).toString().toStdString()) != s_avatars->end())
          avatar.loadFromData(reinterpret_cast<FrameWithAttachment *>((*s_avatars)[recipientsquery.value(ColumnNames::d_recipient_e164).toString().toStdString()].get())->attachmentData(),
                              reinterpret_cast<FrameWithAttachment *>((*s_avatars)[recipientsquery.value(ColumnNames::d_recipient_e164).toString().toStdString()].get())->attachmentSize());
      }
      else // dbv < 24, everything uses phone number, recipient database does not exist
      {
        if (s_avatars->find(recipientsquery.value("recipient_preferences.recipient_ids").toString().toStdString()) != s_avatars->end())
          avatar.loadFromData(reinterpret_cast<FrameWithAttachment *>((*s_avatars)[recipientsquery.value("recipient_preferences.recipient_ids").toString().toStdString()].get())->attachmentData(),
                              reinterpret_cast<FrameWithAttachment *>((*s_avatars)[recipientsquery.value("recipient_preferences.recipient_ids").toString().toStdString()].get())->attachmentSize());
      }
    }

    qInfo() << "PREFDATA NEW" << id << name << color << /*darkcolor << */avatar.size();
    add(id, {name, color, darkcolor, isgroup, avatar});

  }
  s_loaded = true;
}

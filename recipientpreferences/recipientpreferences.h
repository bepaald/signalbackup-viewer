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

#ifndef RECIPIENTPREFERENCES_H_
#define RECIPIENTPREFERENCES_H_

#include <QMap>
#include <QString>
#include <QVariantList>
#include <QPixmap>

#include <map>
#include <memory>

#include "common_fe.h"
#include "shared/backupframe/backupframe.h"

class RecipientPreferences
{
  inline static bool s_loaded = false;
  inline static uint32_t s_databaseversion = std::numeric_limits<uint32_t>::max();
  inline static std::map<std::string, std::unique_ptr<BackupFrame>> *s_avatars = nullptr;
  inline static QMap<QString /*identifier eg: +316012345678*/, QVariantList/*name,color,darkcolor,isgroup,(avatar)*/> s_recipientprefs = QMap<QString, QVariantList>();
 public:
  inline static void setInfo(uint32_t dbv, std::map<std::string, std::unique_ptr<BackupFrame>> *avatars);
  inline static QString getName(QString const &id);
  inline static QString getColor(QString const &id);
  inline static QString getDarkColor(QString const &id);
  inline static bool isGroup(QString const &id);
  inline static bool hasAvatar(QString const &id);
  inline static QPixmap getAvatar(QString const &id);
 private:
  inline static void add(QString const &id, QVariantList const &info);
  static void loadData();
  static QString getGroupName(QString const &id);
  static bool getGroupAvatar(QString const &gid, QPixmap *avatar);
  static void loadDefaultGroupAvatar(QPixmap *avatar);
};

inline void RecipientPreferences::setInfo(uint32_t dbv, std::map<std::string, std::unique_ptr<BackupFrame>> *avatars)
{
  s_databaseversion = dbv;
  s_avatars = avatars;
}

inline void RecipientPreferences::add(QString const &id, QVariantList const &info)
{
  //qInfo() << "ADDING TO RECIPIENTDATABASE: " << id << ":" << info;
  if (info.size() != 5)
    return;
  s_recipientprefs[id] = info;
}

inline QString RecipientPreferences::getName(QString const &id)
{
  if (!s_loaded)
    loadData();
  return s_recipientprefs.value(id, {id, "", ""})[0].toString();
}

inline QString RecipientPreferences::getColor(QString const &id)
{
  if (!s_loaded)
    loadData();
  return s_recipientprefs.value(id, {id, "#888888"})[1].toString();
}

inline QString RecipientPreferences::getDarkColor(QString const &id)
{
  if (!s_loaded)
    loadData();
  return s_recipientprefs.value(id, {id, "#888888", "#888888"})[2].toString();
}

inline bool RecipientPreferences::isGroup(QString const &id)
{
  if (!s_loaded)
    loadData();
  return s_recipientprefs.value(id, {id, "", "", false})[3].toBool();
}

inline QPixmap RecipientPreferences::getAvatar(QString const &id)
{
  if (!s_loaded)
    loadData();
  return s_recipientprefs.value(id, {id, "", "", false, QPixmap()})[4].value<QPixmap>();
}

inline bool RecipientPreferences::hasAvatar(QString const &id)
{
  if (!s_loaded)
    loadData();
  return !getAvatar(id).isNull();
}

#endif

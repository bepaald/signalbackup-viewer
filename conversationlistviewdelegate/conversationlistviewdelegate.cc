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

#include "conversationlistviewdelegate.ih"

// app/src/main/java/org/thoughtcrime/securesms/database/model/MessageRecord.java

QString ConversationListViewDelegate::getStatusMessage(QModelIndex const &index) const
{
  if (Types::isIdentityUpdate(index.data(bepaald::MsgTypeRole).toULongLong()))
  {
    return "Your safety number with " + RecipientPreferences::getName(index.data(bepaald::RecipientIdRole).toString()) + " has changed.";
  }
  else if (Types::isGroupUpdate(index.data(bepaald::MsgTypeRole).toULongLong()))
  {
    if (Types::isGroupV2(index.data(bepaald::MsgTypeRole).toULongLong()))
    {
      //
      return "(unhandled GroupV2 status message)";
    }
    else
    {

      QString bodytext(index.data(Qt::DisplayRole).toString());
      ProtoBufParser<
        protobuffer::optional::BYTES,
        protobuffer::optional::ENUM,
        protobuffer::optional::STRING,
        protobuffer::repeated::STRING,
        ProtoBufParser<
          protobuffer::optional::FIXED64,
          protobuffer::optional::STRING,
          protobuffer::optional::BYTES,
          protobuffer::optional::UINT32,
          protobuffer::optional::BYTES,
          protobuffer::optional::BYTES,
          protobuffer::optional::STRING,
          protobuffer::optional::UINT32,
          protobuffer::optional::UINT32,
          protobuffer::optional::UINT32
          >
        > statusmsg(bodytext.toStdString());

      if (isOutgoing(index.data(bepaald::MsgTypeRole).toULongLong()))
        return "You updated the group.";

      QString result;
      result = RecipientPreferences::getName(index.data(bepaald::RecipientIdRole).toString()) + " updated the group.";

      auto field3 = statusmsg.getField<3>();
      auto field4 = statusmsg.getField<4>();

      if (field3 || !field4.empty())
        result += "\n";

      if (!field4.empty())
      {
        for (uint i = 0; i < field4.size(); ++i)
          result += ((i > 0) ? ", " : "") + RecipientPreferences::getName(QString::fromStdString(field4[i]));
        result += " joined the group.";

        if (field3)
          result += " ";
      }

      if (field3)
        result += "Group name is now '" + QString::fromStdString(field3.value()) + "'.";

      return result;
    }
  }
  else if (Types::isGroupQuit(index.data(bepaald::MsgTypeRole).toULongLong()))
  {
    if (Types::isGroupV2(index.data(bepaald::MsgTypeRole).toULongLong()))
    {
      //
      return "(unhandled GroupV2 status message (quit))";
    }
    else
    {
      if (isOutgoing(index.data(bepaald::MsgTypeRole).toULongLong()))
        return "You left the group";
      else
        return RecipientPreferences::getName(index.data(bepaald::RecipientIdRole).toString()) + " has left the group";
    }
  }
  else if (Types::isMissedCall(index.data(bepaald::MsgTypeRole).toULongLong()))
  {
    return "Missed call.";
  }
  else if (Types::isIncomingCall(index.data(bepaald::MsgTypeRole).toULongLong()))
  {
    return RecipientPreferences::getName(index.data(bepaald::RecipientIdRole).toString()) + " called.";
  }
  else if (Types::isOutgoingCall(index.data(bepaald::MsgTypeRole).toULongLong()))
  {
    return "You called.";
  }
  else if (Types::isJoined(index.data(bepaald::MsgTypeRole).toULongLong()))
  {
    return RecipientPreferences::getName(index.data(bepaald::RecipientIdRole).toString()) + " is on Signal!";
  }
  else if (index.data(bepaald::MsgTypeRole).toULongLong() == Types::GV1_MIGRATION_TYPE)
  {
    // app/src/main/java/org/thoughtcrime/securesms/database/model/MessageRecord.java
    QString bodytext(index.data(Qt::DisplayRole).toString());
    if (bodytext.isEmpty())
      return "This group was updated to a New Group.";
    else
      return "(unhandled status message (group migration event))";
  }
  else
  {
    //qDebug() << "     !!!!     WARNING   UNHANDLED STATUS MESSAGE";
    return "(unhandled status message)";
  }
}

bool ConversationListViewDelegate::isOutgoing(long type) const
{
  return !Types::isInboxType(type);
}

bool ConversationListViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, QStyleOptionViewItem const &option, QModelIndex const &index)
{
  if (event->type() == QEvent::MouseButtonRelease)
  {
    QMouseEvent *me = static_cast<QMouseEvent *>(event);
    if (me)
      qDebug() << "::editorEvent() mouse release click position = " << (me->pos() - option.rect.topLeft());
  }
  //  else
  //    qDebug() << "GOT EVENT IN DELEGATE: " << event->type();

  return QAbstractItemDelegate::editorEvent(event, model, option, index);
}

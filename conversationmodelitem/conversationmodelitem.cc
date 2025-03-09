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

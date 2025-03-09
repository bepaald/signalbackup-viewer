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

#include "signalviewdelegatebase.ih"

void SignalViewDelegateBase::paintAvatar(QPainter *painter, int x, int y, int size, QString const &recipient) const
{
  // Draw avatar
  QPainterPath avatarpath;
  avatarpath.addEllipse(x, y, size, size);
  painter->setClipPath(avatarpath);
  if (RecipientPreferences::hasAvatar(recipient))
  {
    QPixmap avatar = RecipientPreferences::getAvatar(recipient);
    painter->drawPixmap(x, y, size, size, avatar);
  }
  else
  {
    QColor color = RecipientPreferences::getColor(recipient);
    painter->fillRect(x, y, size, size, color); // CLIPPED to circle

    // draw letter
    painter->setFont(QFont("Roboto", size / 3.));
    painter->setPen(QColor("#FFFFFF"));
    QString name = RecipientPreferences::getName(recipient);
    if (name[0].isLetter())
      painter->drawText(x, y, size, size, Qt::AlignCenter, name.left(1));
    else
      painter->drawText(x, y, size, size, Qt::AlignCenter, "#");
  }
  painter->setClipping(false);
}

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

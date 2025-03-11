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

#include "../emojitextdocument/emojitextdocument.h"

void ConversationListViewDelegate::datemessagePaint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);

  EmojiTextDocument datetimetext;
  QTextOption textOption(datetimetext.defaultTextOption());
  textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  datetimetext.setDefaultTextOption(textOption);
  datetimetext.setDefaultFont(d_datetimemessagefont);
  datetimetext.setHtml(index.data(bepaald::DateMessageRole).toString().toStdString());
  datetimetext.adjustSize();

  QPainterPath datetimebackground;
  datetimebackground.addRoundedRect(option.rect.width() / 2 - datetimetext.size().width() / 2,
                                    option.rect.top() + ((index.row() == 0) ? d_verticalmargin : 0),
                                    datetimetext.size().width(), datetimetext.size().height(), 5, 5);
  painter->fillPath(datetimebackground, QColor("#757575"));

  QAbstractTextDocumentLayout::PaintContext ctx;
  ctx.palette.setColor(QPalette::Text, QColor("white"));
  painter->translate(option.rect.width() / 2 - datetimetext.size().width() / 2,
                     option.rect.top() + ((index.row() == 0) ? d_verticalmargin : 0));
  datetimetext.documentLayout()->draw(painter, ctx);

  painter->restore();

  if constexpr (ConversationListViewDelegate::showhelplines)
  {
    painter->save();
    painter->setPen(QPen(Qt::red));
    painter->drawRect(option.rect);
    painter->drawLine(option.rect.topLeft(), option.rect.bottomRight());
    painter->drawLine(option.rect.topRight(), option.rect.bottomLeft());
    painter->restore();
  }

  return;
}

void ConversationListViewDelegate::statusmessagePaint(QPainter *painter, QStyleOptionViewItem  const &option, QModelIndex const &index) const
{
  // QStyleOptionViewItem options = option;
  // initStyleOption(&options, index);

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);

  EmojiTextDocument bodydoc;
  QTextOption textOption(bodydoc.defaultTextOption());
  textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  //bodydoc.setDocumentMargin(documentmargin);
  bodydoc.setDefaultTextOption(textOption);
  bodydoc.setDefaultFont(d_statusbodyfont);
  bodydoc.setDefaultStyleSheet(".emoji, .jumbo-emoji {font-size: " + QString::number(QFontMetrics(d_statusbodyfont).height() + 4) + "px; font-family: emoji;}");
  QString bodytext(index.data(Qt::DisplayRole).toString());
  bodydoc.setHtml(getStatusMessage(index).toStdString());

  qreal contentswidth = option.rect.width() - d_horizontalmargin - d_horizontalmargin - d_horizontalmargin - d_status_group.width();
  bodydoc.setTextWidth(contentswidth);
  qreal h = bodydoc.size().height();
  if (h < d_status_group.height() + d_verticalmargin)
    h = d_status_group.height() + d_verticalmargin;

  painter->translate(option.rect.left() + d_horizontalmargin, option.rect.top() + ((index.row() == 0) ? d_verticalmargin : 0));

  /* change the next bit to just
     painter->drawPixmap((contentswidth + d_status_group_update.width()) / 2 - (d_status_group_update.width() + bodydoc.idealWidth() + d_horizontalmargin) / 2, h / 2 - d_status_group_update.height() / 2, d_status_group_update);
     when the pixmap is prerendered in correct color, or leave as is, for easier color switching?
  */
  QPixmap mask;
  if (Types::isGroupUpdate(index.data(bepaald::MsgTypeRole).toULongLong()))
    mask = d_status_group;
  else if (Types::isIdentityUpdate(index.data(bepaald::MsgTypeRole).toULongLong()))
    mask = d_status_security;
  else if (Types::isOutgoingCall(index.data(bepaald::MsgTypeRole).toULongLong()))
    mask = d_status_call_made;
  else if (Types::isIncomingCall(index.data(bepaald::MsgTypeRole).toULongLong()))
    mask = d_status_call_received;
  else if (Types::isMissedCall(index.data(bepaald::MsgTypeRole).toULongLong()))
    mask = d_status_call_missed;
  else if (Types::isJoined(index.data(bepaald::MsgTypeRole).toULongLong()))
    mask = d_status_joined_signal;
  else if (Types::isGroupQuit(index.data(bepaald::MsgTypeRole).toULongLong()))
    mask = d_status_group;
  else if (index.data(bepaald::MsgTypeRole).toULongLong() == Types::GV1_MIGRATION_TYPE)
    mask = d_status_update_group_role;
  if (!mask.isNull())
  {
    QPainter maskpainter(&mask);
    maskpainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    maskpainter.fillRect(mask.rect(), QBrush("#757575"));
    maskpainter.end();
    painter->drawPixmap((contentswidth + d_status_group.width()) / 2 -
                        (d_status_group.width() + bodydoc.idealWidth() + d_horizontalmargin) / 2, // x
                        h / 2 - d_status_group.height() / 2,                                      // y
                        //10,                                                                     // w
                        //10,                                                                     // h
                        mask);
  }

  painter->translate((contentswidth + d_status_group.width()) / 2 -
                     (d_status_group.width() + bodydoc.idealWidth() + d_horizontalmargin) / 2 +
                     d_horizontalmargin + d_status_group.width(),
                     h / 2 - bodydoc.size().height() / 2);

  QAbstractTextDocumentLayout::PaintContext ctx;
  ctx.palette.setColor(QPalette::Text, QColor("#757575"));
  bodydoc.documentLayout()->draw(painter, ctx);

  painter->restore();


  if constexpr (ConversationListViewDelegate::showhelplines)
  {
    painter->save();
    painter->setPen(QPen(Qt::red));
    painter->drawRect(option.rect);
    painter->drawLine(option.rect.topLeft(), option.rect.bottomRight());
    painter->drawLine(option.rect.topRight(), option.rect.bottomLeft());
    painter->restore();
  }

  return;
}

void ConversationListViewDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  // if (d_model)
  // {
  //   //qInfo() << "CALLING ITEM IN VIEW:";
  //   d_model->itemInView(index);
  // }

  if (!index.data(bepaald::DateMessageRole).isNull())
    return datemessagePaint(painter, option, index);

  if (/*[[unlikely]]*/Types::isStatusMessage(index.data(bepaald::MsgTypeRole).toULongLong()))
    return statusmessagePaint(painter, option, index);

  bool outgoing = isOutgoing(index.data(bepaald::MsgTypeRole).toULongLong());

  // QStyleOptionViewItem options = option;
  // initStyleOption(&options, index);

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);

  //painter->drawRect(option.rect);

  EmojiTextDocument bodydoc;
  if (!d_regex.pattern().isEmpty())
  {
    SearchHighlighter *highlighter = new SearchHighlighter(&bodydoc, d_regex);
    Q_UNUSED(highlighter);
  }

  bool message_deleted = index.data(bepaald::MsgDeletedRole).toBool();

  QTextOption textOption(bodydoc.defaultTextOption());
  textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  //bodydoc.setDocumentMargin(documentmargin);
  bodydoc.setDefaultTextOption(textOption);
  bodydoc.setDefaultFont(d_bodyfont);
  bodydoc.setDefaultStyleSheet(".emoji {font-size: " + QString::number(QFontMetrics(d_bodyfont).height() + 4) + "px; font-family: emoji;} .jumbo-emoji {font-size: " + QString::number((QFontMetrics(d_bodyfont).height() + 4) * 3) + "px; font-family: emoji;} .monospace {font-family: 'Roboto Mono';}");
  QString bodytext(message_deleted ? (outgoing ? "You deleted this message." : "This message was deleted.") : index.data(Qt::DisplayRole).toString());
  bodydoc.setHtml(bodytext.toStdString(), index.data(bepaald::MsgRangesRole).toByteArray());

  qreal contentswidth = option.rect.width() * d_widthfraction - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
  if (!outgoing && index.data(bepaald::IsGroupThreadRole).toBool())
    contentswidth -= (d_groupavatarsize + 0/*padding left and right*/);

  int att_data_count = index.data(bepaald::AttachmentDataCountRole).toInt();
  //QVector<QVariant> att_data;
  //for (int i = 0; i < index.data(bepaald::AttachmentDataCountRole).toInt(); ++i)
  //  att_data.append(index.data(bepaald::AttachmentDataRole + i).value<QVariant>());
  QList<QString> att_types = index.data(bepaald::AttachmentTypeRole).value<QList<QString>>();
  QList<QSize> att_sizes = index.data(bepaald::AttachmentSizeRole).value<QList<QSize>>();
  QList<bool> att_quotes = index.data(bepaald::AttachmentQuoteRole).value<QList<bool>>();

  // put all image attachments in imgthumbs list
  bool hasimageattachment = false;
  QList<std::pair<QPixmap, MsgAttachment::ATTACHMENTTYPE>> imgthumbs;
  //QVariant msgattachmentlist = index.data(bepaald::AttachmentRole);

  for (int i = 0; i < att_data_count; ++i)
  {
    if (att_quotes[i])
      continue;
    if (att_types[i].startsWith("image/gif"))
    {
      hasimageattachment = true;
      //create thumb
      if (att_data_count > i && index.data(bepaald::AttachmentDataRole + i).isValid())
        imgthumbs.append({index.data(bepaald::AttachmentDataRole + i).value<std::shared_ptr<QMovie>>().get()->currentPixmap(), MsgAttachment::ATTACHMENTTYPE::GIF});
      else
      {
        // image data not available yet, create placeholder thumbnail
        imgthumbs.append({QPixmap(att_sizes[i].isNull() ? QSize{1, 1} : att_sizes[i]), MsgAttachment::ATTACHMENTTYPE::PLACEHOLDER_TYPE});
        imgthumbs.back().first.fill("#555555");
      }
    }
    else if (att_types[i].startsWith("image/"))
    {
      hasimageattachment = true;
      //create thumb
      if (att_data_count > i && index.data(bepaald::AttachmentDataRole + i).isValid())
        imgthumbs.append({index.data(bepaald::AttachmentDataRole + i).value<QPixmap>(), MsgAttachment::ATTACHMENTTYPE::IMAGE});
      else
      {
        imgthumbs.append({QPixmap(att_sizes[i].isNull() ? QSize{1, 1} : att_sizes[i]), MsgAttachment::ATTACHMENTTYPE::PLACEHOLDER_TYPE});
        imgthumbs.back().first.fill("#FF0000");
      }
    }
    else if (att_types[i].startsWith("video/"))
    {
      hasimageattachment = true;
      //create thumb
      if (att_data_count > i && index.data(bepaald::AttachmentDataRole + i).isValid())
      {
        imgthumbs.append({index.data(bepaald::AttachmentDataRole + i).value<QPixmap>(), MsgAttachment::ATTACHMENTTYPE::VIDEO});
      }
      else
      {
        //qInfo() << "                  NO VALID THUIMBNAIL< SETTING EREDREDREDRED : " << att_sizes[i];
        imgthumbs.append({QPixmap(att_sizes[i].isNull() ? QSize{1, 1} : att_sizes[i]), MsgAttachment::ATTACHMENTTYPE::PLACEHOLDER_TYPE});
        imgthumbs.back().first.fill("#FF0000");
      }
    }
  }

  int totalimageswidth = 0;
  int totalimagesheight = 0;

  // scale the imgthumbs to size
  if (hasimageattachment)
  {

    // only a single image is attached
    if (/*[[likely]]*/imgthumbs.size() == 1)
    {
      // if size was unknown to sizeHint(), we should crop image to square
      if (att_sizes[0].isNull())
        cropPixmapSquare(&imgthumbs[0].first);

      if (att_sizes[0].width() > option.rect.width() * .5)
      {
        imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(option.rect.width() * .5);
      }
      else if (att_sizes[0].width() < option.rect.width() * .5) // there is a minimum size apparently
      {
        contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
        imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(option.rect.width() * .5);
      }
      totalimageswidth = imgthumbs[0].first.width();
      totalimagesheight = imgthumbs[0].first.height();
    }

    // multiple images: crop and scale
    else // if (imgthumbs.size() > 1)
    {
      // crop all to smallest square
      for (int i = 0; i < std::min(5, static_cast<int>(imgthumbs.size())); ++i)
        cropPixmapSquare(&imgthumbs[i].first);

      // scale
      if (imgthumbs.size() == 2)
      {
        if (imgthumbs[0].first.width() * 2 + 2 > contentswidth)
        {
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 2 - 1); // -1 for single pixel margin
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 2 - 1); // again
        }
        else if (imgthumbs[0].first.width() * 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 2 - 1);
        }
        else // main size was ok, just scale the children to fit
        {
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(imgthumbs[0].first.width());
        }

        totalimageswidth = imgthumbs[0].first.width() + imgthumbs[1].first.width() + 2;
        totalimagesheight = imgthumbs[0].first.height();
      }
      else if (imgthumbs.size() == 3)
      {
        if (imgthumbs[0].first.width() * 1.5 + 2 > contentswidth)
        {
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 1.5 - 1); // -1 for single pixel margin
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 3 - 1);
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(contentswidth / 3 - 1);
        }
        else if (imgthumbs[0].first.width() * 1.5 + 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 1.5 - 1);
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 3 - 1);
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(contentswidth / 3 - 1);
        }
        else
        {
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(imgthumbs[0].first.width() / 2 - 1);
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(imgthumbs[0].first.width() / 2 - 1);
        }

        totalimageswidth = imgthumbs[0].first.width() + imgthumbs[1].first.width() + 2;
        totalimagesheight = imgthumbs[0].first.height();
      }
      else if (imgthumbs.size() == 4)
      {
        if (imgthumbs[0].first.width() * 2 + 2 > contentswidth)
        {
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 2 - 1); // -1 for single pixel margin
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[3].first = imgthumbs[3].first.scaledToWidth(contentswidth / 2 - 1);
        }
        else if (imgthumbs[0].first.width() * 2 + 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[3].first = imgthumbs[3].first.scaledToWidth(contentswidth / 2 - 1);
        }
        else // main size was ok, just scale the children to fit
        {
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(imgthumbs[0].first.width());
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(imgthumbs[0].first.width());
          imgthumbs[3].first = imgthumbs[3].first.scaledToWidth(imgthumbs[0].first.width());
        }

        totalimageswidth = imgthumbs[0].first.width() + imgthumbs[1].first.width() + 2;
        totalimagesheight = imgthumbs[0].first.height() + imgthumbs[2].first.height() + 2;
      }
      else if (imgthumbs.size() > 4)
      {
        if (imgthumbs[0].first.width() * 2 + 2 > contentswidth)
        {
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 2 - 1); // -1 for single pixel margin
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(contentswidth / 3 - 1);
          imgthumbs[3].first = imgthumbs[3].first.scaledToWidth(contentswidth / 3 - 1);
          imgthumbs[4].first = imgthumbs[4].first.scaledToWidth(contentswidth / 3 - 1);
        }
        else if (imgthumbs[0].first.width() * 2 + 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          imgthumbs[0].first = imgthumbs[0].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(contentswidth / 2 - 1);
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(contentswidth / 3 - 1);
          imgthumbs[3].first = imgthumbs[3].first.scaledToWidth(contentswidth / 3 - 1);
          imgthumbs[4].first = imgthumbs[4].first.scaledToWidth(contentswidth / 3 - 1);
        }
        else // main size was ok, just scale the children to fit
        {
          imgthumbs[1].first = imgthumbs[1].first.scaledToWidth(imgthumbs[0].first.width());
          imgthumbs[2].first = imgthumbs[2].first.scaledToWidth(imgthumbs[0].first.width() / 2 - 1);
          imgthumbs[3].first = imgthumbs[3].first.scaledToWidth(imgthumbs[0].first.width() / 2 - 1);
          imgthumbs[4].first = imgthumbs[4].first.scaledToWidth(imgthumbs[0].first.width() / 2 - 1);
        }

        totalimageswidth = imgthumbs[0].first.width() + imgthumbs[1].first.width() + 2;
        totalimagesheight = imgthumbs[0].first.height() + imgthumbs[2].first.height() + 4;
      }
    }
  }

  bodydoc.setTextWidth(contentswidth);

  QFontMetrics fm(d_footerfont);
  qreal datetimewidth = fm.horizontalAdvance(index.data(bepaald::DateTimeRole).toString());

  if (outgoing && /*[[unlikely]]*/
      ((index.data(bepaald::MsgTypeRole).toULongLong() & Types::BASE_TYPE_MASK) == Types::BASE_OUTBOX_TYPE ||
       (index.data(bepaald::MsgTypeRole).toULongLong() & Types::BASE_TYPE_MASK) == Types::BASE_SENDING_TYPE))
    datetimewidth = fm.horizontalAdvance(index.data(bepaald::DateTimeRole).toString() + "...");
  else if (outgoing && /*[[unlikely]]*/ index.data(bepaald::DeliveryReceiptCountRole).toULongLong() == 0)
    //datetimewidth += d_sent_not_received.width();
    datetimewidth += (d_sent_not_received.width() / (d_sent_not_received.height() / static_cast<float>(fm.ascent()))) + d_checkmarkpadding;
  else if (outgoing) // && index.data(bepaald::DeliveryReceiptCountRole).toULongLong() >0)
    //datetimewidth += d_received.width() + d_checkmarkpadding;
    datetimewidth += (d_received.width() / (d_received.height() / static_cast<float>(fm.ascent()))) + d_checkmarkpadding;
  qreal datetimeheight = fm.height();
  qreal headerheight = 0;

  int audiomsgidx = -1;
  int binaryfilemsgidx = -1;
  for (int i = 0; i < att_types.size(); ++i)
  {
    if (!att_quotes[i])
    {
      if (audiomsgidx == -1 && att_types[i].startsWith("audio/"))
        audiomsgidx = i;
      else if (binaryfilemsgidx == -1 &&
               !att_types[i].startsWith("audio/") &&
               !att_types[i].startsWith("video/") &&
               !att_types[i].startsWith("image/"))
        binaryfilemsgidx = i;
    }
  }

  QString filesizestring;
  if (binaryfilemsgidx != -1)
  {
    QFont oldfont = painter->font();
    painter->setFont(d_bodyfont);

    // check valid? check size() > 0?
    QList<QString> att_filenames = index.data(bepaald::AttachmentFilenameRole).value<QList<QString>>();

    //get width of filename
    qreal filenamewidth = painter->fontMetrics().horizontalAdvance(att_filenames[binaryfilemsgidx]) + d_mediabuttonsize + 1;

    // get filesize
    qreal filesize = -1;
    if (att_data_count > binaryfilemsgidx && index.data(bepaald::AttachmentDataRole + binaryfilemsgidx).isValid())
      filesize = index.data(bepaald::AttachmentDataRole + binaryfilemsgidx).value<bepaald::BinaryFileData>().size;
    //qreal filesize = (att_data.size() > binaryfilemsgidx) ? att_data[binaryfilemsgidx].value<std::pair<unsigned char *, uint64_t>>().second : -1;
    //qInfo() << " datasize : " << att_data.size();
    //qInfo() << " binidx: " << binaryfilemsgidx;
    //qInfo() << " FILESIZE : " << filesize;

    uint i = 0;
    for (; i < 4; ++i)
    {
      if (filesize / 1024. > 1)
        filesize /= 1024.;
        else
          break;
    }
    filesizestring = QString::number(filesize, 'f', 1);
    if (filesizestring.back() == '0' || filesizestring.back() == '.')
      filesizestring.chop(2);
    filesizestring += ((i == 0) ? " B" : (i == 1) ? " KB" : (i == 2) ? " MB" : (i == 3) ? " GB" : " TB");
    if (filesize < 0)
      filesizestring = "(unknown)";
    qreal filesizestringwidth = painter->fontMetrics().horizontalAdvance(filesizestring) + d_mediabuttonsize + 1;

    painter->setFont(oldfont);

    if (std::max(filenamewidth, filesizestringwidth) < contentswidth)
      contentswidth = std::max(filenamewidth, std::max(filesizestringwidth, std::max(std::max(bodydoc.idealWidth(), datetimewidth), static_cast<qreal>(totalimageswidth))));
    // else, keep contentswidth at max value....
  }
  else if (audiomsgidx == -1) // else, contentswidth is max width of bubble
    contentswidth = std::max(std::max(bodydoc.idealWidth(), datetimewidth), static_cast<qreal>(totalimageswidth));

  painter->translate(option.rect.left() + d_horizontalmargin, option.rect.top() + ((index.row() == 0) ? d_verticalmargin : 0));

  QColor bgcolor("#FFFFFF");
  if (!outgoing)
    bgcolor = RecipientPreferences::getColor(index.data(bepaald::RecipientIdRole).toString());

  qreal bodyheight = bodytext.isEmpty() ? d_horizontalmargin : bodydoc.size().height();

  // remove imgheight here, and just use totalimagesheight directly // ?
  int imgheight = totalimagesheight;
  if (audiomsgidx != -1 && !totalimagesheight)
    imgheight = d_mediabuttonsize;
  if (binaryfilemsgidx != -1 && !totalimagesheight) /// ???? wtf does this do?
    imgheight = scaledToWidth(d_mediabuttonsize, d_binarypm.size()).height();

  // if (imgheight)
  //   imgheight += d_toppadding;

  bool hasquote = false;
  if (index.data(bepaald::QuoteRole).isValid())
    hasquote = true;
  int quoteheight = hasquote ? 50 : 0; // calculate quote size

  // draw bubble
  QPainterPath pointie;
  int offset = 0;

  if (!outgoing && index.data(bepaald::IsGroupThreadRole).toBool())
  {
    offset = d_groupavatarsize + 0/*padding*/;
    headerheight = fm.height() + d_headerpadding;
    paintAvatar(painter, 0,
                bodyheight + datetimeheight + headerheight + imgheight + d_toppadding + d_bottompadding - d_groupavatarsize,
                d_groupavatarsize, index.data(bepaald::RecipientIdRole).toString());
  }

  // left bottom
  pointie.moveTo(offset, bodyheight + datetimeheight + headerheight + imgheight + quoteheight + d_toppadding + d_bottompadding);
  // right bottom
  pointie.lineTo(offset + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - d_radius,
                 bodyheight + datetimeheight + headerheight + imgheight + quoteheight + d_toppadding + d_bottompadding);
  pointie.arcTo(offset + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius,
                bodyheight + datetimeheight + headerheight + imgheight + quoteheight + d_toppadding + d_bottompadding - 2 * d_radius,
                2 * d_radius, 2 * d_radius, 270, 90);
  // right top
  pointie.lineTo(offset + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding, 0 + d_radius);
  pointie.arcTo(offset + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius, 0, 2 * d_radius, 2 * d_radius, 0, 90);
  // left top
  pointie.lineTo(offset + d_pointerwidth + d_radius, 0);
  pointie.arcTo(offset + d_pointerwidth, 0, 2 * d_radius, 2 * d_radius, 90, 90);
  // left bottom almost (here is the pointie)
  pointie.lineTo(offset + d_pointerwidth, bodyheight + datetimeheight + headerheight + imgheight + quoteheight + d_toppadding + d_bottompadding - d_pointerheight);
  pointie.closeSubpath();

  QPainterPath shadow;
  if (outgoing)
  {
    shadow.moveTo(0, bodyheight + datetimeheight + imgheight + quoteheight + d_toppadding + d_bottompadding - 1);
    shadow.lineTo(d_pointerwidth - 1, bodyheight + datetimeheight + imgheight + quoteheight + d_toppadding + d_bottompadding - 1 - d_pointerheight);

    shadow.moveTo(contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding, 0 + d_radius - 1);
    shadow.arcTo(contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius - 1, 0, 2 * d_radius, 2 * d_radius, 5, 85);
    shadow.lineTo(d_pointerwidth + d_radius, 0 - 1);
    shadow.arcTo(d_pointerwidth, 0 - 1, 2 * d_radius, 2 * d_radius, 95, 85);
  }
  else
  {
    shadow.moveTo(offset, bodyheight + datetimeheight + headerheight + imgheight + quoteheight + d_toppadding + d_bottompadding + 1);
    shadow.lineTo(offset + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - d_radius,
                  bodyheight + datetimeheight + headerheight + imgheight + quoteheight + d_toppadding + d_bottompadding + 1);
    shadow.arcTo(offset + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius,
                 bodyheight + datetimeheight + headerheight + imgheight + quoteheight + d_toppadding + d_bottompadding + 1 - 2 * d_radius,
                 2 * d_radius, 2 * d_radius, 270, 85);
  }

  // rotate bubble for outgoing
  if (outgoing)
  {
    painter->translate(option.rect.width() - pointie.boundingRect().width() - d_horizontalmargin - d_pointerwidth, 0);
    painter->translate(pointie.boundingRect().center());
    painter->rotate(180);
    painter->translate(-pointie.boundingRect().center());
  }

  // outline message_deleted /* or selected*/
  if (message_deleted /*|| (option.state & QStyle::State_Selected)*/) [[unlikely]]
  {
    painter->setPen(QColor("black"));
    painter->drawPath(pointie);
  }
  else
  {
    painter->setPen(QPen(bgcolor));
    painter->drawPath(pointie);
    painter->fillPath(pointie, QBrush(bgcolor));
    painter->setPen(QColor("#CCCCCC"));
    painter->drawPath(shadow);
  }

  if (outgoing) // rotate back or painter is going to paint the text rotated...
  {
    painter->translate(pointie.boundingRect().center());
    painter->rotate(-180);
    painter->translate(-pointie.boundingRect().center());
  }

  // draw header (name for groupmessages
  if (headerheight > 0)
  {
    painter->setFont(d_groupheaderfont);
    painter->setPen(QColor("white"));
    // painter->drawRect(d_pointerwidth + offset + d_leftpadding, d_headerpadding, contentswidth, headerheight - d_headerpadding);
    painter->drawText(d_pointerwidth + offset + d_leftpadding, d_headerpadding, contentswidth, headerheight - d_headerpadding, Qt::AlignLeft,
                      RecipientPreferences::getName(index.data(bepaald::RecipientIdRole).toString()));
  }
  painter->translate(0, headerheight + d_toppadding);

  QAbstractTextDocumentLayout::PaintContext ctx;

  // set text color
  if (bgcolor != QColor("#FFFFFF") && !message_deleted)
    ctx.palette.setColor(QPalette::Text, QColor("white"));
  else
    ctx.palette.setColor(QPalette::Text, QColor("black"));

  // paint quote
  if (hasquote)
  {

    QFont oldfont = painter->font();
    QPen oldpen = painter->pen();

    QPainterPath quoteframe;
    quoteframe.addRoundedRect((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, 60, quoteheight, d_radius, d_radius);
    QColor quotebg("#AAFFFFFF");
    QColor quoteoutline("#70FFFFFF");
    QColor quotebar("#FFFFFF");
    if (outgoing)
    {
      quotebg = bgcolor;
      quotebg.setAlpha(0x44);
      quoteoutline = "#44BDBDBD";
      quotebar = bgcolor; // this is not correct maybe
    }

    painter->fillPath(quoteframe, QBrush(quotebg));
    painter->setPen(quoteoutline);
    painter->drawPath(quoteframe);

    painter->setClipPath(quoteframe);
    painter->fillRect((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, 5 /* bar width */, quoteheight, quotebar);
    painter->setClipping(false);

    bepaald::Quote quote = index.data(bepaald::QuoteRole).value<bepaald::Quote>();

    painter->setPen(outgoing ?
                    Qt::black :
                    QColor(RecipientPreferences::getDarkColor(index.data(bepaald::RecipientIdRole).toString())));
    painter->setFont(QFont("Roboto", 11));
    painter->drawText((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + 5 /*barwidth*/, d_toppadding,
                      60 /*heigth*/, 20 /*width*/, Qt::AlignLeft,
                      RecipientPreferences::getName(quote.author));

    painter->setPen(outgoing ? Qt::black : Qt::white);
    painter->drawText((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + 5 /*barwidth*/, d_toppadding + 11,
                      60 /*heigth*/, 20 /*width*/, Qt::AlignLeft,
                      quote.body);

    painter->setFont(oldfont);
    painter->setPen(oldpen);
    painter->translate(0, quoteheight);
  }

  // paint image
  if (hasimageattachment)
  {
    QPainterPath imgframe;
    imgframe.addRoundedRect((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, totalimageswidth, totalimagesheight,
                            d_radius, d_radius);
    painter->setClipPath(imgframe);

    for (int i = 0; i < imgthumbs.size(); ++i)
    {
      if (i > 4)
        break;

      // draw pixmap

      painter->drawPixmap((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, imgthumbs[i].first);

      // draw play button on video
      if (imgthumbs[i].second == MsgAttachment::ATTACHMENTTYPE::VIDEO)
      {
        painter->drawPixmap((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + (imgthumbs[i].first.width() / 2 - d_mediabuttonsize / 2),
                            (imgthumbs[i].first.height() / 2 - d_mediabuttonsize / 2),
                            d_mediabuttonsize,
                            d_mediabuttonsize,
                            d_playbutton);
      }

      // draw '+X' on last thumb
      if (i == 4 && imgthumbs.size() > 5)
      {
        //qDebug() << "Painting extra number on row" << index.row();

        QFont oldfont = painter->font();
        QPen oldpen = painter->pen();
        painter->setFont(QFont("Roboto", imgthumbs[4].first.height() / 3.));
        painter->setPen(QColor("#FFFFFF"));

        // Conditional jump or move depends on uninitialised value(s)???
        painter->drawText((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0,
                          imgthumbs[4].first.width(), imgthumbs[4].first.height(),
                          Qt::AlignCenter, "+" + QString::number(imgthumbs.size() - 5));
        painter->setFont(oldfont);
        painter->setPen(oldpen);
      }

      // translate painter
      if (i == 0)
      {
        if (imgthumbs.size() > 1) // another image follows to the right
          painter->translate(imgthumbs[0].first.width() + 2, 0);
        else
          painter->translate(0, imgthumbs[0].first.height() + 2);
      }
      else if (i == 1)
      {
        if (imgthumbs.size() == 2 || imgthumbs.size() > 3) // this was last image or a new row is started
          painter->translate(-(imgthumbs[0].first.width() + 2), imgthumbs[0].first.height() + 2);
        else // if (imgthumbs.size() == 3) // another image is coming straight below this one
          painter->translate(0, imgthumbs[1].first.height() + 2);
      }
      else if (i == 2)
      {
        if (imgthumbs.size() == 3) // last image
          painter->translate(-(imgthumbs[0].first.width() + 2), imgthumbs[2].first.height() + 2);
        else // another image follows to the right
          painter->translate(imgthumbs[2].first.width() + 2, 0);

      }
      else if (i == 3)
      {
        if (imgthumbs.size() == 4) // last image
          painter->translate(-(imgthumbs[0].first.width() + 2), imgthumbs[2].first.height() + 2);
        else // another follows to the right
          painter->translate(imgthumbs[3].first.width() + 2, 0);
      }
      else if (i == 4) // last image
      {
        painter->translate(-(imgthumbs[2].first.width() + 2 + imgthumbs[3].first.width() + 2), imgthumbs[4].first.height() + 2);
      }


    }

    painter->setClipping(false);

    // // draw play button on top
    // //warning, this might not work for videos smaller than mediabutton? I think I have minimum size for thumbnail though....
    // if (firstattachment.type() == MsgAttachment::ATTACHMENTTYPE::VIDEO)
    // {
    //   painter->drawPixmap((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + (imgthumbs[0].width() / 2 - d_mediabuttonsize / 2),
    //                       (imgthumbs[0].height() / 2 - d_mediabuttonsize / 2),
    //                       d_mediabuttonsize,
    //                       d_mediabuttonsize,
    //                       d_playbutton);
    // }
    // painter->translate(0, imgthumbs[0].height());
  }

  if (audiomsgidx != -1)
  {
    // QPen oldpen = painter->pen();
    // painter->setPen(QColor("red"));
    // painter->drawRoundedRect((outgoing ? 0 : d_pointerwidth) + d_leftpadding, d_toppadding, contentswidth, d_mediabuttonsize, 5, 5);
    // painter->setPen(oldpen);

    std::shared_ptr<MediaPlayer> mp(nullptr);
    if (att_data_count > audiomsgidx && index.data(bepaald::AttachmentDataRole + audiomsgidx).isValid())
      mp = index.data(bepaald::AttachmentDataRole + audiomsgidx).value<std::shared_ptr<MediaPlayer>>();

    float position = mp ? mp->position() : 0;
    if (position > 1)
      position = 1;
    uint64_t duration = mp ? mp->duration() : 0;
    uint64_t time = mp ? mp->time() : 0;
    if (time == static_cast<uint64_t>(-1))
      time = 0;

    QString currenttime = QString::number(time / 60000).rightJustified(2, '0') + ":" + QString::number((time % 60000) / 1000).rightJustified(2, '0');
    QString lengthstring = QString::number(duration / 60000).rightJustified(2, '0') + ":" + QString::number((duration % 60000) / 1000).rightJustified(2, '0');

    // draw button
    QPixmap pm = d_pausebutton;
    if (!mp || !mp->isPlaying())
      pm = d_playbutton;
    if (!mp)
    {
      QPainter maskpainter(&pm);
      maskpainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      maskpainter.fillRect(pm.rect(), QBrush("#FF0000"));
      maskpainter.end();
    }
    if (outgoing)
    {
      QPainter maskpainter(&pm);
      maskpainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      maskpainter.fillRect(pm.rect(), QBrush("#000000"));
      maskpainter.end();
    }
    painter->drawPixmap((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, d_mediabuttonsize, d_mediabuttonsize, pm);

    QPainterPath progressbaroutline;
    progressbaroutline.addRoundedRect((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + d_mediabuttonsize + d_leftpadding, 20,
                                      contentswidth - ((outgoing ? 0 : d_pointerwidth) + d_leftpadding + d_mediabuttonsize + d_leftpadding), 10, 5, 5);
    painter->fillPath(progressbaroutline, outgoing ? QColor("black") : QColor("white"));

    QPainterPath progressbarfilling;
    progressbarfilling.addRoundedRect((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + d_mediabuttonsize + d_leftpadding + 2, 20 + 2,
                                      (contentswidth - ((outgoing ? 0 : d_pointerwidth) + d_leftpadding + d_mediabuttonsize + d_leftpadding + 4)) * position, 6, 5, 5);
    painter->fillPath(progressbarfilling, outgoing ? QColor("white") : QColor("black"));

    // painter->drawRect((outgoing ? 0 : d_pointerwidth) + d_leftpadding + d_mediabuttonsize + d_leftpadding, d_toppadding + 20 + 10 + d_toppadding,
    //                   contentswidth - ((outgoing ? 0 : d_pointerwidth) + d_leftpadding + d_mediabuttonsize + d_leftpadding), d_mediabuttonsize - (d_toppadding + 20 + 10 + d_toppadding));

    // paint time information
    QPen oldpen = painter->pen();
    painter->setPen(outgoing ? QColor("black") : QColor("white"));
    painter->drawText((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + d_mediabuttonsize + d_leftpadding,
                      20 + 10 + d_toppadding,
                      contentswidth - ((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding + d_mediabuttonsize + d_leftpadding),
                      d_mediabuttonsize - (20 + 10 + d_toppadding),
                      Qt::AlignRight, currenttime + "/" + lengthstring);

    painter->setPen(oldpen);
    painter->translate(0, d_mediabuttonsize);
  }

  // paint binary file
  if (binaryfilemsgidx != -1)
  {
    QPen oldpen = painter->pen();

    //MediaPlayer *mp = firstattachment.attachment().value<MediaPlayer *>();
    QPixmap filepixmap = d_binarypm.scaledToWidth(d_mediabuttonsize);// = firstattachment.attachment().value<QPixmap>().scaledToWidth(d_mediabuttonsize);
    if (!filepixmap.isNull())
    {
      //painter->fillRect((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, filepixmap.width(), filepixmap.height(), QBrush(Qt::red));
      //painter->fillRect((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, filepixmap.width(), filepixmap.height() / 2, QBrush(Qt::blue));

      painter->drawPixmap((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0, filepixmap.width(), filepixmap.height(), filepixmap);


      // paint extension on it
      QList<QString> att_filenames = index.data(bepaald::AttachmentFilenameRole).value<QList<QString>>();
      QString filename = att_filenames[binaryfilemsgidx];//firstattachment.filename();
      int pos = filename.lastIndexOf('.');
      if (pos >= 0 && pos >= filename.size() - 4) // if its is 3 chars or less
      {
        QString ext = filename.mid(pos + 1).toUpper();
        qreal factor = (static_cast<qreal>(filepixmap.width()) * .60) / static_cast<qreal>(painter->fontMetrics().boundingRect(ext).width());
        QFont oldfont = painter->font();
        if ((factor < 1) || (factor > 1.25))
        {
          QFont f = oldfont;
          f.setPointSizeF(f.pointSizeF() * factor);
          painter->setFont(f);
        }
        painter->setPen(QColor("black"));
        painter->drawText((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0,
                          filepixmap.width(), filepixmap.height(),
                          Qt::AlignCenter, ext);
        painter->setFont(oldfont);
      }

      // move to right of icon
      painter->translate(filepixmap.width(), 0);

      // draw filename
      painter->setPen(QColor(outgoing ? "black" : "white"));
      painter->setFont(d_bodyfont);
      // make sure contentswidth is set to width of this thing
      QString elidedfilename = painter->fontMetrics().elidedText(filename, Qt::ElideRight, contentswidth - filepixmap.width());
      int ypos = (filepixmap.height() / 2);// + painter->fontMetrics().ascent();
      painter->drawText((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, ypos, elidedfilename);
      painter->drawText((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, ypos + painter->fontMetrics().ascent(), filesizestring);

      // move back to base xpos, and to below the image
      painter->translate(-filepixmap.width(), filepixmap.height());
    }
    painter->setPen(oldpen);
  }

  // draw body text
  painter->translate((outgoing ? 0 : d_pointerwidth) + offset + d_leftpadding, 0);
  bodydoc.documentLayout()->draw(painter, ctx);

  painter->translate(0, bodyheight);

  // draw footer (date/time + reception stat)
  painter->setFont(d_footerfont);
  painter->setPen(QColor(outgoing || message_deleted ? "black" : "white"));
  if (outgoing && /*[[unlikely]]*/
      ((index.data(bepaald::MsgTypeRole).toULongLong() & Types::BASE_TYPE_MASK) == Types::BASE_OUTBOX_TYPE ||
       (index.data(bepaald::MsgTypeRole).toULongLong() & Types::BASE_TYPE_MASK) == Types::BASE_SENDING_TYPE))
  {
    //qDebug() << "PENDING!";
    painter->drawText(0, 0, contentswidth, datetimeheight, Qt::AlignRight, index.data(bepaald::DateTimeRole).toString() + "...");
  }
  else if (outgoing && /*[[unlikely]]*/ index.data(bepaald::DeliveryReceiptCountRole).toULongLong() == 0)
  {
    //qDebug() << "SENT NOT RECEIVED!";
    painter->drawText(0, 0, contentswidth - ((d_sent_not_received.width() / (d_sent_not_received.height() / static_cast<float>(fm.ascent()))) + d_checkmarkpadding), datetimeheight, Qt::AlignRight, index.data(bepaald::DateTimeRole).toString());
    QPixmap mask = d_sent_not_received;
    QPainter maskpainter(&mask);
    maskpainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    maskpainter.fillRect(mask.rect(), QBrush("#070C14")); // 'core-light-90'
    maskpainter.end();
    painter->drawPixmap(contentswidth - (mask.width() / (mask.height() / static_cast<float>(fm.ascent()))), 0,
                        mask.width() / (mask.height() / static_cast<float>(fm.ascent())), painter->fontMetrics().ascent(),
                        mask);
  }
  else if (outgoing && /*[[unlikely]]*/ index.data(bepaald::DeliveryReceiptCountRole).toULongLong() > 0)
  {
    //qDebug() << "RECEIVED!";
    painter->drawText(0, 0, contentswidth - ((d_received.width() / (d_received.height() / static_cast<float>(fm.ascent()))) + d_checkmarkpadding), datetimeheight, Qt::AlignRight, index.data(bepaald::DateTimeRole).toString());
    //painter->drawPixmap(datetimewidthnochecks, -4/*-painter->fontMetrics().ascent()*/, d_received);
    QPixmap mask = d_received;
    QPainter maskpainter(&mask);
    maskpainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    if (index.data(bepaald::ReadReceiptCountRole).toULongLong() > 0)
      maskpainter.fillRect(mask.rect(), QBrush("#2090EA")); // 'core-blue'
    else
      maskpainter.fillRect(mask.rect(), QBrush("#070C14")); // 'core-light-90'
    maskpainter.end();
    painter->drawPixmap(contentswidth - (mask.width() / (mask.height() / static_cast<float>(fm.ascent()))), 0,
                        mask.width() / (mask.height() / static_cast<float>(fm.ascent())), painter->fontMetrics().ascent(),
                        mask);
  }
  else // incoming
    painter->drawText(0, 0, contentswidth, datetimeheight, Qt::AlignLeft, index.data(bepaald::DateTimeRole).toString());

  // qDebug() << index.data(bepaald::DateTimeRole).toString()
  //          << "date width:" << datetimewidth

  painter->restore();

  if constexpr (ConversationListViewDelegate::showhelplines)
  {
    painter->save();
    painter->setPen(QPen(Qt::red));
    painter->drawRect(option.rect);
    painter->drawLine(option.rect.topLeft(), option.rect.bottomRight());
    painter->drawLine(option.rect.topRight(), option.rect.bottomLeft());
    painter->restore();
  }
}

#include "conversationlistviewdelegate.ih"

#include "../emojitextdocument/emojitextdocument.h"

#include <QtMath>

QSize ConversationListViewDelegate::datemessageSizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  EmojiTextDocument datetimetext;
  datetimetext.setDefaultFont(d_datetimemessagefont);
  datetimetext.setHtml(index.data(bepaald::DateMessageRole).toString().toStdString());
  QSize size(option.rect.width(), datetimetext.size().height() + d_bottompadding + d_toppadding + d_verticalmargin + 1);
  if (index.row() == 0) [[unlikely]] // have extra margin at top of first item
    size += QSize(0, d_verticalmargin);
  return size;
}

QSize ConversationListViewDelegate::statusmessageSizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  EmojiTextDocument statusdoc;
  statusdoc.setDefaultFont(d_statusbodyfont);
  statusdoc.setDefaultStyleSheet(".emoji, .jumbo-emoji {font-size: " + QString::number(QFontMetrics(d_statusbodyfont).height() + 4) + "px; font-family: emoji;}");
  statusdoc.setHtml(getStatusMessage(index).toStdString());
  qreal contentswidth = option.rect.width() - d_status_group.width() - d_horizontalmargin - d_horizontalmargin - d_horizontalmargin;
  statusdoc.setTextWidth(contentswidth);

  contentswidth = statusdoc.idealWidth();

  QSize size(contentswidth + d_horizontalmargin,
             statusdoc.size().height() + d_verticalmargin + 1);

  if (size.height() < d_status_group.height() + d_verticalmargin)
    size.setHeight(d_status_group.height() + d_verticalmargin);

  if (index.row() == 0) [[unlikely]]  // have extra margin at top of first item
    size += QSize(0, d_verticalmargin);

  return size;
}

QSize ConversationListViewDelegate::quoteSizeHint(QStyleOptionViewItem const &/*option*/, QModelIndex const &/*index*/) const
{
  return QSize();
}

QSize ConversationListViewDelegate::scaledToWidth(qreal target, QSize const &size) const
{
  //qreal factor = static_cast<qreal>(qFloor(target)) / size.width();
  //int hd = qRound(qAbs(factor) * size.height());
  //int wd = qRound(qAbs(factor) * size.width());
  //QSize newsize(qRound(qAbs(factor) * size.height()), qRound(qAbs(factor) * size.width()));
  return {qRound((static_cast<qreal>(qFloor(target)) / size.width()) * size.width()),
          qRound((static_cast<qreal>(qFloor(target)) / size.width()) * size.height())};
}

QSize ConversationListViewDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  //std::cout << "Sizehint called for row " << index.row() << std::endl;

  if (/*[[unlikely]]*/!index.data(bepaald::DateMessageRole).isNull())
    return datemessageSizeHint(option, index);

  if (/*[[unlikely]] */Types::isStatusMessage(index.data(bepaald::MsgTypeRole).toULongLong()))
    return statusmessageSizeHint(option, index);

  bool outgoing = isOutgoing(index.data(bepaald::MsgTypeRole).toULongLong());

  bool message_deleted = index.data(bepaald::MsgDeletedRole).toBool();

  EmojiTextDocument bodydoc;
  QTextOption textOption(bodydoc.defaultTextOption());
  textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  bodydoc.setDefaultTextOption(textOption);
  bodydoc.setDefaultFont(d_bodyfont);
  bodydoc.setDefaultStyleSheet(".emoji {font-size: " + QString::number(QFontMetrics(d_bodyfont).height() + 4) + "px; font-family: emoji;} .jumbo-emoji {font-size: " + QString::number((QFontMetrics(d_bodyfont).height() + 4) * 3) + "px; font-family: emoji;}");
  QString bodytext(message_deleted ? (outgoing ? "You deleted this message." : "This message was deleted.") : index.data(Qt::DisplayRole).toString());
  bodydoc.setHtml(bodytext.toStdString());

  QFontMetrics fm(d_footerfont);
  qreal pixelsHigh = fm.height();

  qreal contentswidth = option.rect.width() * d_widthfraction - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
  if (!outgoing && index.data(bepaald::IsGroupThreadRole).toBool())
  {
    contentswidth -= (d_groupavatarsize + 0/*padding left and right*/);
    pixelsHigh *= 2; // for header
  }


  // check valid? should not be possible to not be valid though....
  QList<QString> att_types = index.data(bepaald::AttachmentTypeRole).value<QList<QString>>();
  QList<QSize> att_sizes = index.data(bepaald::AttachmentSizeRole).value<QList<QSize>>();
  QList<bool> att_quotes = index.data(bepaald::AttachmentQuoteRole).value<QList<bool>>();
  int imgcount = 0;
  QSize firstimagesize{-1, -1}; // !valid
  bool hasaudio = false;
  QSize hasbinaryfile{0, 0};
  for (int i = 0; i < att_types.size(); ++i)
  {
    if (att_quotes[i])
      continue;
    if (att_types[i].startsWith("image/") || att_types[i].startsWith("video/"))
    {
      if (!firstimagesize.isValid())
        firstimagesize = att_sizes[i];
      ++imgcount;
    }
    else if (att_types[i].startsWith("audio/"))
      hasaudio = true;
    else // if (tmp.type() == MsgAttachment::ATTACHMENTTYPE::BINARY)
      hasbinaryfile = d_binarypm.scaledToWidth(d_mediabuttonsize).size(); // ????????
  }
  if (firstimagesize.isNull())
    firstimagesize = {1, 1};

  QSize totalimagessize(0, 0);

  // if has image attachment
  if (imgcount)
  {
    // single image
    if (/*[[likely]]*/imgcount == 1)
    {
      if (firstimagesize.width() > option.rect.width() * .5)
        totalimagessize = scaledToWidth(option.rect.width() * .5, firstimagesize);
      else if (firstimagesize.width() < option.rect.width() * .5)
      {
        contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
        //totalimagessize = scaledToWidth(contentswidth, firstimagesize);
        totalimagessize = scaledToWidth(option.rect.width() * .5, firstimagesize);
      }
      else
        totalimagessize = firstimagesize;
    }
    else
    {
      // crop image square
      if (firstimagesize.width() > firstimagesize.height())
        firstimagesize.setWidth(firstimagesize.height());
      else
        firstimagesize.setHeight(firstimagesize.width());

      if (imgcount == 2)
      {
        if (firstimagesize.width() * 2 + 2 > contentswidth) // +2 two pixel margin
          totalimagessize = scaledToWidth(contentswidth / 2 - 1, firstimagesize); // -1 for single pixel margin
        else if (firstimagesize.width() * 2  + 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          totalimagessize = scaledToWidth(contentswidth / 2 - 1, firstimagesize); // -1 for single pixel margin
        }
        totalimagessize.setWidth(totalimagessize.width() * 2 + 2);
      }
      else if (imgcount == 3)
      {
        if (firstimagesize.width() * 1.5  + 2 > contentswidth)
          totalimagessize = scaledToWidth(contentswidth / 1.5 - 1, firstimagesize);
        else if (firstimagesize.width() * 1.5  + 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          totalimagessize = scaledToWidth(contentswidth / 1.5 - 1, firstimagesize);
        }
        totalimagessize.setWidth(totalimagessize.width()
                                 + scaledToWidth(contentswidth / 3 - 1, firstimagesize).width()
                                 + 2);
      }
      else if (imgcount == 4)
      {
        if (firstimagesize.width() * 2  + 2 > contentswidth)
          totalimagessize = scaledToWidth(contentswidth / 2 - 1, firstimagesize);
        else if (firstimagesize.width() * 2  + 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          totalimagessize = scaledToWidth(contentswidth / 2 - 1, firstimagesize);
        }
        totalimagessize.setWidth(totalimagessize.width() * 2 + 2);
        totalimagessize.setHeight(totalimagessize.height() * 2 + 2);
      }
      else // >4
      {
        if (firstimagesize.width() * 2  + 2 > contentswidth)
          totalimagessize = scaledToWidth(contentswidth / 2 - 1, firstimagesize);
        else if (firstimagesize.width() * 2  + 2 < option.rect.width() * .5)
        {
          contentswidth = option.rect.width() * .5 - d_horizontalmargin - d_pointerwidth - d_leftpadding - d_rightpadding;
          totalimagessize = scaledToWidth(contentswidth / 2 - 1, firstimagesize);
        }
        totalimagessize.setWidth(totalimagessize.width() * 2 + 2);
        totalimagessize.setHeight(totalimagessize.height()
                                  + scaledToWidth(contentswidth / 3 - 1, firstimagesize).height()
                                  + 4);
      }
    }
  }

  if (/*[[unlikely]]*/ hasaudio) // right now, hasaudio implies imgcount == 0
  {
    if (totalimagessize.width() < contentswidth)
      totalimagessize.setWidth(contentswidth);
    totalimagessize.setHeight(totalimagessize.height() + d_mediabuttonsize);
  }

  //if (/*[[unlikely]]*/ !hasbinaryfile.isNull()) // right now, hasbinaryfile implies imgcount == 0
  //{                                   // we dont care about size of text, it will not word wrap so two lines will never be
  totalimagessize += hasbinaryfile; // higher than the file icon.
  //}

  bodydoc.setTextWidth(contentswidth);

  contentswidth = std::max(// std::max(
                           qCeil(bodydoc.idealWidth())// , pixelsWide)
                           , totalimagessize.width()); // also mov

  bool hasquote = false;
  if (index.data(bepaald::QuoteRole).isValid())
    hasquote = true;
  int quoteheight = hasquote ? 50 : 0; // calc quoteheight;

  QSize size(contentswidth + d_horizontalmargin + d_pointerwidth + d_leftpadding + d_rightpadding,
             (bodytext.isEmpty() ? d_horizontalmargin : bodydoc.size().height()) + pixelsHigh + totalimagessize.height() + quoteheight + d_bottompadding + d_toppadding + d_verticalmargin + 1);

  if (/*[[unlikely]]*/ index.row() == 0) // have extra margin at top of first item
    size += QSize(0, d_verticalmargin);

  return size;
}

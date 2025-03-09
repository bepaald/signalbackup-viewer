#include "threadviewdelegate.ih"

#include <QDebug>

#include "../emojitextdocument/emojitextdocument.h"

void ThreadViewDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);

  if (option.state & QStyle::State_Selected)
    painter->fillRect(option.rect, QBrush("#EEEEEE"));

  // painter->setPen(QColor("grey"));
  // painter->drawRect(0, 0, options.rect.width(), options.rect.height());

  painter->translate(option.rect.topLeft());

  QString name = index.data(Qt::DisplayRole).toString();

  // Draw avatar
  paintAvatar(painter, d_horizontalmargin, d_verticalmargin, d_avatarsize, index.data(bepaald::RecipientIdRole).toString());

  // draw text
  painter->translate(d_horizontalmargin + d_horizontalmargin + d_avatarsize, d_verticalmargin);

  EmojiTextDocument contactdoc;
  QTextOption textOption(contactdoc.defaultTextOption());
  QFont thread_contact_font("Roboto");
  textOption.setWrapMode(QTextOption::NoWrap);
  contactdoc.setDocumentMargin(0);
  contactdoc.setDefaultTextOption(textOption);
  contactdoc.setDefaultFont(thread_contact_font);
  contactdoc.setDefaultStyleSheet(".emoji, .jumbo-emoji {font-size: " + QString::number(QFontMetrics(thread_contact_font).height()) + "px; font-family: emoji;}");
  contactdoc.setHtml(name.toStdString());

  EmojiTextDocument snippetdoc;
  textOption.setWrapMode(QTextOption::NoWrap);
  snippetdoc.setDocumentMargin(0);
  snippetdoc.setDefaultTextOption(textOption);
  QFont thread_snippet_font("Roboto", 10);
  if (Types::isIdentityUpdate(index.data(bepaald::MsgTypeRole).toULongLong()) ||
      index.data(bepaald::MultiMediaSnippetRole).toBool())
    thread_snippet_font.setItalic(true);
  snippetdoc.setDefaultFont(thread_snippet_font);
  snippetdoc.setDefaultStyleSheet(".emoji, .jumbo-emoji {font-size: " + QString::number(QFontMetrics(thread_contact_font).height()) + "px; font-family: emoji;}");
  std::string snippet = index.data(bepaald::SnippetRole).toString().toStdString();
  snippetdoc.setHtml(snippet.substr(0, snippet.find('\n')));

  int totalheight = contactdoc.size().height() + snippetdoc.size().height(); // total height of docs

  QAbstractTextDocumentLayout::PaintContext ctx;
  ctx.palette.setColor(QPalette::Text, QColor("black"));

  // draw contact info, elide if necessary
  painter->translate(0, ((option.rect.height() - d_verticalmargin - d_verticalmargin) /*available height*/ - totalheight) / 2);

  painter->setClipRect(0, 0, option.rect.width() - (d_horizontalmargin + d_horizontalmargin + d_avatarsize + d_horizontalmargin) - painter->pen().width(), totalheight);
  contactdoc.documentLayout()->draw(painter, ctx);
  if (contactdoc.idealWidth() > option.rect.width() - (d_horizontalmargin + d_horizontalmargin + d_avatarsize + d_horizontalmargin))
  {
    //qDebug() << "Contact clipping!";
    contactdoc.setHtml("...");
    painter->translate(option.rect.width() - (d_horizontalmargin + d_horizontalmargin + d_avatarsize + d_horizontalmargin) - contactdoc.idealWidth(), 0);

    if (option.state & QStyle::State_Selected)
      painter->fillRect(0, 0, contactdoc.idealWidth(), contactdoc.size().height(), QBrush("#EEEEEE"));
    else
      painter->fillRect(0, 0, contactdoc.idealWidth(), contactdoc.size().height(), "white");

    contactdoc.documentLayout()->draw(painter, ctx);
    painter->translate(-(option.rect.width() - (d_horizontalmargin + d_horizontalmargin + d_avatarsize + d_horizontalmargin) - contactdoc.idealWidth()), 0);
  }

  // draw snippet, elide if necessary
  painter->translate(0, contactdoc.size().height());
  snippetdoc.documentLayout()->draw(painter, ctx);

  if (snippetdoc.idealWidth() > option.rect.width() - (d_horizontalmargin + d_horizontalmargin + d_avatarsize + d_horizontalmargin))
  {
    //qDebug() << "Snippet clipping!";
    snippetdoc.setHtml("...");
    painter->translate(option.rect.width() - (d_horizontalmargin + d_horizontalmargin + d_avatarsize + d_horizontalmargin) - snippetdoc.idealWidth(), 0);
    if (option.state & QStyle::State_Selected)
      painter->fillRect(0, 0, snippetdoc.idealWidth() + 1, snippetdoc.size().height(), QBrush("#EEEEEE"));
    else
      painter->fillRect(0, 0, snippetdoc.idealWidth() + 1, snippetdoc.size().height(), "white");

    snippetdoc.documentLayout()->draw(painter, ctx);
    painter->translate(-(option.rect.width() - (d_horizontalmargin + d_horizontalmargin + d_avatarsize + d_horizontalmargin) - snippetdoc.idealWidth()), 0);
  }

  painter->setClipping(false);

  painter->restore();

}

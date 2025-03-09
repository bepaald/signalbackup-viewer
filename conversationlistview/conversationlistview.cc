#include "conversationlistview.ih"

#include <QDebug>

ConversationListView::ConversationListView(QWidget *parent)
  :
  SignalListViewBase(parent),
  d_model(nullptr),
  d_listreallypainted(false),
  d_currentresultsidx(0),
  d_scrollanimation(new QPropertyAnimation(verticalScrollBar(), "value")),
  d_loading(false),
  d_conversationdelegate(new ConversationListViewDelegate(this))
{
  d_scrollanimation->setEasingCurve(QEasingCurve::OutQuad);
  d_scrollanimation->setDuration(500);

  setSelectionMode(QAbstractItemView::NoSelection);
  viewport()->setAutoFillBackground(true);
  QPalette plt = viewport()->palette();
  plt.setColor(viewport()->backgroundRole(), QColor("#EEEEEE"));
  viewport()->setPalette(plt);

  setItemDelegate(d_conversationdelegate);

  //connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &ConversationListView::controlMovies);

  connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &ConversationListView::handleAttachmentView);

  connect(this, &ConversationListView::clicked, this, &ConversationListView::indexSelected);
  connect(this, &ConversationListView::doubleClicked, this, &ConversationListView::details);
}

void ConversationListView::handleAttachmentView()
{
  qInfo() << "handle attachments";
  if (!d_model)
    return;

  QList<QModelIndex> itemsinview;

  QModelIndex firstidx = indexAt(QPoint(0, 0));
  if (firstidx.isValid())
    itemsinview << firstidx;

  while (viewport()->rect().contains(QPoint(0, visualRect(firstidx).y() + visualRect(firstidx).height() + 1)))
  {
    firstidx = indexAt(QPoint(0, visualRect(firstidx).y() + visualRect(firstidx).height() + 1));
    if (!firstidx.isValid())
      break;
    itemsinview << firstidx;
  }

  qInfo() << itemsinview;

  d_model->itemsInView(itemsinview);
}

#include <QDebug>
void ConversationListView::indexSelected(QModelIndex const &idx)
{
  qInfo() << "Clicked row " << idx.row();

  if (idx.data(bepaald::AttachmentDataCountRole).isValid() &&
      idx.data(bepaald::AttachmentDataCountRole).toInt() == 1)
  {

    qInfo() << "Row has attachment : " << idx.data(bepaald::AttachmentTypeRole).value<QList<QString>>();

    if (idx.data(bepaald::AttachmentTypeRole).value<QList<QString>>().size() &&
        idx.data(bepaald::AttachmentTypeRole).value<QList<QString>>()[0].startsWith("audio"))
    {
      qInfo() << "Clicked audio!";
      std::shared_ptr<MediaPlayer> mp = idx.data(bepaald::AttachmentDataRole).value<std::shared_ptr<MediaPlayer>>();

      if (mp->isPlaying())
      {
        mp->pause();
        updateViewport(); // to force redraw -> show play button
      }
      else
      {
        mp->disconnect(); // disconnect all previous things, they are reconnected here anyway...
        d_mediaplayerconnections.clear();
        d_mediaplayerconnections.append(QObject::connect(mp.get(), &MediaPlayer::newPositionAvailable, this, &ConversationListView::updateViewport));
        d_mediaplayerconnections.append(QObject::connect(mp.get(), &MediaPlayer::mediaReachedEnd, this, &ConversationListView::mediaReachedEnd));
        mp->play();
      }
    }
    else
    if (idx.data(bepaald::AttachmentTypeRole).value<QList<QString>>().size() &&
        idx.data(bepaald::AttachmentTypeRole).value<QList<QString>>()[0].startsWith("video"))
    {
      qInfo() << "Clicked video!";
    }
  }
}

void ConversationListView::details(QModelIndex const &idx)
{
  qDebug() << "Double-clicked row " << idx.row();
  d_model->showDetails(idx);
  //QVariantList fulldata = d_model->getFullData(idx);
  //emit showDetails(d_model->fields(), idx.data(bepaald::FullRawDataRole).toList());
}

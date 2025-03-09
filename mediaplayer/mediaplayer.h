#ifndef MEDIAPLAYER_H_
#define MEDIAPLAYER_H_

#include <QObject>

#include "../vlcbase/vlcbase.h"

class MediaPlayer : public QObject, public VlcBase
{

  Q_OBJECT;

  float d_position;
 public:
  inline MediaPlayer(unsigned char *data, uint64_t datasize);
  inline MediaPlayer(char *data, uint64_t datasize);
  inline void play();
  inline void pause();
  inline void stop();
  inline bool isPlaying();
  virtual ~MediaPlayer();
  inline float position() const;
  inline uint64_t time();
 private:
  inline static void positionChanged(libvlc_event_t const *p_event, void *p_data);
  inline static void endReached(libvlc_event_t const *p_event, void *p_data);
  inline bool ensurePlayer();
 signals:
  void newPositionAvailable();
  void mediaReachedEnd();
};

#include <QDebug>

inline MediaPlayer::MediaPlayer(unsigned char *data, uint64_t datasize)
  :
  VlcBase(data, datasize),
  d_position(0)
{
  // qInfo() << " *** MP CSTOR!";
}

inline MediaPlayer::MediaPlayer(char *data, uint64_t datasize)
  :
  MediaPlayer(reinterpret_cast<unsigned char *>(data), datasize)
{}

inline bool MediaPlayer::ensurePlayer()
{
  if (d_initialized)
    return true;
  if (init())
  {
    libvlc_event_attach(libvlc_media_player_event_manager(d_mediaplayer), libvlc_MediaPlayerPositionChanged, positionChanged, this);
    libvlc_event_attach(libvlc_media_player_event_manager(d_mediaplayer), libvlc_MediaPlayerEndReached, endReached, this);
    return true;
  }
  return false;
}

inline void MediaPlayer::play()
{
  if (ensurePlayer())
    libvlc_media_player_play(d_mediaplayer);
}

inline void MediaPlayer::pause()
{
  if (ensurePlayer())
    libvlc_media_player_pause(d_mediaplayer);
}

inline void MediaPlayer::stop()
{
  if (ensurePlayer())
    libvlc_media_player_stop(d_mediaplayer);
}

inline bool MediaPlayer::isPlaying()
{
  if (ensurePlayer())
    return libvlc_media_player_get_state(d_mediaplayer) == libvlc_Playing;
  return false;
}

inline float MediaPlayer::position() const
{
  return d_position;
}

inline uint64_t MediaPlayer::time()
{
  if (ensurePlayer())
    return libvlc_media_player_get_time(d_mediaplayer);
  return 0;
}

inline void MediaPlayer::positionChanged(libvlc_event_t const *p_event, void *p_data)
{
  MediaPlayer *self = reinterpret_cast<MediaPlayer *>(p_data);
  self->d_position = p_event->u.media_player_position_changed.new_position;
  emit self->newPositionAvailable();
}

inline void MediaPlayer::endReached(libvlc_event_t const *, void *p_data)
{
  MediaPlayer *self = reinterpret_cast<MediaPlayer *>(p_data);
  self->d_position = 1;
  emit self->mediaReachedEnd();
}


#endif

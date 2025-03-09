#include "mediaplayer.ih"

#include <iostream>

MediaPlayer::~MediaPlayer()
{

  // qInfo() << " *** MP DSTOR!";

  if (d_initialized)
  {
    libvlc_event_detach(libvlc_media_player_event_manager(d_mediaplayer), libvlc_MediaPlayerPositionChanged, positionChanged, this);
    libvlc_event_detach(libvlc_media_player_event_manager(d_mediaplayer), libvlc_MediaPlayerEndReached, endReached, this);
  }
}

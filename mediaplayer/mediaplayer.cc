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

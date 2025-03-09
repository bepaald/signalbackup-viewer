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

#include "vlcbase.ih"

bool VlcBase::init()
{
  d_instance = libvlc_new(0, nullptr);
  if (d_instance)
  {
    // disable logging output
    // if (quiet)
    libvlc_log_unset(d_instance);

    d_media = libvlc_media_new_callbacks(d_instance,
                                         callback_vlc_open, callback_vlc_read, callback_vlc_seek, callback_vlc_close,
                                         &d_videodata);
    if (d_media)
    {
      libvlc_event_attach(libvlc_media_event_manager(d_media), libvlc_MediaParsedChanged, preparsed, this);
      libvlc_event_attach(libvlc_media_event_manager(d_media), libvlc_MediaDurationChanged, durationChanged, this);
      d_parsing = true;
      d_mediaplayer = libvlc_media_player_new_from_media(d_media);
      d_initialized = true;
    }
  }
  return d_initialized;
}

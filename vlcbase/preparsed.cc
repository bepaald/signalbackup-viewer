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

#include <iostream>

void VlcBase::preparsed(libvlc_event_t const *p_event, void *p_data)
{
  if (p_event->u.media_parsed_changed.new_status != libvlc_media_parsed_status_done)
    return;

  VlcBase *self = reinterpret_cast<VlcBase *>(p_data);
  self->d_hasvideo = false;
  libvlc_media_track_t **mediatracks;
  unsigned trackcount = libvlc_media_tracks_get(self->d_media, &mediatracks);
  for (uint t = 0; t < trackcount; ++t)
  {
    if (mediatracks[t]->i_type == libvlc_track_video)
    {
      self->d_hasvideo = true;
      self->d_orientation = mediatracks[t]->video->i_orientation;
      self->d_parsing = false;
      break;
    }
    // else if (mediatracks[t]->i_type == libvlc_track_audio)
    //   ;//std::cout << " - Audio track" << std::endl;
  }
  libvlc_media_tracks_release(mediatracks, trackcount);
  libvlc_event_detach(libvlc_media_event_manager(self->d_media), libvlc_MediaParsedChanged, preparsed, p_data);
}

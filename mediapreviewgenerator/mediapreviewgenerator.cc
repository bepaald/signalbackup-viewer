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

#include "mediapreviewgenerator.ih"

MediaPreviewGenerator::MediaPreviewGenerator(unsigned char *data, uint64_t size)
  :
  VlcBase(data, size)
{
  if (init())
  {
    libvlc_audio_set_volume(d_mediaplayer, 0); // mute
    libvlc_audio_set_track(d_mediaplayer, -1); // select NO audio track
    libvlc_video_set_callbacks(d_mediaplayer, callback_vlc_lock, callback_vlc_unlock, callback_vlc_display, &d_context);
    libvlc_video_set_format_callbacks(d_mediaplayer, callback_vlc_format, nullptr);
  }
}

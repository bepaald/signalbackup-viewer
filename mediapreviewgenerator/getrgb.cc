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

VideoContext *MediaPreviewGenerator::getRgb()
{
  if (!d_hasvideo)
    return nullptr;

  libvlc_media_player_play(d_mediaplayer);

  int count = 1000;
  int sleepms = 10;
  while (d_context.currentFrame() == 0)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds{sleepms});
    if (!d_hasvideo)
    {
      std::cout << "Audio file" << std::endl;
      libvlc_media_player_stop(d_mediaplayer);
      return nullptr;
    }
    if (--count <= 0)
    {
      std::cout << "Did not get screenshot within " << (sleepms * count) / 1000 << " second(s), breaking" << std::endl;
      libvlc_media_player_stop(d_mediaplayer);
      return nullptr;
    }
  }

  libvlc_media_player_stop(d_mediaplayer);

  // rotate?
  if (d_orientation == libvlc_video_orient_left_bottom || d_orientation == libvlc_video_orient_right_top)
  {
    rotate90(d_context.firstFrame(), d_context.width(), d_context.height());
    d_context.switchDimensions();
    d_context.setRotation(-90);
  }
  return &d_context;
}

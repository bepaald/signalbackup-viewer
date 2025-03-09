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

#ifndef VLCBASE_H_
#define VLCBASE_H_

#include <vlc/vlc.h>

#include <thread>
#include <algorithm>
#include <cstring>

#include <iostream>

class VlcBase
{
  libvlc_instance_t *d_instance;
  libvlc_media_t *d_media;
  bool d_parsing;
 protected:
  struct VideoDataStruct
  {
    unsigned char *data;
    uint64_t datasize;
    uint64_t datapos;
  } d_videodata;
  libvlc_media_player_t *d_mediaplayer;
  bool d_hasvideo;
  libvlc_video_orient_t d_orientation;
  uint64_t d_duration;
  bool d_initialized;
 public:
  inline VlcBase(unsigned char *data, uint64_t datasize);
  virtual ~VlcBase();
  inline uint64_t duration() const;
 protected:
  bool init();
 private:
  inline static int callback_vlc_open(void *opaque, void **data, uint64_t *sizep);
  inline static long int callback_vlc_read(void *opaque, unsigned char *buf, size_t len);
  inline static int callback_vlc_seek(void *opaque, uint64_t offset);
  inline static void callback_vlc_close(void *opaque);
  static void preparsed(libvlc_event_t const *p_event, void *p_data);
  static void durationChanged(libvlc_event_t const *p_event, void *p_data);
};

inline VlcBase::VlcBase(unsigned char *data, uint64_t datasize)
  :
  d_instance(nullptr),
  d_media(nullptr),
  d_parsing(false),
  d_videodata{data, datasize, 0},
  d_mediaplayer(nullptr),
  d_hasvideo(true),
  d_orientation(libvlc_video_orient_top_left),
  d_duration(0),
  d_initialized(false)
{}

inline uint64_t VlcBase::duration() const
{
  return d_duration;
}

int VlcBase::callback_vlc_open(void *opaque, void **data, uint64_t *sizep)
{
  VideoDataStruct *context = reinterpret_cast<VideoDataStruct *>(opaque);
  context->datapos = 0;
  *sizep = context->datasize;
  *data = context;
  return 0;
}

long int VlcBase::callback_vlc_read(void *opaque, unsigned char *buf, size_t len)
{
  VideoDataStruct *context = reinterpret_cast<VideoDataStruct *>(opaque);
  if (context->datapos >= context->datasize)
    return 0;
  size_t tocpy = std::min(len, context->datasize - context->datapos);
  std::memcpy(buf, context->data + context->datapos, tocpy);
  context->datapos += tocpy;

  return tocpy;
}

int VlcBase::callback_vlc_seek(void *opaque, uint64_t offset)
{
  VideoDataStruct *context = reinterpret_cast<VideoDataStruct *>(opaque);
  if (offset <= context->datasize)
  {
    context->datapos = offset;
    return 0;
  }
  return -1;
}

void VlcBase::callback_vlc_close(void *)
{}

#endif

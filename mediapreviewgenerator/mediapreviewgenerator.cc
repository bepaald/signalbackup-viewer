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

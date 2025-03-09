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

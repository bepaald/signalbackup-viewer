#include "vlcbase.ih"

VlcBase::~VlcBase()
{
  if (d_initialized)
    libvlc_event_detach(libvlc_media_event_manager(d_media), libvlc_MediaDurationChanged, durationChanged, this);

  if (d_mediaplayer)
    libvlc_media_player_release(d_mediaplayer);
  if (d_media)
    libvlc_media_release(d_media);
  if (d_instance)
    libvlc_release(d_instance);
}

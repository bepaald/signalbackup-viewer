#include "vlcbase.ih"

void VlcBase::durationChanged(libvlc_event_t const *p_event, void *p_data)
{
  VlcBase *self = reinterpret_cast<VlcBase *>(p_data);
  self->d_duration = p_event->u.media_duration_changed.new_duration;
}

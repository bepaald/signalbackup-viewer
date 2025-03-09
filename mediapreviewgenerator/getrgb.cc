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

#ifndef MEDIAPREVIEWGENERATOR_H_
#define MEDIAPREVIEWGENERATOR_H_

#include "../vlcbase/vlcbase.h"

class VideoContext
{
  unsigned char *d_firstframe;
  unsigned char *d_pixeldata;
  uint64_t d_currentframe;
  int d_width;
  int d_height;
  int d_rotation;
 public:
  inline VideoContext() : d_firstframe(nullptr), d_pixeldata(nullptr), d_currentframe(0), d_width(0), d_height(0), d_rotation(0) {};
  inline ~VideoContext() { destroy(); };
  inline void resetData(uint64_t size) { destroy() ; d_firstframe = new unsigned char[size]; d_pixeldata = new unsigned char[size]; };
  inline void setSize(int w, int h) { d_width = w; d_height = h; };
  inline void setRotation(int r) { d_rotation = r; };
  inline void upFrame() { ++d_currentframe; };
  inline uint64_t currentFrame() const { return d_currentframe; };
  inline int width() const { return d_width; };
  inline int height() const { return d_height; };
  inline int rotation() const { return d_rotation; };
  inline unsigned char *firstFrame() const { return d_firstframe; };
  inline unsigned char *pixelData() const { return d_pixeldata; };
  inline void switchDimensions() { std::swap(d_width, d_height); };

 private:
  inline void destroy()
  {
    if (d_pixeldata)
    {
      delete[] d_pixeldata;
      d_pixeldata = nullptr;
    }
    if (d_firstframe)
    {
      delete[] d_firstframe;
      d_firstframe = nullptr;
    }
  };
};

class MediaPreviewGenerator : public VlcBase
{
  VideoContext d_context;
 public:
  MediaPreviewGenerator(unsigned char *data, uint64_t size);
  inline MediaPreviewGenerator(char *data, uint64_t size);
  VideoContext *getRgb();
 private:
  inline static unsigned callback_vlc_format(void **opaque, char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines);
  inline static void *callback_vlc_lock(void *opaque, void **planes);
  inline static void callback_vlc_unlock(void *opaque, void *picture, void *const *planes);
  inline static void callback_vlc_display(void *data, void *id);
  void rotate90(unsigned char *data, uint64_t width, uint64_t height);
  void preparse();
};

inline MediaPreviewGenerator::MediaPreviewGenerator(char *data, uint64_t size)
  :
  MediaPreviewGenerator(reinterpret_cast<unsigned char *>(data), size)
{}

unsigned MediaPreviewGenerator::callback_vlc_format(void **opaque, char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines)
{
  memcpy(chroma, "RV24", 4);

  VideoContext *videocontext = reinterpret_cast<VideoContext *>(*opaque);

  *pitches = *width * 3;
  *lines = *height;

  videocontext->resetData(*width * *height * 3);
  videocontext->setSize(*width, *height);

  return 1;
}

void *MediaPreviewGenerator::callback_vlc_lock(void *opaque, void **planes)
{
  VideoContext *videocontext = reinterpret_cast<VideoContext *>(opaque);
  *planes = (videocontext->currentFrame() > 0) ? videocontext->pixelData() : videocontext->firstFrame();
  return nullptr;
}

void MediaPreviewGenerator::callback_vlc_unlock(void *opaque, void *, void *const *)
{
  VideoContext *videocontext = reinterpret_cast<VideoContext *>(opaque);
  videocontext->upFrame();
}

void MediaPreviewGenerator::callback_vlc_display(void *, void *)
{}

#endif

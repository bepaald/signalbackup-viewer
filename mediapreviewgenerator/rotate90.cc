#include "mediapreviewgenerator.ih"

void MediaPreviewGenerator::rotate90(unsigned char *buffer, uint64_t width, uint64_t height)
{
  const unsigned int sizeBuffer = width * height * 3;
  unsigned char *tempBuffer = new unsigned char[sizeBuffer];

  for (uint64_t y = 0, destinationColumn = height - 1; y < height; ++y, --destinationColumn)
  {
    int offset = y * width;

    for (uint64_t x = 0; x < width; ++x)
      for (int i = 0; i < 3; ++i)
        tempBuffer[(x * height + destinationColumn) * 3 + i] = buffer[(offset + x) * 3 + i];
  }

  // Copy rotated pixels

  std::memcpy(buffer, tempBuffer, sizeBuffer);
  delete[] tempBuffer;
}

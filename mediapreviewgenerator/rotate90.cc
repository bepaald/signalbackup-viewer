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

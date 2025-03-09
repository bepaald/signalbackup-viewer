/*
  Copyright (C) 2024-2025  Selwin van Dijk

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

#ifndef BASEATTACHMENTREADER_H_
#define BASEATTACHMENTREADER_H_

class FrameWithAttachment;

class BaseAttachmentReader
{
 public:
  BaseAttachmentReader() = default;
  BaseAttachmentReader(BaseAttachmentReader const &other) = default;
  BaseAttachmentReader(BaseAttachmentReader &&other) = default;
  BaseAttachmentReader &operator=(BaseAttachmentReader const &other) = default;
  BaseAttachmentReader &operator=(BaseAttachmentReader &&other) = default;
  virtual ~BaseAttachmentReader() = default;
  virtual BaseAttachmentReader *clone() const = 0;

  inline virtual int getAttachment(FrameWithAttachment *frame, bool verbose) = 0;
  //inline virtual void clearData() = 0;
};

template <typename T>
class AttachmentReader : public BaseAttachmentReader
{
 public:
  virtual BaseAttachmentReader *clone() const
  {
    return new T(static_cast<T const &>(*this));
  }
};

#endif

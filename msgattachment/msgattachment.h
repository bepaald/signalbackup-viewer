#ifndef MSGATTACHMENT_H_
#define MSGATTACHMENT_H_

#include <QMovie>
#include <QPixmap>

class MsgAttachment
{
 public:
  enum ATTACHMENTTYPE
  {
   GIF,
   IMAGE,
   VIDEO,
   AUDIO,
   BINARY,
   PLACEHOLDER_TYPE,
   INVALID_TYPE,
  };
 private:
  QVariant d_attachment;
  ATTACHMENTTYPE d_type;
  unsigned char *d_data; // dont own this?
  uint64_t d_size;
  QString d_filename;
 public:
  inline MsgAttachment();
  inline MsgAttachment(QVariant const &att, ATTACHMENTTYPE type, QString const &filename = QString(), unsigned char *data = nullptr, uint64_t size = 0);
  inline QVariant const &attachment() const;
  inline ATTACHMENTTYPE type() const;
  inline unsigned char *data() const;
  inline uint64_t dataSize() const;
  inline uint64_t width() const;
  inline uint64_t height() const;
  inline QString const &filename() const;
};

inline MsgAttachment::MsgAttachment()
  :
  d_attachment(QVariant()),
  d_type(INVALID_TYPE),
  d_data(nullptr),
  d_size(0)
{}

inline MsgAttachment::MsgAttachment(QVariant const &att, MsgAttachment::ATTACHMENTTYPE type, QString const &filename, unsigned char *data, uint64_t size)
  :
  d_attachment(att),
  d_type(type),
  d_data(data),
  d_size(size),
  d_filename(filename)
{}

inline QVariant const &MsgAttachment::attachment() const
{
  return d_attachment;
}

inline MsgAttachment::ATTACHMENTTYPE MsgAttachment::type() const
{
  return d_type;
}

inline unsigned char *MsgAttachment::data() const
{
  return d_data;
}

inline uint64_t MsgAttachment::dataSize() const
{
  return d_size;
}

inline uint64_t MsgAttachment::width() const
{
  if (d_type == VIDEO || d_type == IMAGE || d_type == BINARY)
    return d_attachment.value<QPixmap>().width();
  if (d_type == GIF)
    return d_attachment.value<QMovie *>()->currentPixmap().width();
  return 0;
}

inline uint64_t MsgAttachment::height() const
{
  if (d_type == VIDEO || d_type == IMAGE || d_type == BINARY)
    return d_attachment.value<QPixmap>().height();
  if (d_type == GIF)
    return d_attachment.value<QMovie *>()->currentPixmap().height();
  return 0;
}

inline QString const &MsgAttachment::filename() const
{
  return d_filename;
}

#endif

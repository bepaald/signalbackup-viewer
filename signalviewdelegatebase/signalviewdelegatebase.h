#ifndef SIGNALVIEWDELEGATEBASE_H_
#define SIGNALVIEWDELEGATEBASE_H_

#include <QPainter>

class SignalViewDelegateBase
{
 public:
  SignalViewDelegateBase() = default;
 protected:
  void paintAvatar(QPainter *painter, int x, int y, int size, QString const &recipient) const;
};

#endif

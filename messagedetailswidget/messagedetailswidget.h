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

#ifndef MESSAGEDETAILSWIDGET_H_
#define MESSAGEDETAILSWIDGET_H_

#include <QDockWidget>

class QGridLayout;

class MessageDetailsWidget : public QDockWidget
{
  Q_OBJECT;

  QGridLayout *d_layout;

 public:
  MessageDetailsWidget(QWidget *parent = nullptr);

 public slots:
  void setData(QVector<QString> const &fields, QVariantList const &values);
};

#endif

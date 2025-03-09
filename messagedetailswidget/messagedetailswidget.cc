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

#include "messagedetailswidget.ih"

MessageDetailsWidget::MessageDetailsWidget(QWidget *parent)
  :
  QDockWidget(parent),
  d_layout(new QGridLayout(this))
{
  setAllowedAreas(Qt::RightDockWidgetArea);
  setFeatures(QDockWidget::DockWidgetClosable);
  QWidget *mainwidget = new QWidget(this);
  mainwidget->setLayout(d_layout);
  setWidget(mainwidget);
}

void MessageDetailsWidget::setData(QVector<QString> const &fields, QVariantList const &values)
{
  // clear layout
  while (auto item = d_layout->takeAt(0))
    delete item;

  // set new data
  for (int i = 0; i < values.size(); ++i)
  {
    d_layout->addWidget(new QLabel(fields[i], this), i, 0);
    d_layout->addWidget(new QLabel(values[i].toString(), this), i, 1);
  }
}

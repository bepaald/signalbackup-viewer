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

#ifndef SEARCHWIDGET_H_
#define SEARCHWIDGET_H_

#include <QDockWidget>
#include <QRegularExpression>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;

class SearchWidget : public QDockWidget
{
  Q_OBJECT;

  QLineEdit *d_inputbox;
  QPushButton *d_prevbutton;
  QPushButton *d_nextbutton;
  QLabel *d_stats;
  QCheckBox *d_casesensitive;
  int d_total;
  int d_current;
 public:
  SearchWidget(QWidget *parent = nullptr);
 public slots:
  //void caseSensitivityChanged(int state);
  void textChanged(QString const &text);
  void enableSearchBox();
  void setTotal(int total);
  void setCurrent(int current);
  void clearAll();
 signals:
  void newSearch(QRegularExpression const &);
  void next();
  void prev();
};

#endif

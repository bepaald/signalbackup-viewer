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

#include "searchwidget.ih"

#include <QDebug>

SearchWidget::SearchWidget(QWidget *parent)
  :
  QDockWidget(parent),
  d_inputbox(new QLineEdit(this)),
  d_prevbutton(new QPushButton("Previous", this)),
  d_nextbutton(new QPushButton("Next", this)),
  d_stats(new QLabel(this)),
  d_casesensitive(new QCheckBox("Case sensitive", this)),
  d_total(0),
  d_current(0)
{
  // this effectively removes the (empty, but space consuming) title bar
  setTitleBarWidget(new QWidget(this));
  titleBarWidget()->hide();

  setAllowedAreas(Qt::BottomDockWidgetArea);
  setFeatures(QDockWidget::NoDockWidgetFeatures);

  QWidget *searchdockwidget = new QWidget(this);
  QVBoxLayout *vlayout = new QVBoxLayout();
  QHBoxLayout *hlayout = new QHBoxLayout();
  QHBoxLayout *hlayout_2 = new QHBoxLayout();
  vlayout->addLayout(hlayout);
  vlayout->addLayout(hlayout_2);
  searchdockwidget->setLayout(vlayout);

  d_inputbox->setEnabled(false);
  d_inputbox->setClearButtonEnabled(true);
  d_inputbox->setPlaceholderText("Select conversation to search");
  //connect textEdited(const QString &text)

  d_prevbutton->setEnabled(false);
  d_nextbutton->setEnabled(false);

  d_casesensitive->setCheckState(Qt::Unchecked);
  d_casesensitive->setEnabled(true);

  hlayout->addWidget(d_inputbox);
  hlayout->addWidget(d_prevbutton);
  hlayout->addWidget(d_nextbutton);

  hlayout_2->addWidget(d_casesensitive);
  hlayout_2->addWidget(d_stats);

  setWidget(searchdockwidget);

  connect(d_casesensitive, &QCheckBox::checkStateChanged, [=, this]{ textChanged(d_inputbox->text()); });

  connect(d_inputbox, &QLineEdit::textEdited, this, &SearchWidget::textChanged);
  connect(d_nextbutton, &QPushButton::clicked, [=, this]{ emit this->next(); } );
  connect(d_prevbutton, &QPushButton::clicked, [=, this]{ emit this->prev(); } );
}

void SearchWidget::enableSearchBox()
{
  d_inputbox->setPlaceholderText("Find...");
  d_inputbox->setEnabled(true);
}

void SearchWidget::textChanged(QString const &text) // slot
{
  QRegularExpression regex(text, d_casesensitive->checkState() == Qt::Unchecked ? QRegularExpression::CaseInsensitiveOption : QRegularExpression::NoPatternOption);
  emit newSearch(regex);
}

void SearchWidget::setTotal(int total)
{
  //qDebug() << "Setting total: " << total;
  d_total = total;
  if (/*[[likely]]*/ d_total > 0)
  {
    d_nextbutton->setEnabled(d_current < d_total - 1);
    d_prevbutton->setEnabled(d_current > 0);
    d_stats->setText("Result " + QString::number(d_current + 1) + " of " + QString::number(d_total));
  }
  else
  {
    d_nextbutton->setEnabled(false);
    d_prevbutton->setEnabled(false);
    d_stats->setText("");
  }
}

void SearchWidget::setCurrent(int current)
{
  //qDebug() << "Setting current: " << current;
  d_current = current;
  d_nextbutton->setEnabled(d_current < d_total - 1);
  d_prevbutton->setEnabled(d_current > 0);
  d_stats->setText("Result " + QString::number(d_current + 1) + " of " + QString::number(d_total));
}

void SearchWidget::clearAll()
{
  d_inputbox->clear();
  d_total = 0;
  d_current = 0;
}

// void SearchWidget::caseSensitivityChanged(int)
// {
//   textChanged(d_inputbox->text());
// }

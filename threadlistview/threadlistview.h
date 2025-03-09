/*
  Copyright (C) 2025  Selwin van Dijk

  This file is part of signalbackup-viewer0.12.

  signalbackup-viewer0.12 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  signalbackup-viewer0.12 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with signalbackup-viewer0.12.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef THREADLISTVIEW_H_
#define THREADLISTVIEW_H_

#include "../signallistviewbase/signallistviewbase.h"
#include "../threadsqlmodel/threadsqlmodel.h"

class ThreadListView : public SignalListViewBase
{
  ThreadSqlModel *d_model;

  template <typename T>
  struct fail : std::false_type
  {};

 public:
  explicit inline ThreadListView(QWidget *parent = nullptr);
  inline void setThreadModel(ThreadSqlModel *model);
 private:
  inline void indexSelected(QModelIndex const &idx);
};

inline ThreadListView::ThreadListView(QWidget *parent)
  :
  SignalListViewBase(parent),
  d_model(nullptr)
{
  connect(this, &ThreadListView::clicked, this, &ThreadListView::indexSelected);
}

inline void ThreadListView::setThreadModel(ThreadSqlModel *model)
{
  setModel(model);
  d_model = model;
}

inline void ThreadListView::indexSelected(QModelIndex const &idx)
{
  if (selectionModel()->isSelected(idx))
    return;
  selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
  repaint();
  d_model->setConversation(idx);
}

#endif

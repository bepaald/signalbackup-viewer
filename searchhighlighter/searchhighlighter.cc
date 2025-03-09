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

#include "searchhighlighter.ih"

void SearchHighlighter::highlightBlock(const QString &text)
{
  if (d_regex.pattern().isEmpty())
  {
    setFormat(0, 0, d_highlightformat);
    return;
  }
  QRegularExpressionMatchIterator i = d_regex.globalMatch(text);
  while (i.hasNext())
  {
    QRegularExpressionMatch match = i.next();
    setFormat(match.capturedStart(), match.capturedLength(), d_highlightformat);
  }
}

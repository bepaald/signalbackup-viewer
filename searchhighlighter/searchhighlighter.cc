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

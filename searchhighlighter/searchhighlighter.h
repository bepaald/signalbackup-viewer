#ifndef SEARCHHIGHLIGHTER_H_
#define SEARCHHIGHLIGHTER_H_

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class SearchHighlighter : public QSyntaxHighlighter
{
  QTextCharFormat d_highlightformat;
  QRegularExpression d_regex;
 public:
  inline SearchHighlighter(QTextDocument *doc, QRegularExpression const &regex);
  //inline SearchHighlighter(QTextDocument *doc, QString const &regex);
  //inline SearchHighlighter(QTextDocument *doc);
  void setRegex(QString const &regex);
 protected:
  void highlightBlock(const QString &text);
};

inline SearchHighlighter::SearchHighlighter(QTextDocument *doc, QRegularExpression const &regex)
  :
  QSyntaxHighlighter(doc),
  d_regex(regex)
{
  //d_highlightformat.setFontWeight(QFont::Bold);
  d_highlightformat.setBackground(Qt::yellow);
  d_highlightformat.setForeground(Qt::black);
  d_highlightformat.setFontUnderline(true);
}

// inline SearchHighlighter::SearchHighlighter(QTextDocument *doc, QString const &regex)
//   :
//   SearchHighlighter(doc, QRegularExpression{regex, QRegularExpression::CaseInsensitiveOption})
// {}

// inline SearchHighlighter::SearchHighlighter(QTextDocument *doc)
//   :
//   SearchHighlighter(doc, "")
// {}

inline void SearchHighlighter::setRegex(QString const &regex)
{
  d_regex = QRegularExpression(regex, QRegularExpression::CaseInsensitiveOption);
}

#endif

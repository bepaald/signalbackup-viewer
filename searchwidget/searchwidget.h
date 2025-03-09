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

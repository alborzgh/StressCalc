#ifndef INPUTSPREADSHEET_H
#define INPUTSPREADSHEET_H

#include<QTableWidget>

class inputSpreadSheet : public QTableWidget
{
  Q_OBJECT

public:
  inputSpreadSheet(QWidget *parent=0);
  inputSpreadSheet(int rows, int cols, QWidget *parent=0);
  ~inputSpreadSheet();

private:
 void  keyPressEvent(QKeyEvent *);
};

#endif // INPUTSPREADSHEET_H

#include<inputspreadsheet.h>
#include<QKeyEvent>
#include<QApplication>
#include<QClipboard>
#include<QMessageBox>


inputSpreadSheet::inputSpreadSheet(QWidget *parent)
  :QTableWidget(parent)
{

}

inputSpreadSheet::inputSpreadSheet(int rows, int cols, QWidget *parent)
  :QTableWidget(rows, cols, parent)
{

}

inputSpreadSheet::~inputSpreadSheet()
{

}

void
inputSpreadSheet::keyPressEvent(QKeyEvent * event)
{

  if(event->matches(QKeySequence::Copy) )
      {
      QString clipboardString;
      QModelIndexList selectedIndexes = this->selectionModel()->selectedIndexes();
      for (int i = 0; i < selectedIndexes.count(); ++i)
      {
          QModelIndex current = selectedIndexes[i];
          QString displayText = current.data(Qt::DisplayRole).toString();

          // If there exists another column beyond this one.
          if (i + 1 < selectedIndexes.count())
          {
              QModelIndex next = selectedIndexes[i+1];

              // If the column is on different row, the clipboard should take note.
              if (next.row() != current.row())
              {
                  displayText.append("\n");
              }
              else
              {
                  // Otherwise append a column separator.
                  displayText.append("\t ");
              }
          }
          clipboardString.append(displayText);
      }

      QApplication::clipboard()->setText(clipboardString);


    } else if(event->matches(QKeySequence::Paste) )  {
/*
              QString selected_text = qApp->clipboard()->text();
              QStringList cells = selected_text.split(QRegExp(QLatin1String("\\n|\\t")));
              while(!cells.empty() && cells.back().size() == 0)
              {
                  cells.pop_back(); // strip empty trailing tokens
              }
              int rows = selected_text.count(QLatin1Char('\n'));
              int cols = cells.size() / rows;
              if(cells.size() % rows != 0)
              {
                  // error, uneven number of columns, probably bad data
                  QMessageBox::critical(this, tr("Error"),
                                        tr("Invalid clipboard data, unable to perform paste operation."));
                  return;
              }

              if(cols != columnCount())
              {
                  // error, clipboard does not match current number of columns
                  QMessageBox::critical(this, tr("Error"),
                                        tr("Invalid clipboard data, incorrect number of columns."));
                  return;
              }

              // don't clear the grid, we want to keep any existing headers
              setRowCount(rows);
              // setColumnCount(cols);
              int cell = 0;
              for(int row=0; row < rows; ++row)
              {
                  for(int col=0; col < cols; ++col, ++cell)
                  {
                      QTableWidgetItem *newItem = new QTableWidgetItem(cells[cell]);
                      setItem(row, col, newItem);
                  }
              }

*/
  QMessageBox::critical(this, "You requested Paste", "Paste function not implemented yet!");
    }    else    {
        QTableWidget::keyPressEvent(event);
    }
}

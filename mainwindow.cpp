#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#endif

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  // create the central widget and assign it
    QWidget *centWidget = new QWidget(this);
    setCentralWidget(centWidget);

    // put the input and output page on a tabbed section
    tabbedPage = new QTabWidget;

    // create pages for the tabbed section
    QWidget *inputPage = new QWidget;
    QWidget *stressResultPage = new QWidget;
    QWidget *strengthResultPage = new QWidget;
    QWidget *stiffnessResultPage = new QWidget;

    // create layouts for widget alignment
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *inputLayout = new QVBoxLayout;
    QHBoxLayout *stressOutputLayout = new QHBoxLayout;
    QHBoxLayout *strengthOutputLayout = new QHBoxLayout;
    QHBoxLayout *stiffnessOutputLayout = new QHBoxLayout;

    sampleSoilLayers();     // create a set of sample soil layers to show in the table
    createMenu();                  // create the menu bar
    createTable();                   // create the spreadsheet table for data input
    setupContent();             // fill in the table
    createWidgets();           // create the additional widgets
    createConnection();         // connect plot signals to their corresponding slots

    // add widgets to corresponding layouts
    inputLayout->addWidget(table);
    inputLayout->addWidget(soilLayerBox);
    stressOutputLayout->addWidget(bigStressEditor,10);
    stressOutputLayout->addWidget(stressPlot,20);
    stressOutputLayout->addWidget(saveStressButton);
    strengthOutputLayout->addWidget(bigStrengthEditor,10);
    strengthOutputLayout->addWidget(strengthPlot,20);
    strengthOutputLayout->addWidget(saveStrengthButton);
    stiffnessOutputLayout->addWidget(bigStiffnessEditor,10);
    stiffnessOutputLayout->addWidget(stiffnessPlot,20);
    stiffnessOutputLayout->addWidget(saveStiffnessButton);
    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(tabbedPage);
    mainLayout->addWidget(analyzeButton);

    //mainLayout->addLayout(inputLayout);
    //mainLayout->addLayout(stressOutputLayout);

    // assign layouts to the tabbed pages and add them to the tabbed section
    inputPage->setLayout(inputLayout);
    stressResultPage->setLayout(stressOutputLayout);
    strengthResultPage->setLayout(strengthOutputLayout);
    stiffnessResultPage->setLayout(stiffnessOutputLayout);

    tabbedPage->addTab(inputPage, tr("Input"));
    tabbedPage->addTab(stressResultPage, tr("Stress Results"));
    tabbedPage->addTab(strengthResultPage, tr("Strength Results"));
    tabbedPage->addTab(stiffnessResultPage, tr("Stiffness Results"));

    // initialize the plots
    // plotStresses();
    // plotStrength();
    // plotStiffness();
    updateEverything();

    // set the main layout
    centWidget->setLayout(mainLayout);

    // set window title
    setWindowTitle(tr("Stress Calculation"));
}

MainWindow::~MainWindow()
{

}


void MainWindow::setupContent()
{
    // read soil layer information and fill in the table
    int numLayers = soilLayers.size();
    table->setRowCount(numLayers);
    for (int ii = 0; ii < numLayers; ii++)
      {
        table->setItem(ii , 0, new QTableWidgetItem(soilLayers[ii].getLayerName()));
        table->setItem(ii , 1, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerThickness())));
        table->setItem(ii , 2, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerUnitWeight())));
        table->setItem(ii , 3, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerFrictionAng())));
        table->setItem(ii , 4, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerCohesion())));
        table->setItem(ii , 5, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerStiffness())));
        table->setItem(ii , 6, new QTableWidgetItem());
        table->item(ii ,6)->setBackgroundColor(soilLayers[ii].getLayerColor());
      }

}

void MainWindow::createMenu()
{
  menuBar = new QMenuBar;
  fileMenu = new QMenu(tr("&File"), this);
  menuBar->addMenu(fileMenu);

  sampleAction = new QAction(tr("&Sample Soil Layers"), this);
  fileMenu->addAction(sampleAction);
  exitAction = new QAction(tr("E&xit"), this);
  fileMenu->addAction(exitAction);

  connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
  connect(sampleAction, SIGNAL(triggered()), this, SLOT(sampleSoilLayers()));
}

void MainWindow::createSoilLayerBox()
{
  // create the group box with buttons and widgets
  soilLayerBox = new QGroupBox(tr("Soil Layers"));

  QHBoxLayout *layout = new QHBoxLayout;

  QPushButton *addButton = new QPushButton(tr("Add Layer"));
  connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addLayer()));

  QPushButton *removeButton = new QPushButton(tr("Remove Layer"));
  connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(remLayer()));

  QPushButton *reDrawButton = new QPushButton(tr("Redraw Table"));
  connect(reDrawButton, SIGNAL(clicked(bool)),this, SLOT(reDrawTable()));

  QPushButton *clearButton = new QPushButton(tr("Clear Table"));
  connect(clearButton, SIGNAL(clicked(bool)),this, SLOT(clearTable()));

  layout->addWidget(addButton, 0, Qt::AlignLeft);
  layout->addWidget(removeButton, 0, Qt::AlignLeft);
  layout->addWidget(reDrawButton, 0, Qt::AlignRight);
  layout->addWidget(clearButton, 0, Qt::AlignRight);

  soilLayerBox->setLayout(layout);
}

void MainWindow::createTable()
{
  // create the input spreadsheet table
  int rows = 1;
  int cols = 7;
  table = new inputSpreadSheet(rows, cols, this);
  table->setSizeAdjustPolicy(QTableWidget::AdjustToContents);
  QStringList tableLabels;
  tableLabels << "Soil Layer" <<  "Thickness" << "Unit Weight" << "Friction Angle" << "Cohesion" << "Shear Stiffness" << "Color";
  table->setHorizontalHeaderLabels(tableLabels);

  table->setItemPrototype(table->item(rows - 1, cols - 1));
  table->setItemDelegate(new QItemDelegate());

  connect(table, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
          this, SLOT(updateInfo(QTableWidgetItem*)));
  connect(table, SIGNAL(itemChanged(QTableWidgetItem*)),
          this, SLOT(updateInfo(QTableWidgetItem*)));
}

void MainWindow::addLayer()
{
  // add a new soil layer below current layer
  int rowIndex = table->currentRow();
  soilLayer newLayer;
  if (rowIndex < 0)
    soilLayers.insert(soilLayers.begin(), newLayer);
   else
    soilLayers.insert(soilLayers.begin()+rowIndex+1, newLayer);
  table->insertRow(rowIndex+1);
  table->setItem(rowIndex+1 , 0, new QTableWidgetItem(newLayer.getLayerName()));
  table->setItem(rowIndex+1 , 1, new QTableWidgetItem(QString::number(newLayer.getLayerThickness())));
  table->setItem(rowIndex+1 , 2, new QTableWidgetItem(QString::number(newLayer.getLayerUnitWeight())));
  table->setItem(rowIndex+1 , 3, new QTableWidgetItem(QString::number(newLayer.getLayerFrictionAng())));
  table->setItem(rowIndex+1 , 4, new QTableWidgetItem(QString::number(newLayer.getLayerCohesion())));
  table->setItem(rowIndex+1 , 5, new QTableWidgetItem(QString::number(newLayer.getLayerStiffness())));
  table->setItem(rowIndex+1 , 6, new QTableWidgetItem());
  table->item(rowIndex+1 ,6)->setBackgroundColor(newLayer.getLayerColor());
}

void MainWindow::remLayer()
{
  // remove the current layer
  int rowIndex = table->currentRow();
  table->removeRow(rowIndex);
  soilLayers.erase(soilLayers.begin()+rowIndex);
}

void
MainWindow::sampleSoilLayers()
{
  // create sample soil layers
  soilLayers.clear();
  soilLayers.push_back(soilLayer("Sand",2,110,120,10000,32,0.0, QColor(100,0,0,100)));
  soilLayers.push_back(soilLayer("Silt",2,105,110,12000,16,500, QColor(0,100,0,100)));
  soilLayers.push_back(soilLayer("Clay 1",3,105,110,20000,0,2000, QColor(0,0,100,100)));
  soilLayers.push_back(soilLayer("Silty Sand",5,110,115,11000,36,0, QColor(255,0,0,100)));
  soilLayers.push_back(soilLayer("Clay 2",6,105,110,5000,0,1500, QColor(0,255,0,100)));
}

void MainWindow::reDrawTable()
{
  // update and redraw the table
    int numLayers = soilLayers.size();
    table->setRowCount(numLayers);
    for (int ii = 0; ii < numLayers; ii++)
      {
        table->setItem(ii , 0, new QTableWidgetItem(soilLayers[ii].getLayerName()));
        table->setItem(ii , 1, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerThickness())));
        table->setItem(ii , 2, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerUnitWeight())));
        table->setItem(ii , 3, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerFrictionAng())));
        table->setItem(ii , 4, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerCohesion())));
        table->setItem(ii , 5, new QTableWidgetItem(QString::number(soilLayers[ii].getLayerStiffness())));
        table->setItem(ii , 6, new QTableWidgetItem());
        table->item(ii ,6)->setBackgroundColor(soilLayers[ii].getLayerColor());
      }
}

void MainWindow::updateInfo(QTableWidgetItem * item)
{
 if (item && item == table->currentItem()) {
      if(item->column() == 0)
        soilLayers[item->row()].setLayerName(item->text());
      else if (item->column() == 1)
         soilLayers[item->row()].setLayerThickness(item->text().toDouble());
      else if (item->column() == 2)
         soilLayers[item->row()].setLayerUnitWeight(item->text().toDouble());
      else if (item->column() == 3)
         soilLayers[item->row()].setLayerFrictionAng(item->text().toDouble());
      else if (item->column() == 4)
         soilLayers[item->row()].setLayerCohesion(item->text().toDouble());
      else if (item->column() == 5)
         soilLayers[item->row()].setLayerStiffness(item->text().toDouble());
  }
  return;
}

void
MainWindow::createWidgets()
{
  createSoilLayerBox();

  analyzeButton = new QPushButton("Calculate");
  connect(analyzeButton, SIGNAL(clicked(bool)),this,SLOT(updateEverything()));

  bigStressEditor = new QTextEdit(this);

  stressPlot = new QCustomPlot(this);
  stressPlot->setBaseSize(100,200);

  saveStressButton = new QPushButton("Save Stress Plot");
  connect(saveStressButton, SIGNAL(clicked(bool)),this,SLOT(saveStressPlot()));

  bigStrengthEditor = new QTextEdit(this);

  strengthPlot = new QCustomPlot(this);
  strengthPlot->setBaseSize(100,200);

  saveStrengthButton = new QPushButton("Save Strength Plot");
  connect(saveStrengthButton, SIGNAL(clicked(bool)),this,SLOT(saveStrengthPlot()));

  bigStiffnessEditor = new QTextEdit(this);

  stiffnessPlot = new QCustomPlot(this);
  stiffnessPlot->setBaseSize(100,200);

  saveStiffnessButton = new QPushButton("Save Stiffness Plot");
  connect(saveStiffnessButton, SIGNAL(clicked(bool)),this,SLOT(saveStiffnessPlot()));
}

void MainWindow::clearTable()
{
  // clear the table and soil layers
  soilLayers.clear();
  table->clearContents();
  reDrawTable();
}

void MainWindow::updateEverything()
{
  //update and draw stresses
  bigStressEditor->clear();
  bigStrengthEditor->clear();
  bigStiffnessEditor->clear();
  int numLayers = soilLayers.size();

  if (numLayers == 0)
    return;

  soilLayers[0].setLayerTopStress(0);
  bigStressEditor->insertPlainText("Layer "+QString::number(1)+": Stress at bottom = " + QString::number(soilLayers[0].getLayerBottomStress())+"\n" );
  bigStrengthEditor->insertPlainText("Layer "+QString::number(1)+": Strength at bottom = " + QString::number(soilLayers[0].getLayerBottomStrength())+"\n" );
  bigStiffnessEditor->insertPlainText("Layer "+QString::number(1)+": Stiffness at bottom = " + QString::number(soilLayers[0].getLayerStiffness())+"\n" );
  for (int ii = 1; ii < numLayers; ii++)
    {
      soilLayers[ii].setLayerTopStress(soilLayers[ii-1].getLayerBottomStress());
      bigStressEditor->insertPlainText("Layer "+QString::number(ii+1)+": Stress at bottom = " + QString::number(soilLayers[ii].getLayerBottomStress())+"\n" );
      bigStrengthEditor->insertPlainText("Layer "+QString::number(ii+1)+": Strength at bottom = " + QString::number(soilLayers[ii].getLayerBottomStrength())+"\n" );
      bigStiffnessEditor->insertPlainText("Layer "+QString::number(ii+1)+": Stiffness at bottom = " + QString::number(soilLayers[ii].getLayerStiffness())+"\n" );
    }
  plotStresses();
  plotStrength();
  plotStiffness();
}

void MainWindow::plotLayers(QCustomPlot * plot)
{
  // draw soil layers based on their colors
  int numLayers = soilLayers.size();
  double height = 0;
  QCPItemRect  *layerRect;
  for (int ii = 0; ii < numLayers; ii++)
    {
        layerRect = new QCPItemRect(plot);
        layerRect->topLeft->setTypeX(QCPItemPosition::ptAxisRectRatio);
        layerRect->topLeft->setTypeY(QCPItemPosition::ptPlotCoords );
        layerRect->topLeft->setCoords(0,height);

        height += soilLayers[ii].getLayerThickness();

        layerRect->bottomRight->setTypeX(QCPItemPosition::ptAxisRectRatio);
        layerRect->bottomRight->setTypeY(QCPItemPosition::ptPlotCoords );
        layerRect->bottomRight->setCoords(1, height);

        QPen *layerPen = new QPen;
        layerPen->setColor(soilLayers[ii].getLayerColor());
        layerRect->setPen(* layerPen);

        QBrush *layerBrush = new QBrush;
        QColor layerColor = soilLayers[ii].getLayerColor();
        layerColor.setAlphaF(layerColor.alphaF()/2.0);
        layerBrush->setColor(layerColor);
        layerBrush->setStyle(Qt::SolidPattern);
        layerRect->setBrush(*layerBrush);

        layerRect->setVisible(true);

        plot->addItem(layerRect);
    }
}

void MainWindow::plotStresses()
{
  stressPlot->clearPlottables();
  stressPlot->clearItems();
  plotLayers(stressPlot);
  int numLayers = soilLayers.size();
  QVector<double> x(numLayers+1), y(numLayers+1);
  x[0] = 0; y[0] = 0;

  for(int ii = 1; ii < numLayers+1; ii++){
      x[ii] = soilLayers[ii-1].getLayerBottomStress();
      y[ii] = y[ii-1] + soilLayers[ii-1].getLayerThickness();
    }

  QPen *stressCurvePen = new QPen;
  stressCurvePen->setColor(Qt::blue);
  stressCurvePen->setWidth(2);

  QCPCurve *stressCurve = new QCPCurve(stressPlot->xAxis, stressPlot->yAxis);
  stressCurve->setData(x,y);
  stressCurve->setPen(*stressCurvePen);
  stressPlot->addPlottable(stressCurve);
  stressPlot->xAxis->setLabel("Vertical Stress");
  stressPlot->yAxis->setLabel("Depth");
  stressPlot->xAxis->setRange(0,1.1*y.last());
  stressPlot->yAxis->setRange(0, 1.1*x.last());
  stressPlot->yAxis->setRangeReversed(true);
  stressPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
  stressPlot->axisRect()->setupFullAxesBox();
  stressPlot->rescaleAxes();
  stressPlot->replot();
}

void MainWindow::plotStrength()
{
  strengthPlot->clearPlottables();
  strengthPlot->clearItems();
  plotLayers(strengthPlot);
  int numLayers = soilLayers.size();
  QVector<double> x(2*numLayers), y(2*numLayers);

  if (numLayers > 0)
  {
    x[0] = soilLayers[0].getLayerTopStrength();
    x[1] = soilLayers[0].getLayerBottomStrength();
    y[0] = 0;
    y[1] = y[0] + soilLayers[0].getLayerThickness();
  } else {
      return;
    }

  for(int ii = 1; ii < numLayers; ii++){
      x[2*ii] = soilLayers[ii].getLayerTopStrength();
      x[2*ii+1] = soilLayers[ii].getLayerBottomStrength();
      y[2*ii] = y[2*ii-1];
      y[2*ii+1] = y[2*ii] + soilLayers[ii].getLayerThickness();
    }

  QPen *strengthCurvePen = new QPen;
  strengthCurvePen->setColor(Qt::blue);
  strengthCurvePen->setWidth(2);

  QCPCurve *strengthCurve = new QCPCurve(strengthPlot->xAxis, strengthPlot->yAxis);
  strengthCurve->setData(x,y);
  strengthCurve->setPen(*strengthCurvePen);
  strengthPlot->addPlottable(strengthCurve);
  strengthPlot->xAxis->setLabel("Strength");
  strengthPlot->yAxis->setLabel("Depth");
  strengthPlot->xAxis->setRange(0,1.1*y.last());
  strengthPlot->yAxis->setRange(0, 1.1*x.last());
  strengthPlot->yAxis->setRangeReversed(true);
  strengthPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
  strengthPlot->axisRect()->setupFullAxesBox();
  strengthPlot->rescaleAxes();
  strengthPlot->replot();
}

void MainWindow::plotStiffness()
{
  stiffnessPlot->clearPlottables();
  stiffnessPlot->clearItems();
  plotLayers(stiffnessPlot);
  int numLayers = soilLayers.size();
  QVector<double> x(2*numLayers), y(2*numLayers);
  x[0] = 0; y[0] = 0;

  if (numLayers > 0)
  {
    x[0] = x[1] = soilLayers[0].getLayerStiffness();
    y[1] = y[0] + soilLayers[0].getLayerThickness();
  }

  for(int ii = 1; ii < numLayers; ii++){
      x[2*ii] = x[2*ii+1] = soilLayers[ii].getLayerStiffness();
      y[2*ii] = y[2*ii-1];
      y[2*ii+1] = y[2*ii] + soilLayers[ii].getLayerThickness();
    }

  QPen *stiffnessCurvePen = new QPen;
  stiffnessCurvePen->setColor(Qt::blue);
  stiffnessCurvePen->setWidth(2);

  QCPCurve *stiffnessCurve = new QCPCurve(stiffnessPlot->xAxis, stiffnessPlot->yAxis);
  stiffnessCurve->setData(x,y);
  stiffnessCurve->setPen(*stiffnessCurvePen);
  stiffnessPlot->addPlottable(stiffnessCurve);
  stiffnessPlot->xAxis->setLabel("Stiffness");
  stiffnessPlot->yAxis->setLabel("Depth");
  stiffnessPlot->xAxis->setRange(0,1.1*y.last());
  stiffnessPlot->yAxis->setRange(0, 1.1*x.last());
  stiffnessPlot->yAxis->setRangeReversed(true);
  stiffnessPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
  stiffnessPlot->axisRect()->setupFullAxesBox();
  stiffnessPlot->rescaleAxes();
  stiffnessPlot->replot();
}

void MainWindow::saveStressPlot()
{
  stressPlot->savePdf("Stress.pdf");
}

void MainWindow::saveStrengthPlot()
{
  strengthPlot->savePdf("Strength.pdf");
}

void MainWindow::saveStiffnessPlot()
{
  stiffnessPlot->savePdf("Stiffness.pdf");
}

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  QCustomPlot * sender = (QCustomPlot *) QObject::sender();
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      sender->replot();
    }
  }
}

void MainWindow::selectionChanged()
{
  QCustomPlot * sender = (QCustomPlot *) QObject::sender();
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.
   
   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.
   
   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */
  
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (sender->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || sender->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      sender->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || sender->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    sender->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    sender->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (sender->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || sender->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      sender->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || sender->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    sender->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    sender->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
}

void MainWindow::mousePress()
{
  QCustomPlot * sender = (QCustomPlot *) QObject::sender();
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
  
  if (sender->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    sender->axisRect()->setRangeDrag(sender->xAxis->orientation());
  else if (sender->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    sender->axisRect()->setRangeDrag(sender->yAxis->orientation());
  else
    sender->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
  QCustomPlot * sender = (QCustomPlot *) QObject::sender();
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (sender->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    sender->axisRect()->setRangeZoom(sender->xAxis->orientation());
  else if (sender->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    sender->axisRect()->setRangeZoom(sender->yAxis->orientation());
  else
    sender->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::createConnection()
{

  // connect slot that ties some axis selections together (especially opposite axes):
  connect(stressPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
  connect(stressPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(stressPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

  // make bottom and left axes transfer their ranges to top and right axes:
  connect(stressPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), stressPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(stressPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), stressPlot->yAxis2, SLOT(setRange(QCPRange)));

  // connect some interaction slots:
  connect(stressPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));




  // connect slot that ties some axis selections together (especially opposite axes):
  connect(strengthPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
  connect(strengthPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(strengthPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

  // make bottom and left axes transfer their ranges to top and right axes:
  connect(strengthPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), strengthPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(strengthPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), strengthPlot->yAxis2, SLOT(setRange(QCPRange)));

  // connect some interaction slots:
  connect(strengthPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));



  // connect slot that ties some axis selections together (especially opposite axes):
  connect(stiffnessPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
  connect(stiffnessPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
  connect(stiffnessPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

  // make bottom and left axes transfer their ranges to top and right axes:
  connect(stiffnessPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), stiffnessPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(stiffnessPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), stiffnessPlot->yAxis2, SLOT(setRange(QCPRange)));

  // connect some interaction slots:
  connect(stiffnessPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
}

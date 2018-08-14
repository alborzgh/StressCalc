#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<soilmat.h>
#include<inputspreadsheet.h>
#include<vector>
#include<qcustomplot.h>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QLineEdit;
class QToolBar;
class QTableWidgetItem;
class QTableWidget;
class QGroupBox;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void addLayer();
    void remLayer();
    void sampleSoilLayers();
    void reDrawTable();
    void clearTable();
    void updateEverything();
    void updateInfo(QTableWidgetItem*);
    void saveStressPlot();
    void saveStrengthPlot();
    void saveStiffnessPlot();

    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void selectionChanged();
    void mousePress();
    void mouseWheel();

private:
    void createMenu();
    void setupContent();
    void createSoilLayerBox();
    void createWidgets();
    void createTable();
    void createConnection();
    void plotLayers(QCustomPlot*);
    void plotStresses();
    void plotStrength();
    void plotStiffness();

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *sampleAction;
    QAction *exitAction;
    QTabWidget *tabbedPage;

    QGroupBox *soilLayerBox;

    inputSpreadSheet *table;
    QLabel *cellLabel;

    QPushButton *analyzeButton;

    QTextEdit *bigStressEditor;
    QPushButton *saveStressButton;
    QCustomPlot *stressPlot;

    QTextEdit *bigStrengthEditor;
    QPushButton *saveStrengthButton;
    QCustomPlot *strengthPlot;

    QTextEdit *bigStiffnessEditor;
    QPushButton *saveStiffnessButton;
    QCustomPlot *stiffnessPlot;

    std::vector<soilLayer> soilLayers;

};

#endif // MAINWINDOW_H

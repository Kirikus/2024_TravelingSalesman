#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "QStandardPaths"
#include "manager.h"
#include "plot.h"
#include "qcustomplot.h"
#include "strategyIO.h"
#include "table_models/delegates/color_delegate.h"
#include "table_models/delegates/combobox_delegate.h"
#include "table_models/measurements_table.h"
#include "table_models/naming_table.h"
#include "table_models/plot_settings_table.h"
#include "table_models/errors_table.h"
#include "variable.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  lib::Variable a{{1, 2, 3, 4, 5}, "izmerenie_1", "x"};
  lib::Variable b{{4, 2, 11, 3, 5, 1}, "izmerenie_2"};
  lib::Variable c{{4, 2, 11}, "izmerenie_3", "z"};
  lib::Manager::getInstance()->addVariable(c);
  lib::Manager::getInstance()->addVariable(a);
  lib::Manager::getInstance()->addVariable(b);

  ui->tableViewMain->setModel(new lib::MeasurementsTable);
  ui->tableViewNaming->setModel(new lib::NamingTable);
  ui->tableViewPlotsSets->setModel(new lib::PlotSettingsTable);
  ui->tableViewErrors->setModel(new lib::ErrorsTable);

  ui->tableViewPlotsSets->setItemDelegateForColumn(4, new ColorDelegate);
  ui->tableViewPlotsSets->setItemDelegateForColumn(
      2, new ComboBoxDelegate(lib::VisualOptions::point_forms.values()));
  ui->tableViewPlotsSets->setItemDelegateForColumn(
      3, new ComboBoxDelegate(lib::VisualOptions::line_types.values()));

  ui->tableViewErrors->setItemDelegateForColumn(
      0, new ComboBoxDelegate(lib::ErrorOptions::error_types.values()));

  ui->tableViewMain->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui->tableViewNaming->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui->tableViewPlotsSets->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  // ui->tableViewErrors->horizontalHeader()->setSectionResizeMode(
  //     QHeaderView::ResizeToContents);

  ui->tableViewMain->show();
  ui->tableViewNaming->show();
  ui->tableViewPlotsSets->show();
  ui->tableViewErrors->show();

  connect(ui->addPlotBtn, SIGNAL(clicked()), this, SLOT(addPlot()));
  connect(ui->deletePlotBtn, SIGNAL(clicked()), this, SLOT(deletePlot()));
  connect(ui->addRowBtn, SIGNAL(clicked()), this, SLOT(addRow()));
  connect(ui->addColumnBtn, SIGNAL(clicked()), this, SLOT(addColumn()));
  connect(ui->deleteRowBtn, SIGNAL(clicked()), this, SLOT(removeRow()));
  // connect(ui->deleteColumnBtn, SIGNAL(clicked()), this,
  // SLOT(removeColumn()));
  connect(ui->LoadDataBtn, SIGNAL(clicked()), this, SLOT(load()));

  connect(lib::Manager::getInstance(),SIGNAL(Variable_is_deleted()),this,SLOT(removeColumn()));
  connect(ui->deleteColumnBtn,SIGNAL(clicked()),this,SLOT(removeVariable()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::load() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Select a file"),
      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
      tr("Open CSV (*.csv);;Open JSON (*.json);;"));
  if (file_name.isEmpty()) return;
  if (file_name.endsWith(".csv")) {
    StrategyIO* loader = new StrategyIO_CSV;
    loader->load(file_name);
    delete loader;
  }
  if (file_name.endsWith(".json")) {
    StrategyIO* loader = new StrategyIO_JSON;
    loader->load(file_name);
    delete loader;
  }
  if (file_name.endsWith(".db")) {
    StrategyIO* loader = new StrategyIO_DB;
    loader->load(file_name);
    delete loader;
  }
}

void MainWindow::save() {}

void MainWindow::deletePlot() {
  int index = ui->tabWidgetPlots->currentIndex();
  ui->tabWidgetPlots->removeTab(index);
}

void MainWindow::addPlot() {
  int count = ui->tabWidgetPlots->count();
  ui->tabWidgetPlots->addTab(new QCustomPlot,
                             "Plot " + QString::number(count + 1));
}

void MainWindow::addRow() {
  size_t count = lib::Manager::getInstance()->getMeasurementsCount();
  for (int i = 0; i < lib::Manager::getInstance()->getVariablesCount(); i++)
    if (count ==
        lib::Manager::getInstance()->getVariable(i).getMeasurementsCount())
      lib::Manager::getInstance()->getVariable(i).measurements.push_back(0);
  ui->tableViewMain->model()->insertRows(
      lib::Manager::getInstance()->getMeasurementsCount(), 1);
}

void MainWindow::removeRow() {
  size_t count = lib::Manager::getInstance()->getMeasurementsCount();
  if (count == 1) return;
  for (int i = 0; i < lib::Manager::getInstance()->getVariablesCount(); i++)
    if (count ==
        lib::Manager::getInstance()->getVariable(i).getMeasurementsCount())
      lib::Manager::getInstance()->getVariable(i).measurements.pop_back();
  ui->tableViewMain->model()->removeRows(
      lib::Manager::getInstance()->getMeasurementsCount(), 1);
}

void MainWindow::addColumn() {
  lib::Manager::getInstance()->addVariable(lib::Variable());
  ui->tableViewMain->model()->insertColumns(
      lib::Manager::getInstance()->getVariablesCount(), 1);
  ui->tableViewNaming->model()->insertRows(
      lib::Manager::getInstance()->getVariablesCount(), 1);
  ui->tableViewErrors->model()->insertRows(
      lib::Manager::getInstance()->getVariablesCount(), 1);
}

void MainWindow::removeColumn() {
  ui->tableViewMain->model()->removeColumns(
      lib::Manager::getInstance()->getVariablesCount(), 1);
  ui->tableViewNaming->model()->removeRows(
      lib::Manager::getInstance()->getVariablesCount(), 1);
  ui->tableViewPlotsSets->model()->removeRows(
      lib::Manager::getInstance()->getVariablesCount(), 1);
}

void MainWindow::removeVariable()
{
    lib::Manager::getInstance()->deleteVariable();
}

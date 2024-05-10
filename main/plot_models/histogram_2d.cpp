#include "histogram_2d.h"

#include <cmath>

#include "manager/manager.h"

class CustomTicker : public QCPAxisTicker {
 public:
  CustomTicker(double granularity_0) : granularity_(granularity_0 / 10) {}

  // selecting division values
  QString getTickLabel(double tick, const QLocale& locale, QChar formatChar,
                       int precision) override {
    return QCPAxisTicker::getTickLabel(tick / granularity_, locale, formatChar,
                                       precision);
  }

  // the interval between divisions
  double getTickStep(const QCPRange& range) override {
    return QCPAxisTicker::getTickStep(range) * granularity_;
  }

  // the number of divisions that display the values
  QVector<double> createTickVector(double tickStep, const QCPRange &range) override {
    return QCPAxisTicker::createTickVector(tickStep / granularity_, range);
  }
 private:
  double granularity_ = 1;
};

void Histogram2D::Draw() {
  // The color scheme automatically adjusts to the set of values

  clearPlottables();

  xAxis->setLabel("");
  yAxis->setLabel("");

  if (lib::Manager::GetInstance()->GetVariablesCount() <= std::max(x_, y_))
    return;

  const lib::Variable& variable_x =
      lib::Manager::GetInstance()->GetVariable(x_);

  const lib::Variable& variable_y =
      lib::Manager::GetInstance()->GetVariable(y_);

  if (variable_x.GetMeasurementsCount() == 0 ||
      variable_y.GetMeasurementsCount() == 0)
    return;

  double size_box =
      std::max(variable_x.measurements[0], variable_y.measurements[0]);

  for (int i = 0; i < variable_y.GetMeasurementsCount(); ++i) {
    size_box =
        std::max(size_box, 2 * std::max(abs(variable_x.measurements[i]),
                                        abs(variable_y.measurements[i])) +
                               20);
  }

  size_box *= granularity_ / 10;

  QVector<QVector<double>> density(int(size_box) + 2,
                                   QVector<double>(int(size_box) + 2, 0));

  for (int i = 0; i < variable_x.GetMeasurementsCount(); ++i) {
    density[std::round(variable_x.measurements[i] * (granularity_ / 10) +
                       size_box / 2)]
           [std::round(variable_y.measurements[i] * (granularity_ / 10) +
                       size_box / 2)]++;
  }

  QSharedPointer<CustomTicker> customTicker(new CustomTicker(granularity_));
  xAxis->setTicker(customTicker);
  yAxis->setTicker(customTicker);

  QCPColorMap* colorMap = new QCPColorMap(xAxis, yAxis);

  colorMap->data()->setSize(size_box, size_box);
  colorMap->data()->setRange(QCPRange(-size_box / 2, size_box / 2),
                             QCPRange(-size_box / 2, size_box / 2));

  for (int i = 0; i < size_box; ++i) {
    for (int j = 0; j < size_box; ++j) {
      colorMap->data()->setCell(i, j, density[i][j]);
    }
  }

  QCPColorGradient gradient;
  gradient.clearColorStops();
  gradient.setColorStopAt(0, QColor(255, 255, 0));
  gradient.setColorStopAt(0.5, QColor(255, 0, 0));
  gradient.setColorStopAt(1, QColor(128, 0, 128));
  colorMap->setGradient(gradient);

  colorMap->setInterpolate(true);
  colorMap->rescaleDataRange(true);

  moveLayer(layer("grid"), layer("main"), QCustomPlot::limAbove);

  xAxis->setLabel("Axis " + variable_x.naming.title);
  yAxis->setLabel("Axis " + variable_y.naming.title);
  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  replot();
}

void Histogram2D::Options() {
  OptionsHistogram2D a(index_x_, index_y_, index_granularity_);
  a.exec();

  x_ = a.choose_AxisX();
  y_ = a.choose_AxisY();
  granularity_ = a.choose_granularity();

  index_x_ = a.get()->axisXComboBox->currentIndex();
  index_y_ = a.get()->axisYComboBox->currentIndex();
  index_granularity_ = a.get()->granularityComboBox->currentIndex();

  Draw();
}

OptionsHistogram2D::OptionsHistogram2D(int index_x_, int index_y_,
                                       int index_granularity_)
    : ui(new Ui::DialogHistogram2D) {
  ui->setupUi(this);

  setWindowTitle("Histogram2D options");

  for (int i = 0; i < lib::Manager::GetInstance()->GetVariablesCount(); ++i) {
    ui->axisXComboBox->addItem(
        lib::Manager::GetInstance()->GetVariable(i).naming.title);
  }
  ui->axisXComboBox->setCurrentIndex(index_x_);

  for (int i = 0; i < lib::Manager::GetInstance()->GetVariablesCount(); ++i) {
    ui->axisYComboBox->addItem(
        lib::Manager::GetInstance()->GetVariable(i).naming.title);
  }
  ui->axisYComboBox->setCurrentIndex(index_y_);

  ui->granularityComboBox->addItem("10");
  ui->granularityComboBox->addItem("50");
  ui->granularityComboBox->addItem("100");
  ui->granularityComboBox->addItem("200");
  ui->granularityComboBox->setCurrentIndex(index_granularity_);

  connect(ui->confirmPushButton, &QPushButton::clicked, this, &QDialog::close);
}

double OptionsHistogram2D::choose_granularity() {
  switch (ui->granularityComboBox->currentIndex()) {
    case 0:
      return 10;
    case 1:
      return 50;
    case 2:
      return 100;
    case 3:
      return 200;
  }
}

int OptionsHistogram2D::choose_AxisX() {
  return ui->axisXComboBox->currentIndex();
}

int OptionsHistogram2D::choose_AxisY() {
  return ui->axisYComboBox->currentIndex();
}

OptionsHistogram2D::~OptionsHistogram2D() { delete ui; }

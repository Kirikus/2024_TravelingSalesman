#include "plot.h"

#include <random>
#include <chrono>
#include "manager.h"

void LinePlot::customization(auto& visual, auto& graph) {
    // visible
    graph->setVisible(visual.visible);

    // width
    QPen graphPen = graph->pen();
    graphPen.setWidth(visual.width);

    // type line
    switch(visual.line_type) {
    case 1:
        graphPen.setStyle(Qt::SolidLine);
        break;
    case 2:
        graphPen.setStyle(Qt::DashLine);
        break;
    case 3:
        graphPen.setStyle(Qt::DotLine);
        break;
    case 4:
        graphPen.setStyle(Qt::DashDotLine);
        break;
    case 5:
        graphPen.setStyle(Qt::DashDotDotLine);
        break;
    case 6:
        auto now = std::chrono::system_clock::now();
        auto seed = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        std::mt19937 gen(seed);
        std::uniform_int_distribution<> dis(1, 10);

        QVector<qreal> dashes;
        for (int i = 0; i < 10; ++i) {
            qreal length = static_cast<qreal>(dis(gen));
            dashes << length;
        }
        graphPen.setDashPattern(dashes);
    }


    graph->setPen(graphPen);
}

void LinePlot::draw(QCustomPlot* plot) {
  plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

  for (int graphIndex = 0; graphIndex < plot->graphCount(); ++graphIndex) {
    plot->graph(graphIndex)->data()->clear();
  }

  for (int i = 0; i < lib::Manager::getInstance()->getVariablesCount(); ++i) {
    auto graph = plot->addGraph(plot->xAxis, plot->yAxis);

    if (i <= 7)
      graph->setPen(QColor(colors[i][0], colors[i][1], colors[i][2]));
    else {
      std::mt19937 gen(i);
      std::uniform_int_distribution<> dis(0, 204);
      graph->setPen(QColor(dis(gen), dis(gen), dis(gen)));
    }

    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 9));

    QVector<double> xs, ys;
    auto var = lib::Manager::getInstance()->getVariable(i).measurements;
    for (int j = 0; j < var.size(); ++j) {
      ys.push_back(var[j]);
      xs.push_back(j + 1);
    }
    graph->setData(xs, ys);

    auto& visual = lib::Manager::getInstance()->getVariable(i).variable_visual;
    customization(lib::Manager::getInstance()->getVariable(i).variable_visual, graph);
  }

  plot->replot();
}

void LinePlot::setOptions() {}

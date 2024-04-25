#ifndef SCTATTER_PLOT_2D_H
#define SCTATTER_PLOT_2D_H

#include "abstractplotmodel.h"

class ScatterPlot2D : public AbstractPlotModel {
    Q_OBJECT
public:
    ScatterPlot2D(QString x_label, QString y_label, QString title = "",
                QWidget* parent = nullptr)
        : AbstractPlotModel(parent),
        x_label(x_label),
        y_label(y_label),
        title(title) {}

    void Draw(QCustomPlot*);

private:
    QString x_label;
    QString y_label;
    QString title;
};

#endif // SCTATTER_PLOT_2D_H

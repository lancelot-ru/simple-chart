#pragma once

#include <QtCharts>
using namespace QtCharts;

class BarView : public QWidget
{
    Q_OBJECT
public:
    explicit BarView(QWidget *parent = nullptr);
    ~BarView();

    void setModel(QStandardItemModel *model);
    void refresh();

public slots:
    void update(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &);

private:
    void rebuiltChart();
    int maxValue(QList<QBarSet *> list);

    QStandardItemModel *_model;
    QChartView *_view;
    QChart *_chart;
    QBarSeries *_series;
};

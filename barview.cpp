#include "barview.h"

BarView::BarView(QWidget *parent)
    : QWidget(parent)
{
    _view = new QChartView(this);
    _view->setRenderHint(QPainter::Antialiasing);

    auto layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_view);

    this->setLayout(layout);
}

BarView::~BarView()
{
    delete _series;
    delete _chart;
    delete _view;
}

void BarView::setModel(QAbstractItemModel *model)
{
    _model = model;

    _chart = new QChart();

    _series = new QBarSeries(_chart);

    rebuiltChart();
}

void BarView::update(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &)
{
    Q_UNUSED(bottomRight)

    auto list = _series->barSets();
    if (list.count() == 0)
        return;
    auto set = list[topLeft.row()];

    if (topLeft.column() == 0)
    {
        set->setLabel(_model->data(topLeft, Qt::DisplayRole).toString());
    }
    else
    {
        int number = _model->data(topLeft, Qt::DisplayRole).toInt();
        set->replace(0, number);
    }

    _series->insert(topLeft.row(), set);

    int max = maxValue(list);

    _chart->axes(Qt::Vertical).first()->setRange(0, max);
}

void BarView::onNewFileOpened()
{
    _series->clear();
    _chart->removeSeries(_series);
    rebuiltChart();
}

void BarView::rebuiltChart()
{
    int max = 0;
    for (int row = 0; row < _model->rowCount(QModelIndex()); ++row)
    {
        QString str = _model->data(_model->index(row, 0, QModelIndex()), Qt::DisplayRole).toString();
        auto set = new QBarSet(str);
        for (int column = 1; column < _model->columnCount(QModelIndex()); ++column)
        {
            int number = _model->data(_model->index(row, column, QModelIndex()), Qt::DisplayRole).toInt();
            *set << number;

            if (number > max)
                max = number;
        }
        _series->append(set);
    }

    _chart->addSeries(_series);
    _chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    for (int column = 1; column < _model->columnCount(QModelIndex()); ++column)
    {
        categories << _model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
    }
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    _chart->createDefaultAxes();
    _chart->setAxisX(axis, _series);

    _chart->axes(Qt::Vertical).first()->setRange(0, max);

    _chart->legend()->setVisible(true);
    _chart->legend()->setAlignment(Qt::AlignRight);

    _view->setChart(_chart);
}

int BarView::maxValue(QList<QBarSet *> list)
{
    int max = 0;
    for(const auto& set : list)
        if (set->at(0) > max)
            max = set->at(0);

    return max;
}

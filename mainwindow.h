#pragma once

#include <QMainWindow>

#include <QAbstractItemModel>

#include "barview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void about();

private:
    void setupModel();
    void setupViews();
    void loadFile(const QString &path);
    void save(const QString &fileName);

    QAbstractItemModel *_model;
    BarView *_barView;

    QString _currentFileName;
};

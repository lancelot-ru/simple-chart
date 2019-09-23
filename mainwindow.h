#pragma once

#include <QMainWindow>

#include <QAbstractItemModel>

#include "barview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void openFile();
    void saveFile();
    void saveFileAs();
    void addRow();
    void about();

private:
    void setupModel();
    void setupViews();
    void loadFile(const QString &path);
    void save(const QString &fileName);
    bool addDialog();

    QStandardItemModel *_model;
    BarView *_barView;
    QTableView *_table;

    QString _currentFileName;
};

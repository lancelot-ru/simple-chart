#include "mainwindow.h"

#include <QtWidgets>

const int FACULTIES_ROWS = 14;
const int FACULTIES_COLUMNS = 2;
const int STATUS_BAR_TIMEOUT = 4*1000;

MainWindow::MainWindow()
    : _model(nullptr)
    , _barView(nullptr)
    , _table(nullptr)
{
    auto fileMenu = new QMenu(tr("&Файл"), this);
    auto openAction = fileMenu->addAction(tr("&Открыть..."));
    openAction->setShortcuts(QKeySequence::Open);
    auto saveAction = fileMenu->addAction(tr("&Сохранить..."));
    saveAction->setShortcuts(QKeySequence::Save);
    auto saveAsAction = fileMenu->addAction(tr("&Сохранить как..."));
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    auto quitAction = fileMenu->addAction(tr("&Выйти"));
    quitAction->setShortcuts(QKeySequence::Quit);

    auto editMenu = new QMenu(tr("&Редактировать"), this);
    auto addAction = editMenu->addAction(tr("&Добавить ряд"));

    auto helpMenu = new QMenu(tr("&Помощь"), this);
    auto aboutAction = helpMenu->addAction(tr("&О программе"), this, &MainWindow::about);
    aboutAction->setShortcuts(QKeySequence::HelpContents);

    setupModel();
    loadFile(":/Charts/faculties.rff");
    setupViews();

    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(addAction, &QAction::triggered, this, &MainWindow::addRow);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(helpMenu);
    statusBar();

    setWindowTitle(tr("Диаграмма"));
    resize(870, 550);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
    {
        auto selection = _table->selectionModel()->selectedRows();
        if (!selection.isEmpty())
        {
            auto reply = QMessageBox::question(this, "Удалить ряд?", "Вы уверены, что хотите удалить выбранный ряд?",
                                               QMessageBox::Ok | QMessageBox::Cancel);
            if (reply == QMessageBox::Ok)
            {
                _model->removeRow(selection.at(0).row());
                _barView->refresh();
            }
        }
    }
    }
}

void MainWindow::setupModel()
{
    _model = new QStandardItemModel(FACULTIES_ROWS, FACULTIES_COLUMNS, this);
}

void MainWindow::setupViews()
{
    auto splitter = new QSplitter(this);
    _table = new QTableView(this);
    _table->setMaximumWidth(400);
    _barView = new BarView(this);
    splitter->addWidget(_table);
    splitter->addWidget(_barView);
    splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));

    _table->setModel(_model);
    _barView->setModel(_model);
    connect(_model, &QAbstractItemModel::dataChanged, _barView, &BarView::update);

    auto selectionModel = new QItemSelectionModel(_model);
    _table->setSelectionModel(selectionModel);

    auto headerView = _table->horizontalHeader();
    headerView->setStretchLastSection(true);

    setCentralWidget(splitter);
}

void MainWindow::openFile()
{
    disconnect(_model, &QAbstractItemModel::dataChanged, _barView, &BarView::update);
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Выберите файл"), "", "*.rff");
    if (!fileName.isEmpty())
        loadFile(fileName);

    _barView->refresh();
    connect(_model, &QAbstractItemModel::dataChanged, _barView, &BarView::update);
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;

    QTextStream stream(&file);
    QString line;

    _model->removeRows(0, _model->rowCount(QModelIndex()), QModelIndex());

    line = stream.readLine();
    if (!line.isEmpty())
    {
        QStringList pieces = line.split(',', QString::SkipEmptyParts);
        _model->setHeaderData(0, Qt::Horizontal, pieces.value(0));
        _model->setHeaderData(1, Qt::Horizontal, pieces.value(1));
    }

    int row = 0;
    do
    {
        line = stream.readLine();
        if (!line.isEmpty())
        {
            _model->insertRows(row, 1, QModelIndex());

            QStringList pieces = line.split(',', QString::SkipEmptyParts);
            _model->setData(_model->index(row, 0, QModelIndex()), pieces.value(0));
            _model->setData(_model->index(row, 1, QModelIndex()), pieces.value(1));
            row++;
        }
    }
    while (!line.isEmpty());

    file.close();
    _currentFileName = fileName;
    statusBar()->showMessage(tr("Загружен %1").arg(fileName), STATUS_BAR_TIMEOUT);
}

void MainWindow::saveFile()
{
    if (QString::compare(_currentFileName, ":/Charts/faculties.rff") == 0)
    {
        statusBar()->showMessage(tr("Демонстрационный файл не может быть сохранен!"), STATUS_BAR_TIMEOUT);
    }
    else
    {
        save(_currentFileName);
    }
}

void MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл как"), "", "*.rff");

    if (fileName.isEmpty())
        return;

    save(fileName);
}

void MainWindow::addRow()
{
    bool ok = addDialog();

    if (ok)
    {
        statusBar()->showMessage(tr("Добавлен ряд"), STATUS_BAR_TIMEOUT);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("О программе"), tr("ТГУ, РФФ, май 2018"));
}

void MainWindow::save(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;

    QTextStream stream(&file);

    {
        QStringList pieces;

        pieces.append(_model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
        pieces.append(_model->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

        stream << pieces.join(',') << "\n";
    }

    for (int row = 0; row < _model->rowCount(QModelIndex()); ++row)
    {
        QStringList pieces;

        pieces.append(_model->data(_model->index(row, 0, QModelIndex()), Qt::DisplayRole).toString());
        pieces.append(_model->data(_model->index(row, 1, QModelIndex()), Qt::DisplayRole).toString());

        stream << pieces.join(',') << "\n";
    }

    file.close();
    statusBar()->showMessage(tr("Сохранен %1").arg(fileName), STATUS_BAR_TIMEOUT);
}

bool MainWindow::addDialog()
{
    QDialog dialog(this, Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("Добавить ряд");
    QFormLayout form(&dialog);

    form.addRow(new QLabel("Добавить ряд"));

    auto stringEdit = new QLineEdit(&dialog);
    QString stringLabel = QString("Строка");
    form.addRow(stringLabel, stringEdit);

    auto numberEdit = new QLineEdit(&dialog);
    QString numberLabel = QString("Значение");
    form.addRow(numberLabel, numberEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
    {
        QStandardItem *stringItem = new QStandardItem(stringEdit->text());
        QStandardItem *numberItem = new QStandardItem(numberEdit->text());

        _model->appendRow(QList<QStandardItem *>() << stringItem << numberItem);
        _barView->refresh();

        return true;
    }

    return false;
}

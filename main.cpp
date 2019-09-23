#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(simple_chart);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/Charts/icon.png"));

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(), "qt", "_",
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        app.installTranslator(&qtTranslator);
    }

    MainWindow window;
    window.show();
    return app.exec();
}

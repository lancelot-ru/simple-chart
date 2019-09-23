QT += widgets
QT += charts

HEADERS       = mainwindow.h \
                barview.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                barview.cpp
RESOURCES     = simple_chart.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

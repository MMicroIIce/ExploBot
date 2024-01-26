TEMPLATE = app
TARGET = ExploBot

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    client_tcp/dispatcher.cpp \
    client_tcp/postman.cpp \
    client_tcp/proxyPilot.cpp \
    customgraphicsview.cpp \
    main.cpp \
    map.cpp \
    window.cpp

RESOURCES += \
    resources.qrc

HEADERS += \
    client_tcp/defs.h \
    client_tcp/dispatcher.h \
    client_tcp/postman.h \
    client_tcp/proxyPilot.h \
    customgraphicsview.h \
    map.h \
    window.h

CONFIG += file_copies

QT += network
QT += multimedia multimediawidgets

COPIES += pythonScript
pythonScript.files = trajectory.py generate_commands.py
pythonScript.path = $$OUT_PWD # anciennement pythonScript.path = $$OUT_PWD/codes_python

# DISTFILES += \
#     code_python.py

DISTFILES += \
    generate_commands.py \
    trajectory.py


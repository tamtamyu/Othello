QT += widgets

CONFIG += c++17

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/AIParameters.cpp \
    src/OthelloAI.cpp \
    src/OthelloBoard.cpp \
    src/OthelloGame.cpp \
    src/OthelloWindow.cpp

HEADERS += \
    include/AIParameters.h \
    include/OthelloAI.h \
    include/OthelloBoard.h \
    include/OthelloGame.h \
    include/OthelloWindow.h

DISTFILES += \
    ai_training/ai_params.json \
    ai_training/train_othello_ai.py
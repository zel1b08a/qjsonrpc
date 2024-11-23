TEMPLATE = lib

NAME_APPLICATION = qjsonrpc

TARGET = $$qtLibraryTarget($${NAME_APPLICATION})

VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_PATCH = 0

VER_MAJ = $${VERSION_MAJOR}
VER_MIN = $${VERSION_MINOR}
VER_PAT = $${VERSION_PATCH}
VERSION = $$join(VER_MIN,.,$${VER_MAJ}.,.$${VER_PAT})

# QMAKE_TARGET_COMPANY =
QMAKE_TARGET_PRODUCT = $${NAME_APPLICATION}
QMAKE_TARGET_DESCRIPTION = There is a json-rpc protocol library based on Qt Json classes.
# QMAKE_TARGET_COPYRIGHT =

QT += core
QT -= gui

QMAKE_SUBSTITUTES += config.hpp.in

DEFINES += QJSONRPC_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

PRECOMPILED_HEADER = $${NAME_APPLICATION}/stable.h

CONFIG += c++17 shared warn_on

build_pass {
    CONFIG(debug, debug|release) {
        BUILD_PREFIX = debug
    } else {
        BUILD_PREFIX = release
    }
} else {
    BUILD_PREFIX = .
}

OBJECTS_DIR = $${BUILD_PREFIX}/.obj
MOC_DIR = $${BUILD_PREFIX}/.moc
UI_DIR = $${BUILD_PREFIX}/.ui
DESTDIR = $${BUILD_PREFIX}/bin

HEADERS += \
    $${NAME_APPLICATION}/qjsonrpc-export.hpp \
    $${NAME_APPLICATION}/qjson-rpc.hpp

SOURCES += \
    $${NAME_APPLICATION}/qjson-rpc.cpp

OTHER_FILES += \
    scripts/general.sh \
    build.sh \
    .gitignore \
    config.cmake.in \
    qjsonrpc-config.hpp.in \
    LICENSE \
    README.md

headers.target = headers

unix {
    headers.path = include/$${NAME_APPLICATION}
    headers.files = $${HEADERS}

    target.path = lib
}

!isEmpty(headers.path): INSTALLS += headers
!isEmpty(target.path): INSTALLS += target

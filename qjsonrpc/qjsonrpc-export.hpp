#pragma once

#include <QtCore/qglobal.h>

#if defined(QJSONRPC_LIBRARY)
#  define LIBQJSONRPC_EXPORT Q_DECL_EXPORT
#else
#  define LIBQJSONRPC_EXPORT Q_DECL_IMPORT
#endif

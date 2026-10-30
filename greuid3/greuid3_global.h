#pragma once

#include <QtCore/qglobal.h>

#if defined(GREUID3_LIBRARY)
#define GREUID3_EXPORT Q_DECL_EXPORT
#else
#define GREUID3_EXPORT Q_DECL_IMPORT
#endif

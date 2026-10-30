#pragma once
#include "greucore/include/greuGlobal.h"

namespace greu::movies {

void GREU_PUBLIC(MOVIES) openDatabase(QString const &db, bool setup = false);
void GREU_PUBLIC(MOVIES) initDatabase(QString const &db, QString const &rootPass);

} // namespace greu::movies

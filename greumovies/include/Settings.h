#pragma once
#include "greucore/include/DetailPtr.h"

namespace greu::core {
class Language;
}

namespace greu::movies {
class Settings;
}

namespace greu::movies::settings::privatens {
greu::movies::Settings *instance();
}

namespace greu::movies {

class GREU_PUBLIC(MOVIES) Settings {
public:
    static Settings *instance();
    static Settings *instance(bool readOnly);

    core::Language const &language() const;
    void setLanguage(core::Language const &);

    QString pathPrefix() const;
    void setPathPrefix(QString);

    int64_t volume() const;
    void setVolume(int64_t);

    uint tilesPerRowBig() const;
    void setTilesPerRowBig(uint);

    uint tilesPerRowSmall() const;
    void setTilesPerRowSmall(uint);

private:
    Settings();
    friend greu::movies::Settings *greu::movies::settings::privatens::instance();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies

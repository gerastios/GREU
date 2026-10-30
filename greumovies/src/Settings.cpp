#include "Settings.h"
#include <QDir>
#include <QSqlQuery>
#include <QStandardPaths>
#include "Strings.h"
#include "greucore/include/Language.h"
#include "greusql/include/Table.h"

using namespace std;

namespace greu::movies::settings::privatens {
greu::movies::Settings *instance() {
    static greu::movies::Settings i;
    return &i;
}
} // namespace greu::movies::settings::privatens

namespace greu::movies {

using namespace core;
using namespace sql;

struct Settings::Detail {
    // clang-format off
    static size_t constexpr SLINE_TYPE = __LINE__,
        LANGUAGE = __LINE__ - SLINE_TYPE,
        PATH_PREFIX = __LINE__ - SLINE_TYPE,
        TILES_PER_ROW_BIG = __LINE__ - SLINE_TYPE,
        TILES_PER_ROW_SMALL = __LINE__ - SLINE_TYPE,
        VOLUNE = __LINE__ - SLINE_TYPE,
        SLINE = __LINE__ + 1,
        ID = __LINE__ - SLINE,
        SETTINGVALUE = __LINE__ - SLINE;
    // clang-format on

    struct MyTable : public Table {
        MyTable()
            : Table("settings",
                    {{Strings::fieldId, DataType::INT, true},
                     {"settingvalue", DataType::VARCHAR, 600}}) {
            create();
        }
        optional<QString> read(int id) {
            decltype(Table::selectSingle({})) o;
            try {
                o = selectSingle({{id}});
            } catch (...) {
                return nullopt;
            }
            if (!o)
                return nullopt;
            return o.value().at(SETTINGVALUE).toString();
        }
        void save(int id, QString const &value) { insertOrUpdate({id, value}); }
    };
    MyTable table;
    Language const *language;
    QString pathPrefix;
    int64_t volume;
    uint tilesPerRowBig, tilesPerRowSmall;
    bool readOnly, isInitialized = false;

    Detail()
        : table() {}

    void init() {
        if (isInitialized)
            return;
        isInitialized = true;
        auto ls = table.read(LANGUAGE);
        if (ls)
            language = Language::fromCode2(ls.value())
                           .value_or(Language::fromCode2(Language::Code2::English).value());
        else
            language = Language::fromCode2(Language::Code2::English).value();
        if (!readOnly)
            table.save(LANGUAGE, language->code2());
        auto pp = table.read(PATH_PREFIX);
        if (pp)
            pathPrefix = pp.value();
        else
            pathPrefix = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        if (pathPrefix.last(1) != "/")
            pathPrefix += "/";
        tilesPerRowBig = getIntValue(TILES_PER_ROW_BIG, 6);
        tilesPerRowSmall = getIntValue(TILES_PER_ROW_SMALL, 9);
        volume = getIntValue(VOLUNE, 109);
    }

    template<typename T>
    T getIntValue(int id, T defaultVal) {
        auto uv = table.read(id);
        if (uv)
            return uv.value().toInt();
        else
            return defaultVal;
    }

    template<typename T>
    void setIntValue(int id, T v, T &stored) {
        if (v == stored)
            return;
        stored = v;
        if (!readOnly)
            table.save(id, QString("%1").arg(v));
    }
};

Settings::Settings()
    : d(GREU_MKDET()) {}

Settings *Settings::instance() {
    auto res = settings::privatens::instance();
    if (!res->d->isInitialized) {
        res->d->readOnly = true;
        res->d->init();
    }
    return res;
}

Settings *Settings::instance(bool ro) {
    auto res = settings::privatens::instance();
    if (!res->d->isInitialized) {
        res->d->readOnly = ro;
        res->d->init();
    }
    return res;
}

Language const &Settings::language() const {
    return *d->language;
}

void Settings::setLanguage(Language const &v) {
    d->language = &v;
    if (!d->readOnly)
        d->table.save(Detail::LANGUAGE, v.code2());
}

QString Settings::pathPrefix() const {
    return d->pathPrefix;
}

void Settings::setPathPrefix(QString v) {
    if (v.last(1) != "/")
        v += "/";
    if (v == d->pathPrefix)
        return;
    d->pathPrefix = v;
    if (!d->readOnly)
        d->table.save(Detail::PATH_PREFIX, v);
}

int64_t Settings::volume() const {
    return d->volume;
}

void Settings::setVolume(int64_t v) {
    d->setIntValue(Detail::VOLUNE, v, d->volume);
}

uint Settings::tilesPerRowBig() const {
    return d->tilesPerRowBig;
}

void Settings::setTilesPerRowBig(uint v) {
    d->setIntValue(Detail::TILES_PER_ROW_BIG, v, d->tilesPerRowBig);
}

uint Settings::tilesPerRowSmall() const {
    return d->tilesPerRowSmall;
}

void Settings::setTilesPerRowSmall(uint v) {
    d->setIntValue(Detail::TILES_PER_ROW_SMALL, v, d->tilesPerRowSmall);
}

} // namespace greu::movies

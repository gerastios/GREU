#include "TheMovieDatabase.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QUrlQuery>
#include "JsonRequester.h" // IWYU pragma: keep
#include "Reader.h"
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"
#include <iterator>
#include <unordered_map>

using namespace std;
using namespace greu::net;
using namespace greu::tmdb;
using namespace greu::core;

namespace {

QString const APIKey = "2900b64f48d4b6c2f1a74aab4d7775a3";
QString const ReadToken
    = "eyJhbGciOiJIUzI1NiJ9."
      "eyJhdWQiOiIyOTAwYjY0ZjQ4ZDRiNmMyZjFhNzRhYWI0ZDc3NzVhMyIsIm5iZiI6MTczMDI5NzExNC41NjYwMDAyLCJz"
      "dWIiOiI2NzIyM2QxOWMzZjY1YTkzYTQ0ZmYxOGEiLCJzY29wZXMiOlsiYXBpX3JlYWQiXSwidmVyc2lvbiI6MX0."
      "2EG3CFR3Ff143U_Xr3t5bcXvB01YpxD5PVg-m6P89BA";

QString imagePath;
bool initialized = false;
QString bigSize, smallSize;
unordered_map<int, QString> movieGenres;
unordered_map<int, QString> seriesGenres;
unordered_map<QString, QString> countries, languages;
QString const movie{"movie"}, series{"tv"};
vector<TheMovieDatabase::Job> jobs;
vector<QString> jobDepartments;
unordered_map<QString, QString> departmentOfJob;

struct Configuration {
    struct ConfigurationRequester : public greu::tmdb::JsonRequester {
        using JsonRequester::JsonRequester;
        virtual QUrl url() const override {
            return QUrl("https://api.themoviedb.org/3/configuration");
        }
        virtual QUrlQuery query() const override { return QUrlQuery(); };
    };

    struct CountriesRequester : public greu::tmdb::JsonRequester {
        TheMovieDatabase const *const that;
        CountriesRequester(TheMovieDatabase const *that)
            : that(that) {}
        virtual QUrl url() const override {
            return QUrl("https://api.themoviedb.org/3/configuration/countries");
        }
        virtual QUrlQuery query() const override {
            QUrlQuery res;
            res.addQueryItem("language", that->language().code1());
            return res;
        };
    };

    struct LanguagesRequester : public greu::tmdb::JsonRequester {
        TheMovieDatabase const *const that;
        LanguagesRequester(TheMovieDatabase const *that)
            : that(that) {}
        virtual QUrl url() const override {
            return QUrl("https://api.themoviedb.org/3/configuration/languages");
        }
        virtual QUrlQuery query() const override { return QUrlQuery(); };
    };

    struct JobsRequester : public greu::tmdb::JsonRequester {
        TheMovieDatabase const *const that;
        JobsRequester(TheMovieDatabase const *that)
            : that(that) {}
        virtual QUrl url() const override {
            return QUrl("https://api.themoviedb.org/3/configuration/jobs");
        }
        virtual QUrlQuery query() const override { return QUrlQuery(); };
    };

    struct GenreRequester : public greu::tmdb::JsonRequester {
        TheMovieDatabase const *const that;
        QString const what;
        GenreRequester(TheMovieDatabase const *that, QString const &what)
            : that(that)
            , what(what) {}
        virtual QUrl url() const override {
            return QUrl("https://api.themoviedb.org/3/genre/" + what + "/list");
        }
        virtual QUrlQuery query() const override {
            QUrlQuery res;
            res.addQueryItem("language", that->language().code1());
            return res;
        };
    };

    Configuration(TheMovieDatabase const *that) {
        if (!initialized) {
            initialize(that);
            initialized = true;
        }
    }

    void initialize(TheMovieDatabase const *that) {
        readConfiguration();
        readLanguages(that);
        movieGenres = readGenres(that, movie);
        seriesGenres = readGenres(that, series);
        readCountries(that);
        readCountries(that);
        readJobs(that);
    }

    void readConfiguration() {
        greu::tmdb::Reader r;
        auto json = ConfigurationRequester{}.readJson(&r);
        auto o = json.object().value("images").toObject();
        imagePath = valueString(o, "secure_base_url");
        auto is = o.value("poster_sizes").toArray();
        smallSize = is.at(0).toString();
        bigSize = is.at(is.size() - 1).toString();
    }

    unordered_map<int, QString> readGenres(TheMovieDatabase const *that, QString const &what) {
        greu::tmdb::Reader r;
        auto json = GenreRequester{that, what}.readJson(&r);
        unordered_map<int, QString> res;
        auto genres = json.object().value("genres").toArray();
        for (auto g : genres) {
            auto o = g.toObject();
            res[valueInt(o, "id")] = valueString(o, "name");
        }
        return res;
    }

    void readCountries(TheMovieDatabase const *that) {
        greu::tmdb::Reader r;
        auto json = CountriesRequester{that}.readJson(&r);
        auto values = json.array();
        for (auto c : values) {
            auto o = c.toObject();
            auto n = valueString(o, "native_name");
            countries[valueString(o, "iso_3166_1")] = (n.isEmpty() ? valueString(o, "english_name")
                                                                   : n);
        }
    }

    void readLanguages(TheMovieDatabase const *that) {
        greu::tmdb::Reader r;
        auto json = LanguagesRequester{that}.readJson(&r);
        auto values = json.array();
        for (auto c : values) {
            auto o = c.toObject();
            auto n = valueString(o, "name");
            languages[valueString(o, "iso_639_1")] = (n.isEmpty() ? valueString(o, "english_name")
                                                                  : n);
        }
    }

    void readJobs(TheMovieDatabase const *that) {
        greu::tmdb::Reader r;
        auto json = JobsRequester{that}.readJson(&r);
        auto values = json.array();
        for (auto c : values) {
            auto o = c.toObject();
            auto d = valueString(o, "department");
            auto js = valuesString(o, "jobs");
            jobDepartments.push_back(d);
            for (auto &j : js) {
                jobs.push_back({.name = j, .department = d});
                departmentOfJob[j] = d;
            }
        }
    }
};

QString genre(int id, unordered_map<int, QString> genres) {
    try {
        return genres.at(id);
    } catch (...) {
        return QString::number(id);
    }
}

vector<QString> determineGenres(QJsonObject const &object,
                                unordered_map<int, QString> const &genres) {
    auto gi = valuesInt(object, "genre_ids");
    vector<QString> res;
    for (auto g : gi)
        res.push_back(genre(g, genres));
    return res;
}

set<QString> fillGenres(QJsonValue const &json, unordered_map<int, QString> const &genres) {
    set<QString> res;
    if (!json.isArray())
        return res;
    for (auto g : json.toArray()) {
        if (g.isDouble()) {
            res.insert(genre(g.toInt(), genres));
            continue;
        }
        if (g.isObject()) {
            auto o = g.toObject();
            auto name = valueString(o, "name");
            if (!name.isEmpty()) {
                res.insert(name);
                continue;
            }
            auto id = o.value("id");
            if (id.isDouble()) {
                res.insert(genre(id.toInt(), genres));
                continue;
            }
        }
    }
    return res;
}

QString mapString(QString const &id, unordered_map<QString, QString> const &map) {
    try {
        return map.at(id);
    } catch (...) {
    }
    return id;
}

set<QString> fillMappedStrings(QJsonValue const &json,
                               QString const &idString,
                               QString const &nameString,
                               QString const &englishNameString,
                               int idLength,
                               function<QString(QString const &)> mapFunction) {
    set<QString> res;
    if (!json.isArray())
        return res;
    for (auto c : json.toArray()) {
        if (c.isString()) {
            auto s = c.toString();
            if (s.length() == idLength)
                res.insert(mapFunction(s));
            else
                res.insert(s);
            continue;
        }
        if (c.isObject()) {
            auto o = c.toObject();
            if (!nameString.isEmpty()) {
                auto n = valueString(o, nameString);
                if (!n.isEmpty()) {
                    res.insert(n);
                    continue;
                }
            }
            if (!englishNameString.isEmpty()) {
                auto n = valueString(o, englishNameString);
                if (!n.isEmpty()) {
                    res.insert(n);
                    continue;
                }
            }
            if (!idString.isEmpty()) {
                auto i = valueString(o, idString);
                if (!i.isEmpty()) {
                    res.insert(mapFunction(i));
                    continue;
                }
            }
        }
    }
    return res;
}

} // namespace

namespace greu::tmdb {

using namespace core;

struct TheMovieDatabase::Detail {
    Language const &language;
    Language const &fallbackLanguage;
    static TheMovieDatabase *instance;
    Detail(Language const &l, Language const &fl)
        : language(l)
        , fallbackLanguage(fl) {}
};

TheMovieDatabase *TheMovieDatabase::Detail::instance = nullptr;

bool TheMovieDatabase::isUsed() {
    return (bool) Detail::instance;
}

TheMovieDatabase::TheMovieDatabase(Language const &language, Language const &fallbackLanguage)
    : d(GREU_MKDET(language, fallbackLanguage)) {
    assert(Detail::instance == nullptr);
    Detail::instance = this;
    Configuration(this);
}

TheMovieDatabase *TheMovieDatabase::instance() {
    assert(Detail::instance);
    return Detail::instance;
}

Language const &TheMovieDatabase::language() const {
    return d->language;
}

Language const &TheMovieDatabase::fallbackLanguage() const {
    return d->fallbackLanguage;
}

QString TheMovieDatabase::imagePath() {
    return ::imagePath;
}

QString TheMovieDatabase::imageBigSizeInfix() {
    return bigSize;
}

QString TheMovieDatabase::imageSmallSizeInfix() {
    return smallSize;
}

QString TheMovieDatabase::movieGenre(int id) {
    return genre(id, ::movieGenres);
}

QString TheMovieDatabase::seriesGenre(int id) {
    return genre(id, ::seriesGenres);
}

QString TheMovieDatabase::mapCountry(QString const &id) {
    return mapString(id, countries);
}

QString TheMovieDatabase::mapLanguage(QString const &id) {
    return mapString(id, languages);
}

vector<TheMovieDatabase::Job> const &TheMovieDatabase::jobs() {
    return ::jobs;
}

vector<QString> const &TheMovieDatabase::jobDepartments() {
    return ::jobDepartments;
}

QString TheMovieDatabase::jobDepartment(QString job) {
    try {
        return ::departmentOfJob.at(job);
    } catch (...) {
        return QObject::tr("unkonw");
    }
}

unordered_map<QString, QString> const &TheMovieDatabase::departmentsOfJob() {
    return ::departmentOfJob;
}

QString TheMovieDatabase::typeMovie() {
    return movie;
}

QString TheMovieDatabase::typeSeries() {
    return series;
}

QString TheMovieDatabase::apiKey() {
    return APIKey;
}

QString TheMovieDatabase::readToken() {
    return ReadToken;
}

vector<QString> TheMovieDatabase::determineMovieGenres(QJsonObject const &object) {
    return determineGenres(object, ::movieGenres);
}

vector<QString> TheMovieDatabase::determineSeriesGenres(QJsonObject const &object) {
    return determineGenres(object, ::seriesGenres);
}

set<QString> TheMovieDatabase::fillMovieGenres(QJsonValue const &json) {
    return fillGenres(json, ::movieGenres);
}

set<QString> TheMovieDatabase::fillSeriesGenres(QJsonValue const &json) {
    return fillGenres(json, ::seriesGenres);
}

set<QString> TheMovieDatabase::fillCountries(QJsonValue const &json,
                                             QString const &idString,
                                             QString const &nameString,
                                             QString const &englikshNameString) {
    return fillMappedStrings(json,
                             idString,
                             nameString,
                             englikshNameString,
                             2,
                             TheMovieDatabase::mapCountry);
}

set<QString> TheMovieDatabase::fillLanguages(QJsonValue const &json,
                                             QString const &idString,
                                             QString const &nameString,
                                             QString const &englikshNameString) {
    return fillMappedStrings(json,
                             idString,
                             nameString,
                             englikshNameString,
                             2,
                             TheMovieDatabase::mapLanguage);
}

set<QString> TheMovieDatabase::movieGenres() {
    set<QString> res;
    ranges::transform(::movieGenres.cbegin(),
                      ::movieGenres.cend(),
                      inserter(res, res.begin()),
                      [](auto g) { return g.second; });
    return res;
}

set<QString> TheMovieDatabase::seriesGenres() {
    set<QString> res;
    ranges::transform(::seriesGenres.cbegin(),
                      ::seriesGenres.cend(),
                      inserter(res, res.begin()),
                      [](auto g) { return g.second; });
    return res;
}

} // namespace greu::tmdb

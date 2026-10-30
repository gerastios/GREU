#pragma once
#include <QString>
#include "greucore/include/DetailPtr.h"
#include <set>
#include <vector>

class QJsonObject;
class QJsonValue;

namespace greu::core {
class Language;
}
namespace greu::tmdb {

class GREU_PUBLIC(TMDB) TheMovieDatabase {
public:
    struct Job {
        QString name, department;
    };

    static bool isUsed();

    TheMovieDatabase(core::Language const &, core::Language const &fallback);
    static TheMovieDatabase *instance();
    GREU_NO_COPY(TheMovieDatabase);
    GREU_NO_MOVE(TheMovieDatabase);

    core::Language const &language() const;
    core::Language const &fallbackLanguage() const;
    QString imagePath();
    QString imageBigSizeInfix();
    QString imageSmallSizeInfix();
    QString movieGenre(int id);
    QString seriesGenre(int id);
    static QString mapCountry(QString const &id);
    static QString mapLanguage(QString const &id);

    std::vector<Job> const &jobs();
    std::vector<QString> const &jobDepartments();
    QString jobDepartment(QString job);
    std::unordered_map<QString, QString> const &departmentsOfJob();

    QString typeMovie();
    QString typeSeries();

    QString apiKey();
    QString readToken();

    std::vector<QString> determineMovieGenres(QJsonObject const &);
    std::vector<QString> determineSeriesGenres(QJsonObject const &);
    std::set<QString> fillMovieGenres(QJsonValue const &);
    std::set<QString> fillSeriesGenres(QJsonValue const &);
    std::set<QString> fillCountries(QJsonValue const &,
                                    QString const &idString = "",
                                    QString const &nameString = "",
                                    QString const &englishNameString = "");
    std::set<QString> fillLanguages(QJsonValue const &,
                                    QString const &idString = "",
                                    QString const &nameString = "",
                                    QString const &englishNameString = "");

    std::set<QString> movieGenres();
    std::set<QString> seriesGenres();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

#define GREU_TMDB TheMovieDatabase::instance()

} // namespace greu::tmdb

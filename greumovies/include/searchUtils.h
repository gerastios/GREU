#pragma once
#include "Object.h"
#include "SearchOptions.h"
#include "Searchable.h"
#include "Strings.h"
#include "greucore/include/Uuid.h"
#include "greusql/include/Query.h"

namespace greu::movies {

class MovieObject;

QString GREU_PUBLIC(MOVIES)
    buildTargetWhere(SearchOptions const &,
                     QString const &tabName,
                     QString const &titleField = Strings::fieldTitle,
                     QString const &descriptionField = Strings::fieldDescription);

template<class C>
FoundObjects GREU_PUBLIC(MOVIES)
    simpleSearch(SearchOptions const &options,
                 QString const &tabName,
                 QString const &titleField = Strings::fieldTitle,
                 QString const &descriptionField = Strings::fieldDescription) {
    auto qs = QString("select distinct id from %1 where %2")
                  .arg(tabName)
                  .arg(buildTargetWhere(options, tabName, titleField, descriptionField));
    sql::Query q{qs};
    q.bindValue(":string", options.searchString);
    q.execute();
    FoundObjects res;
    while (q.next())
        res.insert(Object::object<C>(q.value(0)));
    return res;
}

} // namespace greu::movies

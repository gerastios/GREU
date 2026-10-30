#include "searchUtils.h"
#include "greucore/include/Uuid.h"

using namespace std;

namespace greu::movies {

using namespace core;
using namespace sql;

QString buildTargetWhere(SearchOptions const &options,
                         QString const &tabName,
                         QString const &titleField,
                         QString const &descriptionField) {
    decltype(buildTargetWhere({}, {})) res;
    QString where;
    if (options.field & SearchOptions::fieldDescription)
        where = QString("%1.%2 like :string").arg(tabName).arg(descriptionField);
    if (options.field & SearchOptions::fieldTitle) {
        if (!where.isEmpty())
            where += " or ";
        where += QString("%1.%2 like :string").arg(tabName).arg(titleField);
    }
    return QString("(%1)").arg(where);
}

} // namespace greu::movies

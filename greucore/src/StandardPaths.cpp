#include "StandardPaths.h"

#include <QStorageInfo>

using namespace std;

namespace {

using Mappings = vector<pair<QString, QString>>;
Mappings mappings;

void init() {
    static bool initialized = false;
    if(initialized) return;
    initialized = true;
    auto vols = QStorageInfo::mountedVolumes();
    auto drives = QDir::drives();
    for(auto dr: drives) {
        auto ds = dr.absoluteFilePath();
        for(auto vol: vols) {
            auto vo = QString(vol.device());
            auto vr = vol.rootPath();
            if (vr == ds && vo.length() > 0) mappings.push_back({ds, vo});
        }
    }
}

}

namespace greu::core {

QString StandardPaths::writeableLocation(QStandardPaths::StandardLocation type) {
    init();
    auto res = QStandardPaths::writableLocation(type);
    if (res.startsWith("//")) {
        for(auto m: mappings) {
            auto dvs = QDir::fromNativeSeparators(m.second);
            if (res.startsWith(dvs)) {
                res = res.right(res.length() - dvs.length());
                res = m.first + res;
            }
        }
    }
    return res;
}

vector<QString> StandardPaths::rootPaths() {
    init();
    vector<QString> res;
    for(auto m: mappings) res.push_back(m.first);
    return res;
}

}

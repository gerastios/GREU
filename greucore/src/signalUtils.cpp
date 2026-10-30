#include "signalUtils.h"

using namespace std;

namespace greu::core {

void disconnect(vector<ScopedConnection> &connections) {
    for (auto &c : connections)
        c.disconnect();
}

void disconnect(vector<QMetaObject::Connection> &connections) {
    for (auto c : connections)
        disconnect(c);
}

void disconnect(QMetaObject::Connection &connection) {
    if (connection) {
        QObject::disconnect(connection);
        connection = {};
    }
}

} // namespace greu::core

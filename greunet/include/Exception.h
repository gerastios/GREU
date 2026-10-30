#pragma once

#include <QNetworkReply>
#include "greucore/include/Exception.h"

namespace greu::net {

class GREU_PUBLIC(NET) Exception : public core::Exception {
public:
    Exception(QNetworkReply::NetworkError const,
              QString const &text,
              QUrl const &,
              QString const &response = "");

    QNetworkReply::NetworkError code() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::net

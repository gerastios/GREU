#pragma once
#include "greucore/include/DetailPtr.h"
#include <QIODevice>
#include <QProcess>

class QWidget;

namespace greu::ui {

class GREU_PUBLIC(UI) ProcessViewer {
public:
    explicit ProcessViewer(QWidget *widget);

    void setPath(QString const &);
    void start(QString const & program, std::vector<QString> const & arguments, QIODevice::OpenMode = QProcess::ReadWrite);

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}

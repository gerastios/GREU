#pragma once
#include <QTextBrowser>
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

class QTextDocument;

namespace greu::ui {

class GREU_PUBLIC(UI) TextBrowser : public QTextBrowser {
public:
    TextBrowser();

    void clear();
    void setHtml(QString const &);
    QTextDocument &document() const;

    core::StringSig &linkClicked() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui

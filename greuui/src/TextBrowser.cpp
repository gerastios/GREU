#include "TextBrowser.h"
#include <QTextDocument>
#include "connect.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct TextBrowser::Detail {
    QTextDocument document;
    StringSig linkClicked;
};

TextBrowser::TextBrowser()
    : d(GREU_MKDET()) {
    setDocument(&d->document);
    setOpenExternalLinks(false);
    setOpenLinks(false);
    setReadOnly(true);
    ui::connect(this, [this](auto u) { d->linkClicked(u.toString()); });
}

void TextBrowser::clear() {
    d->document.clear();
}

void TextBrowser::setHtml(QString const &v) {
    d->document.setHtml(v);
}

QTextDocument &TextBrowser::document() const {
    return d->document;
}

StringSig &TextBrowser::linkClicked() const {
    return d->linkClicked;
}

} // namespace greu::ui

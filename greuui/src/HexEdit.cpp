#include "HexEdit.h"
#include <QMenu>
#include "connect.h"
#include "greucore/include/hexUtils.h"
#include <qevent.h>

namespace greu::ui {

using namespace core;

HexEdit::HexEdit() {
    setEnableValidateColor(false);
    ui::connectTextEdited(this, [this](QString const &text) {
        auto cur_pos = cursorPosition();
        if (cur_pos == text.size())
            cur_pos = -1;
        auto t = core::formatInput(text);
        setText(t);
        if (cur_pos < 0 || cur_pos > t.length())
            setCursorPosition(t.length());
        else
            setCursorPosition(cur_pos);
    });
}

void HexEdit::setBytes(QByteArray const &bytes) {
    setText(core::string(bytes));
}

QByteArray HexEdit::bytes() const {
    return core::bytes(text());
}

void HexEdit::contextMenuEvent(QContextMenuEvent *event) {
    auto menu = createStandardContextMenu();
    menu->addSeparator();
    menu->exec(event->pos());
}

} // namespace greu::ui

#include "ItemDelegate.h"
#include "connect.h"
#include "greucore/include/vectorUtils.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct ItemDelegate::Detail {
    vector<QWidget *> editors;

    void commit(ItemDelegate *that) {
        for (auto e : editors)
            emit that->commitData(e);
    }

    QWidget *createEditor(QWidget *parent,
                          QStyleOptionViewItem const &option,
                          QModelIndex const &index,
                          ItemDelegate const *that) {
        auto ed = that->createWatchedEditor(parent, option, index);
        if (!ed)
            return nullptr;
        if (!core::contains(editors, ed))
            editors.push_back(ed);
        ui::connectDestroyed(ed, [this](QObject *obj) {
            auto w = dynamic_cast<QWidget *>(obj);
            if (w && core::contains(editors, w))
                core::erase(editors, w);
        });
        return ed;
    }
};

ItemDelegate::ItemDelegate()
    : d(GREU_MKDET()) {}

void ItemDelegate::commit() {
    d->commit(this);
}

QWidget *ItemDelegate::createEditor(QWidget *parent,
                                    QStyleOptionViewItem const &option,
                                    QModelIndex const &index) const {
    return d->createEditor(parent, option, index, this);
}

} // namespace greu::ui

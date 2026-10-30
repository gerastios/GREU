#include "Accordion.h"
#include <QHBoxLayout>
#include <QPushButton>
#include "Icons.h"
#include "Settings.h"
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/connect.h"

using namespace std;

namespace {
char const *itemKey = "_item";
}

namespace greu::ui {

using namespace core;
using namespace ui;

struct Accordion::Detail {
    struct Item {
        GuardedPtr<QPushButton> button;
        GuardedPtr<QWidget> widget;
        bool opened;
    };
    QString const name;
    std::vector<Item> items;
    int current = -1;
    GuardedPtr<QHBoxLayout> layout;

    Detail(QString const &name)
        : name(name) {}

    void setupUi(Accordion *that) {
        layout = new QHBoxLayout;
        layout->insertStretch(-1);
        that->setLayout(layout.get());
        size_t index = 0;
        for (auto &i : items) {
            addItem(i, index++);
        }
        current = Settings::instance().value(name + itemKey, -1).toInt();
        if (current >= 0)
            openItem(items[current]);
    }

    void addItem(Item &item, size_t index) {
        layout->insertWidget(index, item.button.get());
        layout->insertWidget(index + 1, item.widget.get());
        item.widget->setVisible(false);
    }

    void addItem(QString const &text, QWidget *widget, Accordion *that) {
        int i = items.size();
        Item item{new QPushButton(GREU_ICON(folder), text, that), widget, false};
        ui::connect(item.button.get(), [this, i] {
            auto &item = items[i];
            if (item.opened) {
                closeItem(item);
                current = -1;
            } else {
                if (current >= 0) {
                    closeItem(items[current]);
                }
                current = i;
                openItem(items[current]);
            }
        });
        items.push_back(std::move(item));
    }

    void setCurrentIndex(int value) {
        if (value == current || value < 0 || value >= int(items.size()))
            return;
        if (current >= 0) {
            closeItem(items[current]);
        }
        current = value;
        openItem(items[current]);
    }

    void closeItem(Item &item) {
        item.opened = false;
        item.button->setIcon(GREU_ICON(folder));
        item.widget->setVisible(false);
    }

    void openItem(Item &item) {
        item.opened = true;
        item.button->setIcon(GREU_ICON(opened_folder));
        item.widget->setVisible(true);
    }
};

Accordion::Accordion(QString const &name)
    : d(GREU_MKDET(name)) {
    d->setupUi(this);
}

Accordion::Accordion(QString const &name, initializer_list<pair<QString const, QWidget *> > items)
    : d(GREU_MKDET(name)) {
    for (auto &i : items) {
        d->addItem(i.first, i.second, this);
    }
}

Accordion::Accordion(QString const &name, ItemList items)
    : d(GREU_MKDET(name)) {
    for (auto &i : items) {
        d->addItem(i.first, i.second, this);
    }
}

void Accordion::addItem(QString const &text, QWidget *widget) {
    d->addItem(text, widget, this);
}

int Accordion::currentIndex() const {
    return d->current;
}

void Accordion::setCurrentIndex(int value) {
    d->setCurrentIndex(value);
}

} // namespace greu::ui

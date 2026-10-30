#pragma once
#include <QWidget>
#include "greucore/include/DetailPtr.h"
#include <initializer_list>

namespace greu::ui {

class GREU_PUBLIC(UI) Accordion : public QWidget {
public:
    using ItemList = std::vector<std::pair<QString const, QWidget *>>;

    explicit Accordion(QString const &name);
    Accordion(QString const &name, std::initializer_list<std::pair<QString const, QWidget *>> items);
    Accordion(QString const &name, ItemList items);

    void addItem(QString const &text, QWidget *);
    int currentIndex() const;
    void setCurrentIndex(int value);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui

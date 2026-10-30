#include "NavigationController.h"
#include "PresentationItem.h"
#include "PresentationWidget.h"
#include "greuui/include/BusyCursor.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;

struct NavigationController::Detail {
    int currentIndex = -1;
    unique_ptr<PresentationItem> current;
    unique_ptr<PresentationItem> older;
    vector<Presentation> stack;
    EmptySig currentSet;

    void setCurrent(Presentation const &p) {
        if (current && current->presentation() == p)
            return;
        BusyCursor _;
        saveCurrentPosition();
        pushItem(p);
        if (currentIndex >= 0 && currentIndex != ((int) stack.size()) - 1)
            stack.erase(stack.begin() + currentIndex + 1, stack.end());
        ++currentIndex;
        stack.push_back(p);
        restoreCurrentPosition();
        currentSet();
    }

    void pushItem(Presentation const &p) {
        older.reset(current.release());
        if (!p.isEmpty())
            current.reset(PresentationItem::create(p).release());
    }

    void forward() {
        BusyCursor _;
        saveCurrentPosition();
        pushItem(stack.at(++currentIndex));
        restoreCurrentPosition();
        currentSet();
    }

    void backward() {
        BusyCursor _;
        saveCurrentPosition();
        pushItem(stack.at(--currentIndex));
        restoreCurrentPosition();
        currentSet();
    }

    void saveCurrentPosition() {
        if(!current)
            return;
        stack.at(currentIndex).position = current->widget()->position();
    }

    void restoreCurrentPosition() {
        if(!current || !stack.at(currentIndex).position.has_value())
            return;
        current->widget()->setPosition(stack.at(currentIndex).position);
    }

};

NavigationController *NavigationController::instance() {
    static NavigationController i;
    return &i;
}

Presentation const *NavigationController::current() const {
    if (!d->current)
        return nullptr;
    return &d->current->presentation();
}

void NavigationController::setCurrent(Presentation const &p) {
    d->setCurrent(p);
}

PresentationItem *NavigationController::currentItem() const {
    return d->current.get();
}

bool NavigationController::forwardPossible() const {
    return d->currentIndex < ((int) d->stack.size()) - 1;
}

bool NavigationController::backwardPossible() const {
    return d->currentIndex > 0;
}

void NavigationController::forward() {
    assert(forwardPossible());
    d->forward();
}

void NavigationController::backward() {
    assert(backwardPossible());
    d->backward();
}

EmptySig &NavigationController::currentSet() const {
    return d->currentSet;
}

NavigationController::NavigationController()
    : d(GREU_MKDET()) {}

} // namespace greu::gaion

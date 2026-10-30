#pragma once

#include "greucore/include/greuGlobal.h"

class QWidget;
class QGridLayout;

namespace greu::grylizo {

class Dispatcher;

class AbstractAttributeEditor {
public:
    explicit AbstractAttributeEditor() = default;
    virtual ~AbstractAttributeEditor() = default;

    virtual inline QWidget *setupUi() { return nullptr; }
    virtual inline void setupUi([[maybe_unused]] QGridLayout *, [[maybe_unused]] int row) {}
    virtual void update() = 0;
    virtual void commit() = 0;

    GREU_NO_MOVE(AbstractAttributeEditor)
};

} // namespace greu::grylizo

#pragma once
#include "greucore/include/greuGlobal.h"

class QImage;
class QVariant;
class QWidget;

namespace greu::ui {

class Action;

class GREU_PUBLIC(UI) EditableTableModel {
public:
    virtual int rows() const = 0;
    virtual int columns() const = 0;

    virtual QImage const *image(int row, int column) const = 0;

    virtual QVariant const value(int row, int column) const = 0;
    virtual void setValue(int row, int column, QVariant const &value) = 0;

    virtual QString columnTitle(int column) const = 0;

    virtual void closeEditor(QWidget *editor) = 0;

    virtual bool addRow() = 0;
    virtual bool insertRow(int row) = 0;
    virtual bool deleteRow(int row) = 0;
};

} // namespace greu::ui

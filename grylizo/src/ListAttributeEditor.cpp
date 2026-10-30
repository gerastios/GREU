#include "ListAttributeEditor.h"
#include <QMessageBox>
#include <QVariant>
#include "FileEditor.h"
#include "FileTable.h"
#include "MainWindow.h"
#include "greucore/include/PlainSelector.h"
#include "greucore/include/utils.h"
#include "greutag/include/Attribute.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/File.h"
#include "greutag/include/FileContainer.h"
#include "greutag/include/Tag.h"
#include "greuui/include/Action.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/EditableTable.h"
#include "greuui/include/EditableTableModel.h"
#include "greuui/include/ExpandingComboBox.h"
#include "greuui/include/ExpandingLineEdit.h"
#include "greuui/include/Icons.h"
#include "greuui/include/ItemDelegate.h"
#include "greuui/include/TableView.h"
#include "greuui/include/connect.h"
#include "include/GuardedPtr.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct ListAttributeEditor::Adapter {
public:
    virtual int size(Attribute const *) const = 0;
    virtual QVariant const value(int row, int column, Attribute const *) const = 0;
    virtual void setValue(int row, int column, QVariant const &value, Attribute *) = 0;
    virtual QWidget *createEditor(QWidget *parent, QModelIndex const &, Attribute *) const = 0;
    virtual bool addRow(Attribute *) = 0;
    virtual bool insertRow(int row, Attribute *) = 0;
    virtual bool deleteRow(int row, Attribute *) = 0;
    virtual void tagFile(Attribute *from, Attribute *to, int index) = 0;
    virtual void tagFile(Attribute *from, Attribute *to) = 0;
    virtual void commit(Attribute *) = 0;
    virtual ~Adapter() = default;
};

namespace listattributeeditor {

template<typename T>
struct Adapter : public ListAttributeEditor::Adapter {
    virtual inline int size(Attribute const *attribute) const override {
        auto att = dynamic_cast<ListAttribute<T> const *>(attribute);
        if (!att)
            return 0;
        return att->size();
    }

    virtual inline QVariant const value(int row,
                                        int column,
                                        Attribute const *attribute) const override {
        if (column != 0)
            return QVariant();
        if (row < 0 || row >= size(attribute))
            return QVariant();
        auto att = dynamic_cast<ListAttribute<T> const *>(attribute);
        if (!att)
            return QVariant();
        return att->at(row);
    }

    virtual void setValue(int row, int column, QVariant const &value, Attribute *attribute) override {
        if (column != 0)
            return;
        if (row < 0 || row >= size(attribute))
            return;
        auto att = dynamic_cast<ListAttribute<T> *>(attribute);
        if (!att)
            return;
        if (att->at(row) == valueValue(value, attribute))
            return;
        att->removeValue(row);
        att->insertValue(valueValue(value, attribute), row);
    }

    virtual QWidget *createEditor(QWidget *parent,
                                  QModelIndex const &,
                                  Attribute *attribute) const override {
        auto att = dynamic_cast<ListAttribute<T> *>(attribute);
        if (!att)
            return nullptr;
        if (att->selector())
            return createComboBox(parent, att->validator(), att->selector());
        else
            return createLineEdit(parent, att->validator());
        return nullptr;
    }

    virtual bool addRow(Attribute *attribute) override {
        auto att = dynamic_cast<ListAttribute<T> *>(attribute);
        if (!att)
            return false;
        att->addValue(T());
        return true;
    }

    virtual bool insertRow(int row, Attribute *attribute) override {
        auto att = dynamic_cast<ListAttribute<T> *>(attribute);
        if (!att)
            return false;
        att->insertValue(T(), row);
        return true;
    }

    virtual bool deleteRow(int row, Attribute *attribute) override {
        auto att = dynamic_cast<ListAttribute<T> *>(attribute);
        if (!att)
            return false;
        att->removeValue(row);
        return true;
    }

    virtual void tagFile(Attribute *from, Attribute *to, int index) override {
        auto f = safeCast<ListAttribute<T>>(from);
        auto t = safeCast<ListAttribute<T>>(to);
        auto value = f->at(index);
        t->addValue(value);
    }

    virtual void tagFile(Attribute *from, Attribute *to) override {
        auto f = safeCast<ListAttribute<T>>(from);
        auto t = safeCast<ListAttribute<T>>(to);
        t->clear();
        for (auto v = f->begin(); v != f->end(); ++v) {
            t->addValue(*v);
        }
    }

    virtual T valueValue(QVariant const &value, Attribute *attribute) const = 0;

    QLineEdit *createLineEdit(QWidget *parent, QValidator *validator) const {
        auto le = make_unique<ExpandingLineEdit>(parent);
        le->setFrame(le->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, le.get()));
        if (!le->style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, 0, le.get()))
            le->setWidgetOwnsGeometry(true);
        le->setValidator(validator);
        return le.release();
    }

    QComboBox *createComboBox(QWidget *parent, QValidator *validator, Selector *selector) const {
        auto cb = make_unique<ExpandingComboBox>(parent);
        cb->setFrame(cb->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, cb.get()));
        cb->setSelector(selector);
        cb->setValidator(validator);
        if (selector->isUserValuesAllowed())
            cb->setEditable(true);
        return cb.release();
    }
};

struct StringAdapter : public Adapter<QString> {
    static StringAdapter *instance() {
        static StringAdapter i;
        return &i;
    }

    virtual void commit(Attribute *attribute) override {
        auto att = dynamic_cast<tag::ListAttribute<QString> *>(attribute);
        if (!att)
            return;
        auto seltr = dynamic_cast<core::PlainSelector *>(att->selector());
        if (!seltr)
            return;
        if (!seltr->isInsertionAllowed())
            return;
        for (auto v : *att) {
            if (!seltr->contains(v))
                seltr->insert(v);
        }
    }

    virtual QString valueValue(QVariant const &value, Attribute *attribute) const override {
        auto att = dynamic_cast<tag::ListAttribute<QString> *>(attribute);
        if (!att)
            return QString();
        return value.toString();
    }
};

} // namespace listattributeeditor

struct ListAttributeEditor::Detail {
    struct Model : public EditableTableModel {
        ListAttributeEditor::Detail *that;

        Model(ListAttributeEditor::Detail *that)
            : that(that) {}

        virtual ~Model() = default;

        virtual int rows() const override {
            auto att = that->attribute();
            if (att)
                return that->adapter->size(att);
            return 0;
        }

        virtual int columns() const override { return 1; }

        virtual QImage const *image(int, int) const override { return nullptr; }

        virtual QVariant const value(int row, int column) const override {
            return that->value(row, column);
        }

        virtual void setValue(int row, int column, QVariant const &value) override {
            auto att = that->attribute();
            if (att && row < that->adapter->size(att))
                that->adapter->setValue(row, column, value, att);
        }

        virtual QString columnTitle(int column) const override {
            if (column == 0)
                return that->that->type()->longUpper();
            return QString();
        }

        virtual void closeEditor(QWidget *editor) override {
            auto cb = dynamic_cast<ui::ExpandingComboBox *>(editor);
            if (cb)
                cb->close();
            auto le = dynamic_cast<ui::ExpandingLineEdit *>(editor);
            if (le)
                le->close();
        }

        virtual bool addRow() override {
            auto att = that->attribute();
            if (att)
                return that->adapter->addRow(att);
            return false;
        }

        virtual bool insertRow(int row) override {
            auto att = that->attribute();
            if (att)
                return that->adapter->insertRow(row, att);
            return false;
        }

        virtual bool deleteRow(int row) override {
            auto att = that->attribute();
            if (att)
                return that->adapter->deleteRow(row, att);
            return false;
        }
    };

    struct ItemDelegate : public ui::ItemDelegate {
        Attribute *attribute = nullptr;
        Adapter *adapter;

        ItemDelegate(Adapter *adapter)
            : adapter(adapter) {}

        void setAttribute(Attribute *attribute) { this->attribute = attribute; }

        virtual void setEditorData(QWidget *editor, QModelIndex const &index) const override {
            QString value = index.model()->data(index, Qt::EditRole).toString();
            auto cb = dynamic_cast<QComboBox *>(editor);
            if (cb) {
                cb->setCurrentText(value);
                return;
            }
            auto le = dynamic_cast<QLineEdit *>(editor);
            if (le)
                le->setText(value);
        }

        virtual void updateEditorGeometry(QWidget *editor,
                                          QStyleOptionViewItem const &option,
                                          QModelIndex const &) const override {
            editor->setGeometry(option.rect);
        }

        virtual void setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  QModelIndex const &index) const override {
            auto cb = dynamic_cast<QComboBox *>(editor);
            if (cb) {
                model->setData(index, cb->currentText());
                return;
            }
            auto le = dynamic_cast<QLineEdit *>(editor);
            if (le)
                model->setData(index, le->text());
        }

        virtual QWidget *createWatchedEditor(QWidget *parent,
                                             QStyleOptionViewItem const &,
                                             QModelIndex const &index) const override {
            return adapter->createEditor(parent, index, attribute);
        }
    };

    ListAttributeEditor *that;
    QString name;
    Adapter *adapter;
    GuardedPtr<EditableTable> table;
    unique_ptr<Model> model;
    GuardedPtr<Action> actTagContainer, actTagSelected, actCommit, actUndo;
    GuardedPtr<ItemDelegate> delegate;

    Detail(QString const &name, Adapter *adapter)
        : name(name)
        , adapter(adapter) {}

    QWidget *setupUi() {
        table = new EditableTable(name);
        delegate = new ItemDelegate(adapter);
        table->setItemDelegate(delegate.get());
        model = make_unique<Model>(this);
        table->setModel(model.get());
        table->setupUi(delegate.get());
        actTagContainer = new Action(GREU_ICON(tag_container), QObject::tr("Tag container files"));
        actTagSelected = new Action(GREU_ICON(tag_selected), QObject::tr("Tag selected files"));
        actCommit = new Action(GREU_ICON(ok), QObject::tr("Commit changes"));
        actUndo = new Action(GREU_ICON(undo), QObject::tr("Undo changes"));
        table->addAction(actCommit.get());
        table->addAction(actUndo.get());
        table->addAction(actTagContainer.get());
        table->addAction(actTagSelected.get());
        table->addAction(actCommit.get());
        table->addAction(actUndo.get());
        table->addAction(actTagContainer.get());
        table->addAction(actTagSelected.get());
        ui::connect(actCommit.get(), [this] { that->commit(); });
        ui::connect(actUndo.get(), [this] {
            that->commit();
            auto att = attribute();
            if (att) {
                att->undoChanges();
                that->update();
            }
        });
        ui::connect(actTagContainer.get(), [this] {
            that->commit();
            auto att = attribute();
            if (!att)
                return;
            auto file = att->tag()->file();
            if (!file)
                return;
            auto container = GREU_M->fileTable()->fileContainer();
            if (!container)
                return;
            std::vector<tag::File *> files;
            for (auto fi = container->begin(); fi != container->end(); ++fi) {
                if ((*fi) != file)
                    files.push_back(*fi);
            }
            tagFiles(files);
        });
        ui::connect(actTagSelected.get(), [this] {
            that->commit();
            auto files = GREU_M->fileTable()->selectedFiles();
            tagFiles(files);
        });
        update();
        return table.get();
    }

    void update() {
        auto att = attribute();
        if (att) {
            auto container = GREU_M->fileTable()->fileContainer();
            if (container) {
                actTagContainer->setToolTip(
                    QObject::tr("Tag %1 files with selected %2")
                        .arg(container->tagText(), that->type()->pluralLower()));
                actTagContainer->setIcon(container->tagIcon());
            }
            actTagSelected->setToolTip(
                QObject::tr("Tag selected files with selected %2").arg(that->type()->pluralLower()));
        } else {
            actTagContainer->setToolTip("");
            actTagContainer->setIcon(GREU_ICON(tag_container));
            actTagSelected->setToolTip("");
            actTagSelected->setIcon(GREU_ICON(tag_selected));
        }
        table->setEnabled(att);
        delegate->setAttribute(att);
        table->update();
    }

    void commit() {
        delegate->commit();
        auto att = attribute();
        if (att)
            adapter->commit(att);
    }

    void setValue(Attribute *attribute) { adapter->commit(attribute); }

    Attribute *attribute() const {
        auto f = GREU_M->fileEditor()->file();
        if (f)
            return f->tag()->attribute(that->type()->id());
        return nullptr;
    }

    QVariant const value(int row, int column) const {
        auto att = attribute();
        if (att && row < adapter->size(att))
            return adapter->value(row, column, att);
        return QVariant();
    }

    void tagFiles(vector<File *> &files) {
        auto file = GREU_M->fileEditor()->file();
        if (file) {
            auto from = attribute();
            if (from) {
                auto rows = table->table()->selectionModel()->selectedRows();
                if (rows.size() == 0) {
                    if (askOverwriteTableAttribute(*that->type())) {
                        for (auto fi : files) {
                            auto to = fi->tag()->attribute(that->type()->id());
                            if (to) {
                                adapter->tagFile(from, to);
                            }
                        }
                    }
                    return;
                }
                for (auto r : rows) {
                    for (auto fi : files) {
                        auto to = fi->tag()->attribute(that->type()->id());
                        if (to)
                            adapter->tagFile(from, to, r.row());
                    }
                }
            }
        }
    }
};

ListAttributeEditor::ListAttributeEditor(AttributeType const *type,
                                         QString const &name,
                                         Adapter *adapter)
    : AttributeEditor(type)
    , d(GREU_MKDET(name, adapter)) {
    d->that = this;
}

ListAttributeEditor::Adapter *ListAttributeEditor::stringAdapter() {
    return listattributeeditor::StringAdapter::instance();
}

ListAttributeEditor::Adapter *ListAttributeEditor::adapter() const {
    return d->adapter;
}

QWidget *ListAttributeEditor::setupUi() {
    return d->setupUi();
}

void ListAttributeEditor::update() {
    d->update();
}

void ListAttributeEditor::commit() {
    d->commit();
}

void ListAttributeEditor::setValue(tag::Attribute *attribute) {
    d->setValue(attribute);
}

} // namespace greu::grylizo

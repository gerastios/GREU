#include "KeyedListAttributeEditor.h"
#include <QAbstractTableModel>
#include <QClipboard>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QMimeData>
#include "Application.h"
#include "Dispatcher.h"
#include "FileEditor.h"
#include "FileTable.h"
#include "MainWindow.h"
#include "Settings.h"
#include "greucore/include/Log.h"
#include "greucore/include/PlainSelector.h"
#include "greucore/include/hexUtils.h"
#include "greucore/include/utils.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AttributeTypes.h"
#include "greutag/include/File.h"
#include "greutag/include/FileContainer.h"
#include "greutag/include/ImagesAttribute.h"
#include "greutag/include/KeyedListAttribute.h"
#include "greutag/include/Tag.h"
#include "greuui/include/Action.h"
#include "greuui/include/EditableTable.h"
#include "greuui/include/EditableTableModel.h"
#include "greuui/include/ExpandingComboBox.h"
#include "greuui/include/ExpandingLineEdit.h"
#include "greuui/include/HexEdit.h"
#include "greuui/include/Icons.h"
#include "greuui/include/ImageTool.h"
#include "greuui/include/ItemDelegate.h"
#include "greuui/include/TableView.h"
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct KeyedListAttributeEditor::Adapter {
    virtual void setAttribute(Attribute *) = 0;
    virtual bool isDescriptionDependent(Attribute const *) const = 0;
    virtual size_t size(Attribute const *) const = 0;
    virtual QImage const *image(int row, int column, Attribute const *) const = 0;
    virtual QVariant const value(int row, int column, Attribute const *) const = 0;
    virtual void setValue(int row, int column, QVariant const &value, Attribute *) = 0;
    virtual QWidget *createEditor(QWidget *parent, QModelIndex const &index, Attribute *) const = 0;
    virtual bool addRow(Attribute *) = 0;
    virtual bool insertRow(int row, Attribute *) = 0;
    virtual bool deleteRow(int row, Attribute *) = 0;
    virtual void commit(Attribute *) = 0;
    virtual Attribute::ErrorType tagFile(Attribute *from,
                                         Attribute *to,
                                         int index,
                                         bool forceOverwrite)
        = 0;
    virtual void tagFile(Attribute *from, Attribute *to) = 0;
    virtual TableView::ContextActionRequestFunc contextActionRequest(Attribute *) = 0;
    virtual ~Adapter() = default;
};

namespace {

template<typename K, typename V>
struct Adapter : public KeyedListAttributeEditor::Adapter {
    typename KeyedListAttribute<K, V>::ValueType values;

    virtual K keyValue(QVariant const &value, Attribute *) const = 0;
    virtual V valueValue(QVariant const &value, Attribute *) const = 0;
    virtual void insertKeyInSelector(Selector *, K const &) {}
    virtual QString keyText(KeyedListAttribute<K, V> const *attribute,
                            typename KeyedListAttribute<K, V>::SingleValue const &value)
        = 0;
    virtual QImage const *image(int, int, Attribute const *) const override { return nullptr; }
    virtual TableView::ContextActionRequestFunc contextActionRequest(Attribute *) override {
        return nullptr;
    }

    virtual void setAttribute(Attribute *attribute) override {
        values.clear();
        if (attribute) {
            auto att = safeCast<tag::KeyedListAttribute<K, V>>(attribute);
            for (auto c = att->begin(); c != att->end(); c++) {
                values.push_back(*c);
            }
        }
    }

    virtual bool isDescriptionDependent(Attribute const *attribute) const override {
        if (!attribute)
            return true;
        return safeCast<tag::KeyedListAttribute<K, V> const>(attribute)->isDescriptionDependent();
    }

    virtual size_t size(Attribute const *attribute) const override {
        if (!attribute)
            return 0;
        return values.size();
    }

    virtual void setValue(int row, int column, QVariant const &value, Attribute *attribute) override {
        auto att = dynamic_cast<tag::KeyedListAttribute<K, V> *>(attribute);
        if (!att)
            return;
        if (column == 0 || (column == 1 && att->isDescriptionDependent())) {
            if (att->isDescriptionDependent()) {
                if (column == 0)
                    values[row].key = keyValue(value, attribute);
                else
                    values[row].description = value.toString();
            } else {
                if (column == 0)
                    values[row].key = keyValue(value, attribute);
            }
        } else if (column == 2 || (column == 1 && !att->isDescriptionDependent())) {
            values[row].value = valueValue(value, attribute);
        }
    }

    virtual QWidget *createEditor(QWidget *parent,
                                  QModelIndex const &index,
                                  Attribute *attribute) const override {
        auto att = dynamic_cast<tag::KeyedListAttribute<K, V> *>(attribute);
        if (!att)
            return nullptr;
        auto av = values[index.row()];
        switch (index.column()) {
        case 0:
            if (att->keySelector())
                return createComboBox(parent, att->keyValidator(), att->keySelector());
            return createLineEdit(parent, att->keyValidator());
        case 1:
            if (att->isDescriptionDependent()) {
                auto kc = att->keyConstraint(av.key);
                if (kc.isDescriptionDependent)
                    return createLineEdit(parent, nullptr);
                return nullptr;
            }
            if (att->valueSelector())
                return createComboBox(parent, att->valueValidator(), att->valueSelector());
            return createLineEdit(parent, att->valueValidator());
        case 2:
            if (att->valueSelector())
                return createComboBox(parent, att->valueValidator(), att->valueSelector());
            return createLineEdit(parent, att->valueValidator());
        }
        return nullptr;
    }

    virtual bool addRow(Attribute *attribute) override {
        auto att = dynamic_cast<tag::KeyedListAttribute<K, V> *>(attribute);
        if (!att)
            return false;
        auto dv = att->defaultValue();
        values.push_back(dv);
        return true;
    }

    virtual bool insertRow(int row, Attribute *attribute) override {
        auto att = dynamic_cast<tag::KeyedListAttribute<K, V> *>(attribute);
        if (!att)
            return false;
        auto dv = att->defaultValue();
        auto it = values.begin();
        std::advance(it, row);
        values.insert(it, dv);
        return true;
    }

    virtual bool deleteRow(int row, Attribute *attribute) override {
        auto att = dynamic_cast<tag::KeyedListAttribute<K, V> *>(attribute);
        if (!att)
            return false;
        auto it = values.begin();
        std::advance(it, row);
        values.erase(it);
        return true;
    }

    virtual void commit(Attribute *attribute) override {
        auto att = dynamic_cast<KeyedListAttribute<K, V> *>(attribute);
        if (!att)
            return;
        Attribute::ErrorType rc = Attribute::ErrorType::NoError;
        int i = 0;
        int max = att->size();
        for (auto v : values) {
            if (att->isDescriptionDependent() || att->keyConstraint(v.key).isDescriptionDependent) {
                if (i >= max)
                    rc = att->addValue(v.key, v.description, v.value, false);
                else
                    rc = att->modifyValue(v.key, v.description, v.value, i);
            } else {
                if (i >= max)
                    rc = att->addValue(v.key, v.value, false);
                else
                    rc = att->modifyValue(v.key, v.value, i);
            }
            if (rc != Attribute::ErrorType::NoError)
                GREU_D->log().addWarn(att->errorText(rc, keyText(att, v)));
            i++;
            insertKeyInSelector(att->keySelector(), v.key);
        }
        if (i < max) {
            for (int k = i; k < max; k++) {
                auto v = att->at(k);
                if (att->isDescriptionDependent()
                    || att->keyConstraint(v.key).isDescriptionDependent) {
                    rc = att->removeValue(v.key, v.description, v.value);
                } else {
                    rc = att->removeValue(v.key, v.value);
                }
                if (rc != Attribute::ErrorType::NoError)
                    GREU_D->log().addWarn(att->errorText(rc, keyText(att, v)));
                max = att->size();
            }
        }
    }

    virtual Attribute::ErrorType tagFile(Attribute *from,
                                         Attribute *to,
                                         int index,
                                         bool forceOverwrite) override {
        auto f = safeCast<tag::KeyedListAttribute<K, V>>(from);
        auto t = safeCast<tag::KeyedListAttribute<K, V>>(to);
        if ((size_t) index + 1 > f->size())
            return tag::Attribute::ErrorType::EntryNotFound;
        auto value = f->at(index);
        tag::Attribute::ErrorType rc = tag::Attribute::ErrorType::NoError;
        if (t->isDescriptionDependent() || t->keyConstraint(value.key).isDescriptionDependent) {
            rc = t->addValue(value.key, value.description, value.value, forceOverwrite);
        } else {
            rc = t->addValue(value.key, value.value, forceOverwrite);
        }
        return rc;
    }

    virtual void tagFile(Attribute *from, Attribute *to) override { to->copyValue(*from); }

    ui::ExpandingLineEdit *createLineEdit(QWidget *parent, QValidator *validator) const {
        auto le = std::make_unique<ExpandingLineEdit>(parent);
        le->setFrame(le->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, le.get()));
        if (!le->style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, 0, le.get()))
            le->setWidgetOwnsGeometry(true);
        le->setValidator(validator);
        return le.release();
    }

    greu::ui::ExpandingComboBox *createComboBox(QWidget *parent,
                                                QValidator *validator,
                                                Selector *selector) const {
        auto cb = std::make_unique<ExpandingComboBox>(parent);
        cb->setFrame(cb->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, cb.get()));
        cb->setSelector(selector);
        cb->setValidator(validator);
        return cb.release();
    }
};

struct ImagesAdapter : public Adapter<uint, QImage> {
    GuardedPtr<Action> actLoadFromFile, actLoadFromClipboard, actSaveToClipboard, actEdit;
    GuardedPtr<TableView> table;

    ImagesAdapter() {
        actLoadFromFile = new Action(QObject::tr("Load from file"));
        actLoadFromClipboard = new Action(QObject::tr("Load from clipboard"));
        actSaveToClipboard = new Action(QObject::tr("Save to clipboard"));
        actEdit = new Action(QObject::tr("Edit image"));
    }

    virtual uint keyValue(QVariant const &value, Attribute *attribute) const override {
        auto att = safeCast<tag::ImagesAttribute const>(attribute);
        return att->keySelector()->idOf(value.toString());
    }

    virtual QImage valueValue(QVariant const &, Attribute *) const override { return QImage(); }

    virtual void setValue(int row, int column, QVariant const &value, Attribute *attribute) override {
        if (!attribute)
            return;
        auto att = safeCast<tag::ImagesAttribute const>(attribute);
        if ((column == 1 && !att->isDescriptionDependent()) || column == 2)
            return;
        Adapter<uint, QImage>::setValue(row, column, value, attribute);
    }

    virtual QImage const *image(int row, int column, Attribute const *attribute) const override {
        auto att = dynamic_cast<tag::ImagesAttribute const *>(attribute);
        if (!att)
            return nullptr;
        if ((column != 2 && att->isDescriptionDependent())
            || (column != 1 && !att->isDescriptionDependent()))
            return nullptr;
        return &values[row].value;
    }

    virtual QVariant const value(int row, int column, Attribute const *attribute) const override {
        auto att = dynamic_cast<tag::ImagesAttribute const *>(attribute);
        if (!att)
            return QVariant();
        auto av = values[row];
        switch (column) {
        case 0:
            return att->keySelector()->value(av.key);
        case 1:
            if (att->isDescriptionDependent()) {
                if (att->keyConstraint(av.key).isDescriptionDependent)
                    return av.description;
            }
            break;
        }
        return QVariant();
    }

    virtual QWidget *createEditor(QWidget *parent,
                                  QModelIndex const &index,
                                  Attribute *attribute) const override {
        if ((index.column() == 2 && isDescriptionDependent(attribute))
            || (index.column() == 1 && !isDescriptionDependent(attribute)))
            return nullptr;
        return Adapter<uint, QImage>::createEditor(parent, index, attribute);
    }

    virtual TableView::ContextActionRequestFunc contextActionRequest(Attribute *attribute) override {
        return [this, attribute](int row, int column) -> std::vector<ui::TableView::ContextAction> {
            auto att = safeCast<tag::ImagesAttribute>(attribute);
            if (column != 2 && (att->isDescriptionDependent() || column != 1))
                return std::vector<ui::TableView::ContextAction>();
            std::vector<ui::TableView::ContextAction>
                actions{{actLoadFromClipboard.get(),
                         [this](int row, int, QAbstractTableModel *) {
                             auto cl = Application::clipboard();
                             cl->ownsClipboard();
                             if (!cl->mimeData()->hasImage())
                                 return;
                             auto image = cl->image();
                             if (image.isNull())
                                 return;
                             values[(uint) row].value = image;
                         }},
                        {actLoadFromFile.get(),
                         [this](int row, int, QAbstractTableModel *) {
                             auto fileName = QFileDialog::getOpenFileName(
                                 nullptr,
                                 QObject::tr("Open Image"),
                                 QObject::tr("Images (*.png *.xpm *.jpg);;All files (*.*)"));
                             if (fileName.isEmpty())
                                 return;
                             values[row].value = QImage(fileName);
                         }},
                        {actSaveToClipboard.get(),
                         [this](int row, int, QAbstractTableModel *) {
                             if (values[row].value.isNull())
                                 return;
                             QGuiApplication::clipboard()->setImage(values[row].value);
                         }},
                        {actEdit.get(), [this](int row, int, QAbstractTableModel *) {
                             if (values[row].value.isNull())
                                 return;
                             auto tool = new ImageTool(values[row].value,
                                                       values[row].description,
                                                       Settings::instance()->quadratLength());
                             if (tool->exec())
                                 values[row].value = tool->image();
                         }}};
            auto enabled = !(values[row].value.isNull());
            actEdit->setEnabled(enabled);
            actSaveToClipboard->setEnabled(enabled);
            return actions;
        };
    }

    virtual QString keyText(
        KeyedListAttribute<uint, QImage> const *attribute,
        typename KeyedListAttribute<uint, QImage>::SingleValue const &value) override {
        if (attribute->isDescriptionDependent()
            || attribute->keyConstraint(value.key).isDescriptionDependent) {
            return QObject::tr("%1/%2")
                .arg(attribute->keySelector() ? attribute->keySelector()->value(value.key)
                                              : QString("%1").arg(value.key))
                .arg(value.description);
        } else {
            return QObject::tr("%1").arg(attribute->keySelector()
                                             ? attribute->keySelector()->value(value.key)
                                             : QString("%1").arg(value.key));
        }
    }
};

struct StringBytesAdapter : public Adapter<QString, QByteArray> {
    virtual QString keyValue(QVariant const &value, Attribute *) const override {
        return value.toString();
    }

    virtual QByteArray valueValue(QVariant const &value, Attribute *) const override {
        return core::bytes(value.toString());
    }

    virtual QVariant const value(int row, int column, Attribute const *attribute) const override {
        auto att = dynamic_cast<tag::AttributeTypes::StringKeyedListBytesAttribute const *>(
            attribute);
        if (!att)
            return QVariant();
        auto av = values[row];
        switch (column) {
        case 0:
            return av.key;
        case 1:
            if (att->isDescriptionDependent()) {
                if (att->keyConstraint(av.key).isDescriptionDependent)
                    return av.description;
                else
                    return QVariant();
            } else {
                return core::formatInput(core::string(av.value));
            }
            break;
        case 2:
            return av.value;
        }
        return QVariant();
    }

    virtual QWidget *createEditor(QWidget *parent,
                                  QModelIndex const &index,
                                  Attribute *attribute) const override {
        if ((index.column() == 1 && isDescriptionDependent(attribute)) || (index.column() == 0))
            return Adapter<QString, QByteArray>::createEditor(parent, index, attribute);
        auto att = safeCast<tag::AttributeTypes::StringKeyedListBytesAttribute>(attribute);
        return createHexEditor(att->validator());
    }

    virtual QString keyText(
        KeyedListAttribute<QString, QByteArray> const *attribute,
        typename KeyedListAttribute<QString, QByteArray>::SingleValue const &value) override {
        if (attribute->isDescriptionDependent()
            || attribute->keyConstraint(value.key).isDescriptionDependent) {
            return QObject::tr("%1/%2").arg(value.key, value.description);
        } else {
            return QObject::tr("%1").arg(value.key);
        }
    }

    HexEdit *createHexEditor(QValidator *validator) const {
        auto he = std::make_unique<HexEdit>();
        he->setFrame(he->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, he.get()));
        he->setValidator(validator);
        return he.release();
    }
};

struct StringStringAdapter : public Adapter<QString, QString> {
    virtual QString keyValue(QVariant const &value, Attribute *) const override {
        return value.toString();
    }

    virtual QString valueValue(QVariant const &value, Attribute *) const override {
        return value.toString();
    }

    virtual void insertKeyInSelector(Selector *selector, QString const &key) override {
        if (!selector || !selector->isInsertionAllowed() || selector->contains(key))
            return;
        auto s = dynamic_cast<core::PlainSelector *>(selector);
        if (s)
            s->insert(key);
    }

    virtual QVariant const value(int row, int column, Attribute const *attribute) const override {
        auto att = dynamic_cast<tag::AttributeTypes::StringKeyedListStringAttribute const *>(
            attribute);
        if (!att)
            return QVariant();
        auto av = values[row];
        switch (column) {
        case 0:
            return av.key;
        case 1:
            if (att->isDescriptionDependent()) {
                if (att->keyConstraint(av.key).isDescriptionDependent)
                    return av.description;
                else
                    return QVariant();
            } else {
                return av.value;
            }
            break;
        case 2:
            return av.value;
        }
        return QVariant();
    }

    virtual QString keyText(
        KeyedListAttribute<QString, QString> const *attribute,
        typename KeyedListAttribute<QString, QString>::SingleValue const &value) override {
        if (attribute->isDescriptionDependent()
            || attribute->keyConstraint(value.key).isDescriptionDependent) {
            return QObject::tr("%1/%2").arg(value.key, value.description);
        } else {
            return QObject::tr("%1").arg(value.key);
        }
    }
};

struct UintStringAdapter : public Adapter<uint, QString> {
    virtual uint keyValue(QVariant const &value, Attribute *attribute) const override {
        auto att = safeCast<tag::AttributeTypes::UintKeyedListStringAttribute const>(attribute);
        if (att->keySelector())
            return att->keySelector()->idOf(value.toString());
        return value.toInt();
    }

    virtual QString valueValue(QVariant const &value, Attribute *) const override {
        return value.toString();
    }

    virtual QVariant const value(int row, int column, Attribute const *attribute) const override {
        auto att = dynamic_cast<tag::AttributeTypes::UintKeyedListStringAttribute const *>(
            attribute);
        if (!att)
            return QVariant();
        auto av = values[row];
        switch (column) {
        case 0:
            if (att->keySelector()) {
                auto v = att->keySelector()->value(av.key);
                return v;
            }
            return av.key;
        case 1:
            if (att->isDescriptionDependent()) {
                if (att->keyConstraint(av.key).isDescriptionDependent)
                    return av.description;
                else
                    return QVariant();
            } else {
                return av.value;
            }
            break;
        case 2:
            return av.value;
        }
        return QVariant();
    }

    virtual QString keyText(
        KeyedListAttribute<uint, QString> const *attribute,
        typename KeyedListAttribute<uint, QString>::SingleValue const &value) override {
        if (attribute->isDescriptionDependent()
            || attribute->keyConstraint(value.key).isDescriptionDependent) {
            return QObject::tr("%1/%2")
                .arg(attribute->keySelector() ? attribute->keySelector()->value(value.key)
                                              : QString("%1").arg(value.key))
                .arg(value.description);
        } else {
            return QObject::tr("%1").arg(attribute->keySelector()
                                             ? attribute->keySelector()->value(value.key)
                                             : QString("%1").arg(value.key));
        }
    }
};

struct UintUintAdapter : public Adapter<uint, uint> {
    virtual uint keyValue(QVariant const &value, Attribute *attribute) const override {
        auto att = safeCast<tag::AttributeTypes::UintKeyedListUintAttribute const>(attribute);
        if (att->keySelector())
            return att->keySelector()->idOf(value.toString());
        return value.toInt();
    }

    virtual uint valueValue(QVariant const &value, Attribute *) const override {
        return value.toInt();
    }

    virtual QVariant const value(int row, int column, Attribute const *attribute) const override {
        auto att = dynamic_cast<tag::AttributeTypes::UintKeyedListUintAttribute const *>(attribute);
        if (!att)
            return QVariant();
        auto av = values[row];
        switch (column) {
        case 0:
            if (att->keySelector()) {
                auto v = att->keySelector()->value(av.key);
                return v;
            }
            return av.key;
        case 1:
            if (att->isDescriptionDependent()) {
                if (att->keyConstraint(av.key).isDescriptionDependent)
                    return av.description;
                else
                    return QVariant();
            } else {
                return av.value;
            }
            break;
        case 2:
            return av.value;
        }
        return QVariant();
    }

    virtual QString keyText(
        KeyedListAttribute<uint, uint> const *attribute,
        typename KeyedListAttribute<uint, uint>::SingleValue const &value) override {
        if (attribute->isDescriptionDependent()
            || attribute->keyConstraint(value.key).isDescriptionDependent) {
            return QObject::tr("%1/%2").arg(attribute->keySelector()
                                                ? attribute->keySelector()->value(value.key)
                                                : QString("%1").arg(value.key),
                                            value.description);
        } else {
            return QObject::tr("%1").arg(attribute->keySelector()
                                             ? attribute->keySelector()->value(value.key)
                                             : QString("%1").arg(value.key));
        }
    }
};

} // namespace

struct KeyedListAttributeEditor::Detail {
    struct Model : public EditableTableModel {
        KeyedListAttributeEditor::Detail *const that;

        Model(KeyedListAttributeEditor::Detail *that)
            : that(that) {}

        virtual int rows() const override {
            auto att = that->that->attribute();
            if (att)
                return that->adapter->size(att);
            return 0;
        }

        virtual int columns() const override {
            auto att = that->that->attribute();
            if (att)
                return that->adapter->isDescriptionDependent(att) ? 3 : 2;
            return 3;
        }

        virtual QImage const *image(int row, int column) const override {
            auto att = that->that->attribute();
            if (att && (size_t) row < that->adapter->size(att))
                return that->adapter->image(row, column, att);
            return nullptr;
        }

        virtual QVariant const value(int row, int column) const override {
            auto att = that->that->attribute();
            if (att && (size_t) row < that->adapter->size(att))
                return that->adapter->value(row, column, att);
            return QVariant();
        }

        virtual void setValue(int row, int column, QVariant const &value) override {
            auto att = that->that->attribute();
            if (att && (size_t) row < that->adapter->size(att))
                that->adapter->setValue(row, column, value, att);
        }

        virtual QString columnTitle(int column) const override {
            auto att = that->that->attribute();
            switch (column) {
            case 0:
                return that->that->type()->keyUpper();
            case 1:
                if (att && that->adapter->isDescriptionDependent(att))
                    return QObject::tr("Description");
                else
                    return that->that->type()->longUpper();
            case 2:
                if (att && that->adapter->isDescriptionDependent(att))
                    return that->that->type()->longUpper();
            }
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
            auto att = that->that->attribute();
            if (att)
                return that->adapter->addRow(att);
            return false;
        }

        virtual bool insertRow(int row) override {
            auto att = that->that->attribute();
            if (att)
                return that->adapter->insertRow(row, att);
            return false;
        }

        virtual bool deleteRow(int row) override {
            auto att = that->that->attribute();
            if (att)
                return that->adapter->deleteRow(row, att);
            return false;
        }
    };

    struct ItemDelegate : public ui::ItemDelegate {
        Attribute *attribute = nullptr;
        KeyedListAttributeEditor::Detail *const that;

        ItemDelegate(KeyedListAttributeEditor::Detail *that)
            : that(that) {}

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

        virtual void updateEditorGeometry(QWidget *editor,
                                          QStyleOptionViewItem const &option,
                                          QModelIndex const &) const override {
            editor->setGeometry(option.rect);
        }

        virtual QWidget *createWatchedEditor(QWidget *parent,
                                             QStyleOptionViewItem const &,
                                             QModelIndex const &index) const override {
            return that->adapter->createEditor(parent, index, attribute);
        }
    };

    KeyedListAttributeEditor *that;
    QString const name;
    Model model;
    unique_ptr<Adapter> adapter;
    GuardedPtr<ItemDelegate> delegate;
    GuardedPtr<EditableTable> table;
    AdapterCreator *creator;
    GuardedPtr<Action> actTagContainer, actTagSelected, actCommit, actUndo;

    Detail(QString const &name, AdapterCreator *c)
        : name(name)
        , model(this)
        , creator(c) {}

    QWidget *setupUi() {
        table = new EditableTable(name);
        table->setModel(&model);
        delegate = new ItemDelegate(this);
        table->setItemDelegate(delegate.get());
        actTagContainer = new Action(QObject::tr("Tag container files"), GREU_ICON(tag_container));
        actTagSelected = new Action(QObject::tr("Tag selected files"), GREU_ICON(tag_selected));
        actCommit = new Action(QObject::tr("Commit changes"), GREU_ICON(ok));
        actUndo = new Action(QObject::tr("Undo changes"), GREU_ICON(undo));
        table->addAction(actCommit.get());
        table->addAction(actUndo.get());
        table->addAction(actTagContainer.get());
        table->addAction(actTagSelected.get());
        table->setupUi(delegate.get());
        auto cr = creator();
        adapter = std::move(cr);
        ui::connect(actCommit.get(), [this] { that->commit(); });
        ui::connect(actUndo.get(), [this] {
            auto att = that->attribute();
            if (att) {
                att->undoChanges();
                that->update();
            }
        });
        ui::connect(actTagContainer.get(), [this] {
            auto file = GREU_M->fileEditor()->file();
            if (!file)
                return;
            auto container = GREU_M->fileTable()->fileContainer();
            if (!container)
                return;
            that->commit();
            std::vector<tag::File *> files;
            for (auto f = container->begin(); f != container->end(); f++) {
                if (*f != file) {
                    files.push_back(*f);
                }
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
        auto att = that->attribute();
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
        adapter->setAttribute(att);
        delegate->setAttribute(att);
        table->table()->clearContextActionRequests();
        auto ctxActReq = adapter->contextActionRequest(att);
        if (ctxActReq)
            table->table()->addContextActionRequest(ctxActReq);
        table->update();
    }

    void commit() {
        delegate->commit();
        auto att = that->attribute();
        if (att)
            adapter->commit(att);
        update();
    }

    void setValue(Attribute *attribute) { adapter->commit(attribute); }

    void tagFiles(vector<tag::File *> const &files) {
        auto file = GREU_M->fileEditor()->file();
        if (!file)
            return;
        auto from = that->attribute();
        if (!from)
            return;
        auto rows = table->table()->selectionModel()->selectedRows();
        auto type = that->type();
        if (rows.size() == 0) {
            if (askOverwriteTableAttribute(*type)) {
                for (auto f : files) {
                    auto to = f->tag()->attribute(type->id());
                    adapter->tagFile(from, to);
                }
            }
            return;
        }
        auto forceOverwriteAll = false;
        auto forceNone = false;
        for (auto r : rows) {
            for (auto f : files) {
                auto to = f->tag()->attribute(type->id());
                if ((f != file) && to) {
                    auto done = false;
                    auto forceOverwrite = false;
                    do {
                        auto rc = adapter->tagFile(from,
                                                   to,
                                                   r.row(),
                                                   forceOverwrite | forceOverwriteAll);
                        switch (rc) {
                        case tag::Attribute::ErrorType::NoError:
                            done = true;
                            break;
                        case tag::Attribute::ErrorType::DuplicateKey:
                            if (forceNone) {
                                done = true;
                            } else {
                                auto answer = QMessageBox::question(
                                    table->table(),
                                    QObject::tr("%1 already exists").arg(type->shortUpper()),
                                    QObject::tr("The objects already exists in the target "
                                                "file.\nDo you want to overwrite?"),
                                    QMessageBox::StandardButtons(
                                        QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No
                                        | QMessageBox::NoToAll | QMessageBox::Cancel),
                                    QMessageBox::Yes);
                                switch (answer) {
                                case QMessageBox::Yes:
                                    forceOverwrite = true;
                                    break;
                                case QMessageBox::YesToAll:
                                    forceOverwriteAll = true;
                                    break;
                                case QMessageBox::No:
                                    done = true;
                                    break;
                                case QMessageBox::NoToAll:
                                    done = true;
                                    forceNone = true;
                                    break;
                                case QMessageBox::Cancel:
                                    return;
                                default:
                                    done = true;
                                }
                            }
                            break;
                        default:
                            QMessageBox::warning(table->table(),
                                                 QObject::tr("Error during copying"),
                                                 to->errorText(rc, type->shortLower()));
                        }
                    } while (!done);
                }
            }
        }
    }
};

KeyedListAttributeEditor::KeyedListAttributeEditor(AttributeType const *type,
                                                   QString const &name,
                                                   AdapterCreator *creator)
    : AttributeEditor(type)
    , d(GREU_MKDET(name, creator)) {
    d->that = this;
}

unique_ptr<KeyedListAttributeEditor::Adapter> KeyedListAttributeEditor::imagesAdapter() {
    return make_unique<ImagesAdapter>();
}

unique_ptr<KeyedListAttributeEditor::Adapter> KeyedListAttributeEditor::stringBytesAdapter() {
    return make_unique<StringBytesAdapter>();
}

unique_ptr<KeyedListAttributeEditor::Adapter> KeyedListAttributeEditor::stringStringAdapter() {
    return make_unique<StringStringAdapter>();
}

unique_ptr<KeyedListAttributeEditor::Adapter> KeyedListAttributeEditor::uintStringAdapter() {
    return make_unique<UintStringAdapter>();
}

unique_ptr<KeyedListAttributeEditor::Adapter> KeyedListAttributeEditor::uintUintAdapter() {
    return make_unique<UintUintAdapter>();
}

QWidget *KeyedListAttributeEditor::setupUi() {
    return d->setupUi();
}

void KeyedListAttributeEditor::update() {
    return d->update();
}

void KeyedListAttributeEditor::commit() {
    d->commit();
}

void KeyedListAttributeEditor::setValue(Attribute *attribute) {
    d->setValue(attribute);
}

} // namespace greu::grylizo

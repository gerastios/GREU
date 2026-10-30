#include "DateTimeAttributeEditor.h"
#include "Settings.h"
#include "greucore/include/utils.h"
#include "greutag/include/DateTimeAttribute.h"
#include "greuui/include/DateTimeEdit.h"
#include "include/GuardedPtr.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct DateTimeAttributeEditor::Detail {
    GuardedPtr<DateTimeEdit> editor;
    ScopedConnection connDate, connTime;

    void setEditEnabled(bool value, DateTimeAttributeEditor *that) {
        if (value) {
            editor->setEnabled(true);
            auto a = that->attribute();
            if (a) {
                auto att = safeCast<DateTimeAttribute>(a);
                editor->setDisplayFormat(determineDisplayFormat(att));
                auto v = att->value();
                if (v.isValid() && !v.isNull())
                    editor->setValue(att->value());
                else
                    editor->setValue(QDateTime());
            }
        } else {
            editor->setValue(QDateTime());
            editor->setEnabled(false);
        }
    }

    void setValue(Attribute *attribute) {
        auto att = dynamic_cast<DateTimeAttribute *>(attribute);
        if (att)
            att->setValue(editor->value());
    }

    static QString determineDateFormat(int const format) {
        switch (format) {
        case Settings::DateFormat::ADateW:
            break;
        case Settings::DateFormat::EDateW:
            return "dd.MM.yyyy";
        case Settings::DateFormat::SDateW:
            return "yyyy/MM/dd";
        case Settings::DateFormat::iso:
            return "yyyy-MM-dd";
        }
        return "MM/dd/yyyy";
    }

    QString determineDisplayFormat(DateTimeAttribute *attribute) {
        auto dateFormat = Settings::instance()->dateFormat();
        auto timeFormat = Settings::instance()->timeFormat();
        switch (attribute->precision()) {
        case DateTimeAttribute::Precision::Year:
            return "yyyy";
        case DateTimeAttribute::Precision::Month:
            switch (dateFormat) {
            case Settings::DateFormat::ADateW:
                break;
            case Settings::DateFormat::EDateW:
                return "MM.yyyy";
            case Settings::DateFormat::SDateW:
                return "yyyy/MM";
            case Settings::DateFormat::iso:
                return "yyyy-MM";
            }
            return "MM/yyyy";
        case DateTimeAttribute::Precision::Day:
            return determineDateFormat(dateFormat);
        case DateTimeAttribute::Precision::Hour: {
            QString result = determineDateFormat(dateFormat);
            switch (timeFormat) {
            case Settings::TimeFormat::AmPm:
                result.append(" ").append("hh a");
                break;
            case Settings::TimeFormat::_24:
                result.append(" ").append("hh");
                break;
            }
            return result;
        }
        case tag::DateTimeAttribute::Precision::Minute: {
            QString result = determineDateFormat(dateFormat);
            switch (timeFormat) {
            case Settings::TimeFormat::AmPm:
                result.append(" ").append("hh:mm a");
                break;
            case Settings::TimeFormat::_24:
                result.append(" ").append("hh:mm");
                break;
            }
            return result;
        }
        case tag::DateTimeAttribute::Precision::Second: {
            QString result = determineDateFormat(dateFormat);
            switch (timeFormat) {
            case Settings::TimeFormat::AmPm:
                result.append(" ").append("hh:mm:ss a");
                break;
            case Settings::TimeFormat::_24:
                result.append(" ").append("hh:mm:ss");
                break;
            }
            return result;
        }
        }
        return "";
    }
};

DateTimeAttributeEditor::DateTimeAttributeEditor(AttributeType *type,
                                                 bool hasTagContainer,
                                                 bool hasTagSelected)
    : PlainEditor(type, hasTagContainer, hasTagSelected)
    , d(GREU_MKDET()) {
    d->editor = new DateTimeEdit;
    d->connDate = Settings::instance()->dateFormatChanged().connect(
        [this] { d->setEditEnabled(attribute(), this); });
    d->connTime = Settings::instance()->timeFormatChanged().connect(
        [this] { d->setEditEnabled(attribute(), this); });
}

QWidget *DateTimeAttributeEditor::editor() const {
    return d->editor.get();
}

ui::EditAlerter *DateTimeAttributeEditor::editAlerter() const {
    return d->editor.get();
}

void DateTimeAttributeEditor::setEditEnabled(bool v) {
    d->setEditEnabled(v, this);
}

void DateTimeAttributeEditor::setValue(tag::Attribute *a) {
    d->setValue(a);
}

QLineEdit *DateTimeAttributeEditor::lineEdit() const {
    return d->editor->editor();
}

void DateTimeAttributeEditor::setReadOnly(bool v) {
    d->editor->setReadOnly(v);
}

} // namespace greu::grylizo

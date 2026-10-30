#include "Settings.h"
#include <QVariant>
#include "Application.h"

using namespace std;

namespace greu::grylizo {

using namespace core;

namespace {
QString const keyDateFormat{"DateFormat"};
QString const keyTimeFormat{"TimeFormat"};
QString const keyQuadratLength{"QuadLength"};
QString const keyLastFileName{"LastFileName"};
QString const keyLastFilePath{"LastFilePath"};
QString const keyOverwriteTabAttr{"OverwriteTableAttribute"};
} // namespace

struct Settings::Detail {
    EmptySig dateFormatChanged;
    EmptySig timeFormatChanged;
};

Settings *Settings::instance() {
    static Settings i;
    return &i;
}

EmptySig &Settings::dateFormatChanged() const {
    return d->dateFormatChanged;
}

EmptySig &Settings::timeFormatChanged() const {
    return d->timeFormatChanged;
}

int Settings::dateFormat() const {
    return value(keyDateFormat, DateFormat::ADateW).toInt();
}

void Settings::setDateFormat(int value) {
    if (dateFormat() != value) {
        setValue(keyDateFormat, value);
        d->dateFormatChanged();
    }
}

int Settings::timeFormat() const {
    return value(keyTimeFormat, TimeFormat::_24).toInt();
}

void Settings::setTimeFormat(int value) {
    if (timeFormat() != value) {
        setValue(keyTimeFormat, value);
        d->timeFormatChanged();
    }
}

int Settings::quadratLength() const {
    return value(keyQuadratLength, 400).toInt();
}

void Settings::setQuadratLength(int value) {
    setValue(keyQuadratLength, value);
}

QString Settings::lastFileName() const {
    return value(keyLastFileName).toString();
}

void Settings::setLastFileName(QString value) {
    setValue(keyLastFileName, value);
}

QString Settings::lastFilePath() const {
    return value(keyLastFilePath).toString();
}

void Settings::setLastFilePath(QString value) {
    setValue(keyLastFilePath, value);
}

optional<bool> Settings::overwriteTableAttribute() const {
    if (!contains(keyOverwriteTabAttr))
        return nullopt;
    return value(keyOverwriteTabAttr).toBool();
}

void Settings::setOverwriteTableAttribute(optional<bool> value) {
    if (value.has_value())
        setValue(keyOverwriteTabAttr, value.value());
    else
        remove(keyOverwriteTabAttr);
}

Settings::Settings()
    : core::Settings(Application::instance()->applicationName())
    , d(GREU_MKDET()) {}

} // namespace greu::grylizo

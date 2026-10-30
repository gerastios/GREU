#include "FileNameValidator.h"
#include <QDir>
#include <QRegularExpression>
#include "File.h"

namespace greu::tag {

using namespace core;

struct FileNameValidator::Detail {
    File *file = nullptr;

    State validate(QString &text) const {
        if (!file)
            return QValidator::Acceptable;
        if (text.contains(forbidden()))
            return QValidator::Invalid;
        auto fi = file->info();
        if (text == fi.completeBaseName())
            return QValidator::Acceptable;
        QFileInfo f(QDir(fi.path()), text + "." + file->info().suffix());
        if (f.exists()) {
            return QValidator::Invalid;
        }
        return QValidator::Acceptable;
    }

    static QRegularExpression &forbidden() {
        static QRegularExpression f{
            "[\\x00\\x01\\x02\\x03\\x04\\x05\\x06\\x07\\x08\\x09\\x0A\\x0B\\x0C\\x0D\\x0E\\x0F\\x10"
            "\\x11\\x12\\x13\\x14\\x15\\x16\\x17\\x18\\x19\\x1A\\x1B\\x1C\\x1D\\x1E\\x1F\\x7F\\\"*"
            "\\/\\:\\<\\>\\?\\\\|]"};
        return f;
    }
};

FileNameValidator::FileNameValidator()
    : d(GREU_MKDET()) {}

void FileNameValidator::setFile(File *value) {
    d->file = value;
}

QValidator::State FileNameValidator::validate(QString &text, int &) const {
    return d->validate(text);
}

} // namespace greu::tag

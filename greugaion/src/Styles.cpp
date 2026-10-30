#include "Styles.h"
#include <QApplication>
#include <QScreen>

namespace {
QString const end{"</span>"};
QString const start{"<span style='font-size:%1px; "};
QString const anchor{"<a href=\"%1\">%2</a>"};
QString const bold{"font-weight:bold; "};
QString const italic{"font-style:italic; "};
QString const closeStart{"'>"};
} // namespace

namespace greu::gaion {

using namespace core;

struct Styles::Detail {
    int width, height;
    Detail() {
        auto r = QApplication::primaryScreen()->geometry();
        height = r.height();
        width = r.width();
    }

    QString bold(int div, QString const &s) const {
        return QString(start.arg(width / div) + ::bold + closeStart + "%1" + end).arg(s);
    }
    QString boldLink(int div, QString const &link, QString const &s) const {
        return QString(start.arg(width / div) + ::bold + closeStart + anchor + end).arg(link).arg(s);
    }
    QString link(int div, QString const &link, QString const &s) const {
        return QString(start.arg(width / div) + closeStart + anchor + end).arg(link).arg(s);
    }
    QString italic(int div, QString const &s) const {
        return QString(start.arg(width / div) + ::italic + closeStart + "%1" + end).arg(s);
    }
    QString italicLink(int div, QString const &link, QString const &s) const {
        return QString(start.arg(width / div) + ::italic + closeStart + anchor + end)
            .arg(link)
            .arg(s);
    }
    QString normal(int div, QString const &s) const {
        return QString(start.arg(width / div) + closeStart + "%1" + end).arg(s);
    }
};

Styles *Styles::instance() {
    static Styles i;
    return &i;
}

Styles::Styles()
    : d(GREU_MKDET()) {}

int Styles::width() const {
    return d->width;
}
int Styles::height() const {
    return d->height;
}

QString Styles::title(QString const &s) const {
    return d->bold(64, s);
}
QString Styles::titleLink(QString const &link, QString const &s) const {
    return d->link(64, link, s);
}
QString Styles::titleAddition(QString const &s) const {
    return d->normal(64, s);
}
QString Styles::titleTile(QString const &s) const {
    return d->bold(128, s);
}
QString Styles::subTitleTile(QString const &s) const {
    return d->bold(128, s);
}

QString Styles::job(QString const &s) const {
    return d->italic(137, s);
}

QString Styles::jobLink(QString const &link, QString const &s) const {
    return d->italicLink(137, link, s);
}

QString Styles::titleTileLink(QString const &link, QString const &s) const {
    return d->link(128, link, s);
}

QString Styles::originalTitle(QString const &s) const {
    return d->bold(137, s);
}
QString Styles::originalTitleLabel(QString const &s) const {
    return d->normal(137, s);
}

QString Styles::tagline(QString const &s) const {
    return d->italic(128, s);
}

QString Styles::genresRuntime(QString const &s) const {
    return d->normal(128, s);
}

QString Styles::overview(QString const &s) const {
    return d->normal(128, s);
}

QString Styles::sectionHeader(QString const &s) const {
    return d->bold(100, s);
}

QString Styles::subHeader(QString const &s) const {
    return d->bold(80, s);
}

QString Styles::collection(QString const &link, QString const &s) const {
    return d->boldLink(160, link, s);
}
QString Styles::collectionLabel(QString const &s) const {
    return d->normal(160, s);
}
QString Styles::collectionNoLink(QString const &s) const {
    return d->bold(160, s);
}

QString Styles::tracks(QString const &s) const {
    return d->bold(160, s);
}
QString Styles::tracksLabel(QString const &s) const {
    return d->normal(160, s);
}

QString Styles::creditName(QString const &link, QString const &s) const {
    return d->boldLink(137, link, s);
}
QString Styles::creditDescription(QString const &s) const {
    return d->normal(137, s);
}

int Styles::bigImageWidth() const {
    return d->width / 4.5;
}

} // namespace greu::gaion

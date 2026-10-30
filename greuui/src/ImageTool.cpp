#include "ImageTool.h"
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>
#include <QStyle>
#include <QToolBar>
#include <QVBoxLayout>
#include "Action.h"
#include "Icons.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "imageTransformation.h"
#include "utils.h"
#include <cmath>

using namespace std;
using namespace greu::core;
using namespace greu::ui;

namespace {

void addSpinBox(GuardedPtr<QSpinBox> &spin,
                GuardedPtr<QLabel> &label,
                QGridLayout *layout,
                int &row,
                QString const &text,
                int value,
                int min,
                int max) {
    addNewWidget(label, text, layout, row, 0);
    addNewWidget(spin, layout, row, 1);
    label->setBuddy(spin.get());
    spin->setRange(min, max);
    spin->setValue(value);
    ++row;
}

using TriggerFunc = function<void()>;
void makeStandardActions(GuardedPtr<Action> &ok,
                         GuardedPtr<Action> &cancel,
                         Dialog *that,
                         QToolBar *toolbar,
                         TriggerFunc funcOk = nullptr,
                         TriggerFunc funcCancel = nullptr) {
    ok = new Action{QObject::tr("OK"),
                    QKeySequence(Qt::Key_Enter),
                    QKeySequence(Qt::Key_Return),
                    GREU_ICON(ok)};
    cancel = new Action{QObject::tr("Cancel"), QKeySequence(Qt::Key_Escape), GREU_ICON(cancel)};
    toolbar->addAction(ok.get());
    toolbar->addAction(cancel.get());
    if (funcOk)
        greu::ui::connect(ok.get(), funcOk);
    else
        greu::ui::connect(ok.get(), [that] { that->accept(); });
    if (funcCancel)
        greu::ui::connect(cancel.get(), funcOk);
    else
        greu::ui::connect(cancel.get(), [that] { that->reject(); });
}

} // namespace

namespace greu::ui {

struct ImageTool::Detail {
    struct ImageLabel : public QLabel {
        QImage image;
        QRect visible;

        ImageLabel(QImage const *image)
            : image(*image) {
            visible = image->rect();
            setBackgroundRole(QPalette::Base);
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            setMinimumSize(image->width(), image->height());
            setScaledContents(true);
        }

        void setImage(QImage &image) {
            this->image = image;
            visible = image.rect();
            update();
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            setMinimumSize(this->image.width(), this->image.height());
            setMaximumSize(this->image.width(), this->image.height());
            adjustSize();
        }

        int x() const { return visible.x(); }

        int y() const { return visible.y(); }

        void setX(int value) {
            auto o = visible.width();
            visible.setX(value);
            visible.setWidth(o);
        }

        void setY(int value) {
            auto o = visible.height();
            visible.setY(value);
            visible.setHeight(o);
        }

        void setHeight(int value) { visible.setHeight(value); }

        void setWidth(int value) { visible.setWidth(value); }

        QRect visibleRect() const { return visible; }

        virtual void paintEvent(QPaintEvent *) override {
            auto style = QLabel::style();
            QPainter painter(this);
            drawFrame(&painter);
            auto cr = contentsRect();
            cr.adjust(margin(), margin(), -margin(), -margin());
            int align = QStyle::visualAlignment(Qt::LeftToRight, Qt::AlignLeft | Qt::AlignTop);
            QPixmap pix;
            long double sx{1};
            long double sy{1};
            if (hasScaledContents()) {
                auto scaledSize = cr.size() * devicePixelRatioF();
                auto scaledImage = image.scaled(scaledSize,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);
                pix = QPixmap::fromImage(scaledImage);
                pix.setDevicePixelRatio(devicePixelRatioF());
                sx = static_cast<long double>(scaledImage.width())
                     / static_cast<long double>(image.width());
                sy = static_cast<long double>(scaledImage.height())
                     / static_cast<long double>(image.height());
            } else {
                pix = QPixmap::fromImage(image);
            }
            style->drawItemPixmap(&painter, cr, align, pix);
            QPen p{Qt::NoPen};
            QBrush b{Qt::gray, Qt::Dense5Pattern};
            painter.setBrush(b);
            painter.setPen(p);
            auto ir = image.rect();
            if (ir.x() < visible.x()) {
                painter.drawRect(sx * ir.x(),
                                 sy * ir.y(),
                                 sx * (visible.x() - ir.x()),
                                 sy * ir.height());
            }
            if (ir.y() < visible.y()) {
                painter.drawRect(sx * ir.x(),
                                 sy * ir.y(),
                                 sx * ir.width(),
                                 sy * (visible.y() - ir.y()));
            }
            if (visible.x() + visible.width() < ir.width()) {
                painter.drawRect(sx * (visible.x() + visible.width()),
                                 sy * ir.y(),
                                 sx * (ir.width() - visible.width() - visible.x()),
                                 sy * ir.height());
            }
            if (visible.y() + visible.height() < ir.height()) {
                painter.drawRect(sx * ir.x(),
                                 sy * (visible.y() + visible.height()),
                                 sx * ir.width(),
                                 sy * (ir.height() - visible.height() - visible.y()));
            }
        }
    };

    struct CropTool : public Dialog {
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QSpinBox> x, y, w, h;
        GuardedPtr<QLabel> xl, yl, wl, hl;
        GuardedPtr<QToolBar> tools;
        GuardedPtr<Action> ok, cancel;
        ImageLabel *imageLabel;
        QRect imageSize;

        CropTool(QImage const *image, ImageLabel *imageLabel, Dialog *dialog)
            : imageLabel(imageLabel)
            , imageSize(image->rect()) {
            setWindowTitle(QObject::tr("Crop image"));
            layout = new QGridLayout;
            setLayout(layout.get());
            int row = 0;
            addSpinBox(x, xl, layout.get(), row, QObject::tr("&x"), 0, 0, image->width() - 1);
            addSpinBox(y, yl, layout.get(), row, QObject::tr("&y"), 0, 0, image->height() - 1);
            addSpinBox(w,
                       wl,
                       layout.get(),
                       row,
                       QObject::tr("&width"),
                       image->width(),
                       1,
                       image->width());
            addSpinBox(h,
                       hl,
                       layout.get(),
                       row,
                       QObject::tr("&height"),
                       image->height(),
                       1,
                       image->height());
            addNewWidget(tools, layout.get(), row, 0, 1, 4);
            ++row;
            makeStandardActions(
                ok,
                cancel,
                this,
                tools.get(),
                [this] {
                    this->imageLabel->setX(x->value());
                    this->imageLabel->setY(y->value());
                    this->imageLabel->setWidth(w->value());
                    this->imageLabel->setHeight(h->value());
                    this->imageLabel->update();
                    accept();
                },
                [this] {
                    this->imageLabel->setX(0);
                    this->imageLabel->setY(0);
                    this->imageLabel->setWidth(imageSize.width());
                    this->imageLabel->setHeight(imageSize.height());
                    this->imageLabel->update();
                    reject();
                });

            ui::connect(x.get(), [this](auto) { changeVisibleSize(); });
            ui::connect(y.get(), [this](auto) { changeVisibleSize(); });
            ui::connect(w.get(), [this](auto) { changeVisibleSize(); });
            ui::connect(h.get(), [this](auto) { changeVisibleSize(); });

            move(dialog->x() + dialog->frameGeometry().width()
                     + (dialog->frameGeometry().width() - dialog->width()) / 2,
                 dialog->geometry().y());
        }

        void changeVisibleSize() {
            auto old = imageLabel->visibleRect();
            if (old.x() != x->value()) {             // x was changed
                if (old.width() - x->value() == 0) { // has now width 0
                    x->setValue(old.x());
                    return;
                }
                imageLabel->setX(x->value());
                if (old.x() < x->value() && x->value() + old.width() > imageSize.width()) {
                    auto v = old.width() - x->value() + old.x();
                    imageLabel->setWidth(v);
                    w->setValue(v);
                }
            } else if (old.width() != w->value()) { // width was changed
                if (w->value() + x->value() > imageSize.width()) {
                    w->setValue(old.width());
                }
                imageLabel->setWidth(w->value());
            }
            if (old.y() != y->value()) {              // y was changed
                if (old.height() - y->value() == 0) { // has now height 0
                    y->setValue(old.y());
                    return;
                }
                imageLabel->setY(y->value());
                if (old.y() < y->value() && y->value() + old.height() > imageSize.height()) {
                    auto v = old.height() - y->value() + old.y();
                    imageLabel->setHeight(v);
                    h->setValue(v);
                }
            } else if (old.height() != h->value()) { // height was changed
                if (h->value() + y->value() > imageSize.height()) {
                    h->setValue(old.height());
                }
                imageLabel->setHeight(h->value());
            }
            imageLabel->update();
        }
    };

    struct ResizeTool : public Dialog {
        QSize size;
        GuardedPtr<QSpinBox> w, h;
        GuardedPtr<QLabel> wl, hl;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QToolBar> tools;
        GuardedPtr<Action> ok, cancel;
        bool inChange;

        ResizeTool(QImage const *image, Dialog *dialog)
            : size(image->size())
            , inChange(false) {
            setWindowTitle(QObject::tr("Scale image"));
            layout = new QGridLayout(this);
            setLayout(layout.get());
            int row = 0;
            addSpinBox(w,
                       wl,
                       layout.get(),
                       row,
                       QObject::tr("&width"),
                       image->width(),
                       1,
                       image->width());
            addSpinBox(h,
                       hl,
                       layout.get(),
                       row,
                       QObject::tr("&height"),
                       image->height(),
                       1,
                       image->height());
            addNewWidget(tools, layout.get(), row, 0, 1, 4);
            ++row;
            makeStandardActions(ok, cancel, this, tools.get());

            ui::connect(w.get(), [this](auto) {
                if (inChange)
                    return;
                inChange = true;
                h->setValue(std::roundl(static_cast<long double>(size.height() * w->value())
                                        / static_cast<long double>(size.width())));
                inChange = false;
            });

            ui::connect(h.get(), [this](auto) {
                if (inChange)
                    return;
                inChange = true;
                w->setValue(std::roundl(static_cast<long double>(size.width() * h->value())
                                        / static_cast<long double>(size.height())));
                inChange = false;
            });

            move(dialog->x() + dialog->frameGeometry().width()
                     + (dialog->frameGeometry().width() - dialog->width()) / 2,
                 dialog->geometry().y());
        }

        QSize imageSize() const { return QSize{w->value(), h->value()}; }
    };

    struct QuadTool : public Dialog {
        int length;
        GuardedPtr<QSpinBox> l;
        GuardedPtr<QLabel> ll;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<Action> ok, cancel;
        GuardedPtr<QToolBar> tools;

        QuadTool(QImage const *image, QDialog *dialog, int defaultQuadWidth) {
            setWindowTitle(QObject::tr("Quadrat image"));
            length = (defaultQuadWidth == 0 ? max(image->width(), image->height())
                                            : defaultQuadWidth);
            layout = new QGridLayout;
            setLayout(layout.get());
            int row = 0;
            addSpinBox(l,
                       ll,
                       layout.get(),
                       row,
                       QObject::tr("&length"),
                       length,
                       1,
                       std::max(image->width(), image->height()));
            addNewWidget(tools, layout.get(), row, 0, 1, 4);
            ++row;
            makeStandardActions(ok, cancel, this, tools.get());

            ui::connect(l.get(), [this](int) { length = l->value(); });

            move(dialog->x() + dialog->frameGeometry().width()
                     + (dialog->frameGeometry().width() - dialog->width()) / 2,
                 dialog->geometry().y());
        }
    };

    QImage image;
    QString description;
    GuardedPtr<ImageLabel> imageLabel;
    int defaultQuadWidth;
    GuardedPtr<QVBoxLayout> layout;
    GuardedPtr<QToolBar> tools;
    GuardedPtr<Action> ok, cancel, actCrop, actQuad, actResize;

    Detail(QImage const &image, QString const &description, int dq)
        : image(image)
        , description(description)
        , defaultQuadWidth(dq) {}

    void setupUi(ImageTool *that) {
        that->setWindowTitle(
            QObject::tr("%1 (%2x%3)")
                .arg(description, QString::number(image.width()), QString::number(image.height())));
        layout = new QVBoxLayout;
        that->setLayout(layout.get());
        imageLabel = new ImageLabel(&image);
        layout->addWidget(imageLabel.get());
        tools = new QToolBar;
        makeStandardActions(ok, cancel, that, tools.get());
        actCrop = new Action(QObject::tr("Crop"),
                             QKeySequence(int(Qt::CTRL) + Qt::Key_C),
                             GREU_ICON(crop));
        actQuad = new Action(QObject::tr("Quadrat"),
                             QKeySequence(int(Qt::CTRL) + Qt::Key_Q),
                             GREU_ICON(quadrat));
        actResize = new Action(QObject::tr("Resize"),
                               QKeySequence(int(Qt::CTRL) + Qt::Key_S),
                               GREU_ICON(resize));
        tools->addSeparator();
        tools->addAction(actCrop.get());
        tools->addAction(actQuad.get());
        tools->addAction(actResize.get());
        layout->addWidget(tools.get());
        imageLabel->adjustSize();

        ui::connect(actCrop.get(), [this, that] {
            auto tool = new CropTool(&image, imageLabel.get(), that);
            if (tool->exec()) {
                setImage(image.copy(imageLabel->visibleRect()), that);
            }
        });

        ui::connect(actQuad.get(), [this, that] {
            auto tool = new QuadTool(&image, that, defaultQuadWidth);
            if (tool->exec())
                setImage(scale(image, tool->length, tool->length, *that, ImageOrientation::portrait),
                         that);
        });

        ui::connect(actResize.get(), [this, that] {
            auto tool = new ResizeTool(&image, that);
            if (tool->exec()) {
                setImage(image.scaled(tool->imageSize(),
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation),
                         that);
            }
        });
    }

    void setImage(QImage img, ImageTool *that) {
        imageLabel->setImage(img);
        that->adjustSize();
        that->update();
        imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        image = img;
        that->setWindowTitle(
            QObject::tr("%1 (%2x%3)")
                .arg(description, QString::number(image.width()), QString::number(image.height())));
    };
};

ImageTool::ImageTool(QImage const &image, QString const &description, int dq)
    : d(GREU_MKDET(image, description, dq)) {
    d->setupUi(this);
}

QImage ImageTool::image() const {
    return d->image;
}

} // namespace greu::ui

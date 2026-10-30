#pragma once
#include "greucore/include/DetailPtr.h"
#include "greucore/include/GuardedPtr.h"
#include <any>

class QWidget;
class QPushButton;
class QBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QHBoxLayout;
class QScrollArea;

namespace greu::movies {
class CreditHolder;
}
namespace greu::gaion {

class LinkController;

struct GREU_PUBLIC(GAION) PresentationWidget {
    PresentationWidget();
    virtual ~PresentationWidget() = default;
    GREU_NO_COPY(PresentationWidget);
    GREU_NO_MOVE(PresentationWidget);

    virtual QWidget *widget() const = 0;
    LinkController *linkController() const;

    virtual std::any position() const = 0;
    virtual void setPosition(std::any const &) = 0;

    static void gridEven(QGridLayout *, std::set<size_t> const &exclRows = {}, std::set<size_t> const &exclCols = {});
    static void gridLeft(QGridLayout *);
    static void boxEven(QBoxLayout *, std::set<size_t> const &excl = {});

protected:
    struct CastCrewButton {
        core::GuardedPtr<QWidget> widget;
        core::GuardedPtr<QHBoxLayout> layout;
        core::GuardedPtr<QPushButton> button;
    };
    static CastCrewButton castCrewButton(movies::CreditHolder const *, QVBoxLayout *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion

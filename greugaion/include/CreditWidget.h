#pragma once
#include "greucore/include/DetailPtr.h"
#include "greumovies/include/creditUtils.h"

class QWidget;

namespace greu::gaion {

class PresentationWidget;

class CreditWidget {
public:
    enum class ListType {
        Names,
        Pictures,
    };
    CreditWidget(QString const &, movies::MediumCredits const &, ListType, size_t, PresentationWidget *);
    QWidget *widget() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion

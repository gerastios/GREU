#pragma once
#include "greumovies/include/Collection.h"
#include "greuui/include/Dialog.h"

class QValidator;

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) CollectionEditor : public ui::Dialog {
public:
    CollectionEditor();

    QString text() const;
    QString description() const;
    movies::Collection::SortOrder sortOrder() const;
    ui::ImageContainer image() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup

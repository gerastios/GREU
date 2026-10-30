#include "PersonItem.h"
#include <QImage>
#include "FeatureModel.h"
#include "PersonWidget.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/MovieCollection.h"
#include "greumovies/include/Person.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct PersonItem::Detail {
    struct Model : public FeatureModel {
        Person const *const person;
        PresentationWidget *const widget;

        Model(Person const *p, PresentationWidget *w)
            : person(p)
            , widget(w) {}

        virtual QString title() const override { return person->name(); }

        virtual QString titleLabel() const override { return QObject::tr("Name"); }

        virtual QImage image() const override { return person->image(); }

        virtual optional<Uuid> linkedUuid() const override { return person->uuid(); }

        virtual QString description() const override { return person->description(); }

        virtual QString descriptionLabel() const override { return QObject::tr("Biography"); }

        virtual QDate born() const override { return person->date(); }

        virtual QString bornPlace() const override { return person->placeOfBirth(); }

        virtual QDate died() const override { return person->deathday(); }

        virtual int age() const override { return person->age(); }

        virtual QString knownForDepartment() const override {
            return person->knownForJobDepartment();
        }
    };

    Person const *const person;
    unique_ptr<PersonWidget> widget;
    unique_ptr<Model> model;

    Detail(Presentation const &p)
        : person(safeCast<Person>(MovieObjectItem::object(p))) {}
};

PersonItem::PersonItem(Presentation const &p)
    : MovieObjectItem(p)
    , d(GREU_MKDET(p)) {
    d->model = make_unique<Detail::Model>(d->person, d->widget.get());
}

Person const *PersonItem::person() const {
    return d->person;
}

PresentationWidget *PersonItem::widget() const {
    if(!d->widget)
        d->widget = make_unique<PersonWidget>(this);
    return d->widget.get();
}

FeatureModel *PersonItem::featureModel() const {
    return d->model.get();
}

} // namespace greu::gaion

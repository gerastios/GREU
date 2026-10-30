#include "PlainSelector.h"
#include "Settings.h"

using namespace std;

namespace greu::core {

struct PlainSelector::Detail {
    unique_ptr<Settings> settings;
    int biggest = 0;
    static char const *key;

    int insert(QString const &value, bool onInit, PlainSelector *that) {
        if (that->contains(value))
            return that->idOf(value);
        if (onInit)
            that->Selector::insertInit({biggest, value});
        else
            that->Selector::insert({biggest, value});
        return biggest++;
    }

    void initValues(InitializerList const &list, PlainSelector *that) {
        for (auto &l : list)
            insert(l, true, that);
    }

    void init(InitializerList const &list, QString const &settingsKey, PlainSelector *that) {
        if (!settingsKey.isEmpty()) {
            settings = make_unique<Settings>(settingsKey);
            auto size = settings->beginReadArray(settingsKey);
            for (int i = 0; i < size; i++) {
                settings->setArrayIndex(i);
                insert(settings->value(key).toString(), true, that);
            }
            settings->endArray();
        }
        if (that->size() == 0)
            initValues(list, that);
    }
};

char const *PlainSelector::Detail::key = "ITEM";

PlainSelector::PlainSelector()
    : d(GREU_MKDET()) {
    d->init(InitializerList{}, "", this);
}

PlainSelector::PlainSelector(QString const &settingsKey)
    : d(GREU_MKDET()) {
    d->init(InitializerList{}, settingsKey, this);
}

PlainSelector::PlainSelector(InitializerList const &list)
    : d(GREU_MKDET()) {
    d->init(list, "", this);
}

PlainSelector::PlainSelector(InitializerList const &list, QString const &settingsKey)
    : d(GREU_MKDET()) {
    d->init(list, settingsKey, this);
}

PlainSelector::PlainSelector(InitializerList const &list,
                             QString const &settingsKey,
                             bool userValuesAllowed,
                             bool InsertionAllowed,
                             bool DeletionAllowed)
    : d(GREU_MKDET()) {
    d->init(list, settingsKey, this);
    allowUserValues(userValuesAllowed);
    allowInsertion(InsertionAllowed);
    allowDeletion(DeletionAllowed);
}

PlainSelector::PlainSelector(QString const &settingsKey,
                             bool userValuesAllowed,
                             bool InsertionAllowed,
                             bool DeletionAllowed)
    : d(GREU_MKDET()) {
    d->init(InitializerList{}, settingsKey, this);
    allowUserValues(userValuesAllowed);
    allowInsertion(InsertionAllowed);
    allowDeletion(DeletionAllowed);
}

PlainSelector::~PlainSelector() {
    if (d->settings && isChanged()) {
        d->settings->beginWriteArray(d->settings->applicationName());
        for (size_t i = 0; i < size(); i++) {
            d->settings->setArrayIndex(i);
            d->settings->setValue(d->key, atSortedByString(i).second);
        }
        d->settings->endArray();
    }
}

int PlainSelector::insert(QString const &v) {
    if (isUserValuesAllowed())
        return d->insert(v, false, this);
    return -1;
}

void PlainSelector::insertInit(QString const &v) {
    d->insert(v, true, this);
}

} // namespace greu::core

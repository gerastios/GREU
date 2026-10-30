#include "Selector.h"
#include "Settings.h"
#include "vectorUtils.h"
#include <unordered_map>
#include <vector>

using namespace std;

namespace greu::core {

struct Selector::Detail {
    vector<QString> strings;
    vector<int> ids;

    unordered_map<int, size_t> ids2Indexes;
    unordered_map<QString, int> strings2Ids;
    vector<vector<QString>::size_type> indexesSortedById;
    vector<vector<QString>::size_type> indexesSortedByString;

    unique_ptr<Settings> settings;

    bool userValuesAllowed = false;
    int userValueId = -1;
    bool insertionAllowed = false;
    bool deletionAllowed = false;

    static char const *IdKey;
    static char const *ItemKey;

    bool changed = false;

    EmptySig valuesChanged;

    Detail(QString const &settingsKey, InitializerList const &list) {
        init(list);
        init(settingsKey);
    }

    void remove(int id) {
        QString s;
        s = value(id);
        removeSortedIndex(ids, indexesSortedById, id);
        removeSortedIndex(strings, indexesSortedByString, s);
        strings2Ids.erase(s);
        auto idx = ids2Indexes.at(id);
        ids2Indexes.erase(id);
        for (auto i = ids2Indexes.begin(); i != ids2Indexes.end(); i++)
            if (i->second > idx)
                i->second--;
        erase(ids, id);
        erase(strings, s);
        changed = true;
        valuesChanged();
    }

    ~Detail() {
        if (settings && changed) {
            settings->beginWriteArray(settings->applicationName());
            for (int i = 0; i < int(ids.size()); i++) {
                settings->setArrayIndex(i);
                auto id = ids[i];
                settings->setValue(IdKey, id);
                settings->setValue(ItemKey, value(id));
            }
            settings->endArray();
        }
    }

    SingleValue atSortedById(size_t index) const {
        SingleValue res;
        ;
        try {
            res.first = ids.at(indexesSortedById.at(index));
            res.second = strings.at(ids2Indexes.at(res.first));
        } catch (std::out_of_range &) {
            assert(false && "bad sorted id");
        }
        return res;
    }

    SingleValue atSortedByString(size_t index) const {
        SingleValue res;
        ;
        try {
            res.second = strings.at(indexesSortedByString.at(index));
            res.first = strings2Ids.at(res.second);
        } catch (std::out_of_range &) {
            assert(false && "bad sorted string");
        }
        return res;
    }

    void init(InitializerList const &list) {
        for (auto &l : list) {
            insert(l, true);
        }
    }

    void init(QString const &settingsKey) {
        if (!settingsKey.isEmpty()) {
            settings = make_unique<Settings>(settingsKey);
            auto size = settings->beginReadArray(settingsKey);
            for (int i = 0; i < size; i++) {
                settings->setArrayIndex(i);
                insert({settings->value(IdKey).toInt(), settings->value(ItemKey).toString()}, true);
            }
            settings->endArray();
        }
    }

    void insert(SingleValue const &value, bool onInit) {
        if (!contains(value.first) && !strings2Ids.contains(value.second)) {
            strings.push_back(value.second);
            ids.push_back(value.first);
            ids2Indexes.insert({value.first, strings.size() - 1});
            strings2Ids.insert({value.second, value.first});
            insertSortedIndex(strings, indexesSortedByString, value.second, strings.size() - 1);
            insertSortedIndex(ids, indexesSortedById, value.first, ids.size() - 1);
            if (!onInit)
                changed = true;
            valuesChanged();
        }
    }

    QString value(int id) const {
        assert(contains(id) && "Invalid id");
        return strings.at(ids2Indexes.at(id));
    }

    bool contains(int id) const { return ids2Indexes.contains(id); }
};

char const *Selector::Detail::IdKey = "Id";
char const *Selector::Detail::ItemKey = "Item";

Selector::Selector()
    : d(GREU_MKDET("", InitializerList{})) {}

Selector::Selector(QString const &settingsKey)
    : d(GREU_MKDET(settingsKey, InitializerList{})) {}

Selector::Selector(InitializerList const &list)
    : d(GREU_MKDET("", list)) {}

Selector::Selector(QString const &settingsKey, InitializerList const &list)
    : d(GREU_MKDET(settingsKey, list)) {}

void Selector::allowUserValues(bool v) {
    d->userValuesAllowed = v;
}

bool Selector::isUserValuesAllowed() const {
    return d->userValuesAllowed;
}

void Selector::setUserValueId(int value) {
    d->userValueId = value;
}

int Selector::userValueId() const {
    return d->userValueId;
}

void Selector::allowInsertion(bool v) {
    d->insertionAllowed = v;
}

bool Selector::isInsertionAllowed() const {
    return d->insertionAllowed;
}

void Selector::allowDeletion(bool v) {
    d->deletionAllowed = v;
}

bool Selector::isDeletionAllowed() const {
    return d->deletionAllowed;
}

size_t Selector::size() const {
    return d->ids.size();
}

EmptySig &Selector::valuesChanged() const {
    return d->valuesChanged;
}

void Selector::insert(SingleValue const &value) {
    if (isInsertionAllowed())
        d->insert(value, false);
}

void Selector::remove(int id) {
    if (isDeletionAllowed() && contains(id)) {
        d->remove(id);
    }
}

QString Selector::value(int id) const {
    return d->value(id);
}

bool Selector::contains(int id) const {
    return d->contains(id);
}

bool Selector::contains(QString const &string) const {
    return d->strings2Ids.contains(string);
}

int Selector::idOf(QString const &string) const {
    if (contains(string))
        return d->strings2Ids.at(string);
    return -1;
}

Selector::AtFunction Selector::atFunction() const {
    return &Selector::atSortedByString;
}

Selector::SingleValue Selector::atSortedById(size_t index) const {
    return d->atSortedById(index);
}

Selector::SingleValue Selector::atSortedByString(size_t index) const {
    return d->atSortedByString(index);
}

void Selector::insertInit(SingleValue value) {
    d->insert(value, true);
}

bool Selector::isChanged() const {
    return d->changed;
}

} // namespace greu::core

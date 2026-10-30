#pragma once

#include <QString>
#include "DetailPtr.h"
#include <functional>

namespace greu::core {

class GREU_PUBLIC(CORE) Language {
public:
    static uint constexpr code1Len{2};
    static uint constexpr code2Len{3};

    static struct Code2 {
        static QString const Catalan;
        static QString const Czech;
        static QString const Danish;
        static QString const German;
        static QString const GreekModern;
        static QString const English;
        static QString const Finnish;
        static QString const French;
        static QString const Hungarian;
        static QString const Italian;
        static QString const Japanese;
        static QString const Dutch;
        static QString const Polish;
        static QString const Portuguese;
        static QString const Russian;
        static QString const Slovak;
        static QString const Slovenian;
        static QString const Spanish;
        static QString const Swedish;
        static QString const Turkish;
        static QString const Arabic;
        static QString const Norwegian;
        static QString const Chinese;
    } Code2;

    static std::optional<Language const *> fromCode1(QString const &);
    static std::optional<Language const *> fromCode2(QString const &);
    static std::optional<Language const *> fromCode(QString const &);
    static std::optional<Language const *> fromName(QString const &);

    using LanguageFunction = std::function<void(Language const &)>;
    static void forEach(LanguageFunction);

    bool hasCode1() const;
    QString const &code1() const;
    QString const &code2() const;
    QString const &name() const;

    bool operator==(Language const &) const;
    inline bool operator!=(Language const &o) const { return !(*this == o); }

private:
    friend struct LanguageFactory_;
    Language(QString const &code2,
             QString const &code2_2,
             QString const &code1,
             QString const &name);
    struct Detail;
    DetailPtr<Detail> d;
};

} // namespace greu::core

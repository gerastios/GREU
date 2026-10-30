#include "CommonSelectors.h"
#include <QObject>
#include "greucore/include/PlainSelector.h"

namespace greu::tag {

using namespace core;

PlainSelector *CommonSelectors::genres() {
    static PlainSelector genres{{{QObject::tr("Vocal")},
                                 {QObject::tr("Spoken")},
                                 {QObject::tr("R&B")},
                                 {QObject::tr("Rock")},
                                 {QObject::tr("Religious")},
                                 {QObject::tr("Reggae")},
                                 {QObject::tr("Rap")},
                                 {QObject::tr("Pop")},
                                 {QObject::tr("New Age")},
                                 {QObject::tr("Latin")},
                                 {QObject::tr("Jazz")},
                                 {QObject::tr("Folk")},
                                 {QObject::tr("Electronic")},
                                 {QObject::tr("Easy Listening")},
                                 {QObject::tr("Country")},
                                 {QObject::tr("Classical")},
                                 {QObject::tr("Blues")},
                                 {QObject::tr("Avantgarde")},
                                 {QObject::tr("Alternative")}},
                                "genres",
                                true,
                                true,
                                true};
    return &genres;
}

PlainSelector *CommonSelectors::moods() {
    static PlainSelector moods{{{QObject::tr("Trippy")},
                                {QObject::tr("Sexy")},
                                {QObject::tr("Sentimental")},
                                {QObject::tr("Sad")},
                                {QObject::tr("Romantic")},
                                {QObject::tr("Passionate")},
                                {QObject::tr("Mellow")},
                                {QObject::tr("Melancholy")},
                                {QObject::tr("Laid Back")},
                                {QObject::tr("Happy")},
                                {QObject::tr("Energetic")},
                                {QObject::tr("Druggy")},
                                {QObject::tr("Bittersweet")},
                                {QObject::tr("Aggressive")}},
                               "moods",
                               true,
                               true,
                               true};
    return &moods;
}

PlainSelector *CommonSelectors::instruments() {
    static PlainSelector instruments{{{QObject::tr("Vocals")},
                                      {QObject::tr("Violin")},
                                      {QObject::tr("Viola")},
                                      {QObject::tr("Sitar")},
                                      {QObject::tr("Synthesizer")},
                                      {QObject::tr("Saxophone")},
                                      {QObject::tr("Recorder")},
                                      {QObject::tr("Piano")},
                                      {QObject::tr("Percussion")},
                                      {QObject::tr("Organ")},
                                      {QObject::tr("Keyboards")},
                                      {QObject::tr("Guitar")},
                                      {QObject::tr("Flute")},
                                      {QObject::tr("Drums")},
                                      {QObject::tr("Cello")},
                                      {QObject::tr("Bass")}},
                                     "instruments",
                                     true,
                                     true,
                                     true};
    return &instruments;
}

PlainSelector *CommonSelectors::peopleFunctions() {
    static core::PlainSelector peopleFunctions{{{QObject::tr("Producer")}, {QObject::tr("Janitor")}},
                                               "peopleFunctions"};
    return &peopleFunctions;
}

} // namespace greu::tag

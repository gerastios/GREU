#include "Tag.h"
#include <QValidator>
#include "File.h"
#include "FileTypeSelector.h"
#include "Frame.h"
#include "ImageTypeSelector.h"
#include "MediaTypeSelector.h"
#include "UrlTypeSelector.h"
#include "greucore/include/StaticSelector.h"
#include "greucore/include/hexUtils.h"
#include "greutag/include/AttributeTypes.h"
#include "greutag/include/CommonSelectors.h"
#include "greuui/include/LengthValidator.h"

using namespace std;

namespace {
struct Genre {
    QString number, genre;
};
unordered_map<QString, QString> const Genres = {
    // clamg-format off
    {"0", "Blues"},
    {"1", "Classic Rock"},
    {"2", "Country"},
    {"3", "Dance"},
    {"4", "Disco"},
    {"5", "Funk"},
    {"6", "Grunge"},
    {"7", "Hip-Hop"},
    {"8", "Jazz"},
    {"9", "Metal"},
    {"10", "New Age"},
    {"11", "Oldies"},
    {"12", "Other"},
    {"13", "Pop"},
    {"14", "R&B"},
    {"15", "Rap"},
    {"16", "Reggae"},
    {"17", "Rock"},
    {"18", "Techno"},
    {"19", "Industrial"},
    {"20", "Alternative"},
    {"21", "Ska"},
    {"22", "Death Metal"},
    {"23", "Pranks"},
    {"24", "Soundtrack"},
    {"25", "Euro-Techno"},
    {"26", "Ambient"},
    {"27", "Trip-Hop"},
    {"28", "Vocal"},
    {"29", "Jazz+Funk"},
    {"30", "Fusion"},
    {"31", "Trance"},
    {"32", "Classical"},
    {"33", "Instrumental"},
    {"34", "Acid"},
    {"35", "House"},
    {"36", "Game"},
    {"37", "Sound Clip"},
    {"38", "Gospel"},
    {"39", "Noise"},
    {"40", "AlternRock"},
    {"41", "Bass"},
    {"42", "Soul"},
    {"43", "Punk"},
    {"44", "Space"},
    {"45", "Meditative"},
    {"46", "Instrumental Pop"},
    {"47", "Instrumental Rock"},
    {"48", "Ethnic"},
    {"49", "Gothic"},
    {"50", "Darkwave"},
    {"51", "Techno-Industrial"},
    {"52", "Electronic"},
    {"53", "Pop-Folk"},
    {"54", "Eurodance"},
    {"55", "Dream"},
    {"56", "Southern Rock"},
    {"57", "Comedy"},
    {"58", "Cult"},
    {"59", "Gangsta"},
    {"60", "Top 40"},
    {"61", "Christian Rap"},
    {"62", "Pop/Funk"},
    {"63", "Jungle"},
    {"64", "Native American"},
    {"65", "Cabaret"},
    {"66", "New Wave"},
    {"67", "Psychadelic"},
    {"68", "Rave"},
    {"69", "Showtunes"},
    {"70", "Trailer"},
    {"71", "Lo-Fi"},
    {"72", "Tribal"},
    {"73", "Acid Punk"},
    {"74", "Acid Jazz"},
    {"75", "Polka"},
    {"76", "Retro"},
    {"77", "Musical"},
    {"78", "Rock & Roll"},
    {"79", "Hard Rock"},
    {"80", "Folk"},
    {"81", "Folk-Rock"},
    {"82", "National Folk"},
    {"83", "Swing"},
    {"84", "Fast Fusion"},
    {"85", "Bebob"},
    {"86", "Latin"},
    {"87", "Revival"},
    {"88", "Celtic"},
    {"89", "Bluegrass"},
    {"90", "Avantgarde"},
    {"91", "Gothic Rock"},
    {"92", "Progressive Rock"},
    {"93", "Psychedelic Rock"},
    {"94", "Symphonic Rock"},
    {"95", "Slow Rock"},
    {"96", "Big Band"},
    {"97", "Chorus"},
    {"98", "Easy Listening"},
    {"99", "Acoustic"},
    {"100", "Humour"},
    {"101", "Speech"},
    {"102", "Chanson"},
    {"103", "Opera"},
    {"104", "Chamber Music"},
    {"105", "Sonata"},
    {"106", "Symphony"},
    {"107", "Booty Bass"},
    {"108", "Primus"},
    {"109", "Porn Groove"},
    {"110", "Satire"},
    {"111", "Slow Jam"},
    {"112", "Club"},
    {"113", "Tango"},
    {"114", "Samba"},
    {"115", "Folklore"},
    {"116", "Ballad"},
    {"117", "Power Ballad"},
    {"118", "Rhythmic Soul"},
    {"119", "Freestyle"},
    {"120", "Duet"},
    {"121", "Punk Rock"},
    {"122", "Drum Solo"},
    {"123", "A capella"},
    {"124", "Euro-House"},
    {"125", "Dance Hall"},
    {"126", "Goa"},
    {"127", "Drum & Bass"},
    {"128", "Club-House"},
    {"129", "Hardcore"},
    {"130", "Terror"},
    {"131", "Indie"},
    {"132", "BritPop"},
    {"133", "Negerpunk"},
    {"134", "Polsk Punk"},
    {"135", "Beat"},
    {"136", "Christian Gangsta Rap"},
    {"137", "Heavy Metal"},
    {"138", "Black Metal"},
    {"139", "Crossover"},
    {"140", "Contemporary Christian"},
    {"141", "Christian Rock"},
    {"142", "Merengue"},
    {"143", "Salsa"},
    {"144", "Thrash Metal"},
    {"145", "Anime"},
    {"146", "JPop"},
    {"147", "Synthpop"},
    {"148", "Abstract"},
    {"149", "Art Rock"},
    {"150", "Baroque"},
    {"151", "Bhangra"},
    {"152", "Big Beat"},
    {"153", "Breakbeat"},
    {"154", "Chillout"},
    {"155", "Downtempo"},
    {"156", "Dub"},
    {"157", "EBM"},
    {"158", "Eclectic"},
    {"159", "Electro"},
    {"160", "Electroclash"},
    {"161", "Emo"},
    {"162", "Experimental"},
    {"163", "Garage"},
    {"164", "Global"},
    {"165", "IDM"},
    {"166", "Illbient"},
    {"167", "Industro-Goth"},
    {"168", "Jam Band"},
    {"169", "Krautrock"},
    {"170", "Leftfield"},
    {"171", "Lounge"},
    {"172", "Math Rock"},
    {"173", "New Romantic"},
    {"174", "Nu-Breakz"},
    {"175", "Post-Punk"},
    {"176", "Post-Rock"},
    {"177", "Psytrance"},
    {"178", "Shoegaze"},
    {"179", "Space Rock"},
    {"180", "Trop Rock"},
    {"181", "World Music"},
    {"182", "Neoclassical"},
    {"183", "Audiobook"},
    {"184", "Audio Theatre"},
    {"185", "Neue Deutsche Welle"},
    {"186", "Podcast"},
    {"187", "Indie Rock"},
    {"188", "G-Funk"},
    {"189", "Dubstep"},
    {"190", "Garage Rock"},
    {"191", "Psybient"},
    {"RX", "Remix"},
    {"CR", "Cover"}
    // clamg-format on
};

greu::core::StaticSelector eventTimingCodesUnits{{1, QObject::tr("MPEG frames")},
                                                 {2, QObject::tr("milliseconds")}};
greu::core::StaticSelector
    eventTimingCodesTypes{{0x00, QObject::tr("padding (has no meaning)")},
                          {0x01, QObject::tr("end of initial silence")},
                          {0x02, QObject::tr("intro start")},
                          {0x03, QObject::tr("main part start")},
                          {0x04, QObject::tr("outro start")},
                          {0x05, QObject::tr("outro end")},
                          {0x06, QObject::tr("verse start")},
                          {0x07, QObject::tr("refrain start")},
                          {0x08, QObject::tr("interlude start")},
                          {0x09, QObject::tr("theme start")},
                          {0x0a, QObject::tr("variation start")},
                          {0x0b, QObject::tr("key change")},
                          {0x0c, QObject::tr("time change")},
                          {0x0d, QObject::tr("momentary unwanted noise (Snap, Crackle & Pop)")},
                          {0x0e, QObject::tr("sustained noise")},
                          {0x0f, QObject::tr("sustained noise end")},
                          {0x10, QObject::tr("intro end")},
                          {0x11, QObject::tr("main part end")},
                          {0x12, QObject::tr("verse end")},
                          {0x13, QObject::tr("refrain end")},
                          {0x14, QObject::tr("theme end")},
                          {0x15, QObject::tr("profanity")},
                          {0x16, QObject::tr("profanity end")},

                          {0xe0, QObject::tr("predefined synch 0")},
                          {0xe1, QObject::tr("predefined synch 1")},
                          {0xe2, QObject::tr("predefined synch 2")},
                          {0xe3, QObject::tr("predefined synch 3")},
                          {0xe4, QObject::tr("predefined synch 4")},
                          {0xe5, QObject::tr("predefined synch 5")},
                          {0xe6, QObject::tr("predefined synch 6")},
                          {0xe7, QObject::tr("predefined synch 7")},
                          {0xe8, QObject::tr("predefined synch 8")},
                          {0xe9, QObject::tr("predefined synch 9")},
                          {0xea, QObject::tr("predefined synch A")},
                          {0xeb, QObject::tr("predefined synch B")},
                          {0xec, QObject::tr("predefined synch C")},
                          {0xed, QObject::tr("predefined synch D")},
                          {0xee, QObject::tr("predefined synch E")},
                          {0xef, QObject::tr("predefined synch F")},

                          {0xfd, QObject::tr("audio end (start of silence)")},
                          {0xfe, QObject::tr("audio file ends")}};

struct ImagesAttribute : public greu::tag::ImagesAttribute {
    using greu::tag::ImagesAttribute::ImagesAttribute;

    virtual QImage const mainValue() const override {
        if (size() == 0)
            return QImage();
        if (contains(greu::id3::ImageTypeSelector::frontCover)) {
            return first(greu::id3::ImageTypeSelector::frontCover);
        }
        return cbegin()->value;
    }
};

struct CopyrightValidator : public QValidator {
    virtual State validate(QString &text, int &) const override {
        if (text.isEmpty())
            return QValidator::Acceptable;
        bool ok;
        if (text.length() <= 4) {
            int i = text.toInt(&ok);
            if (!ok || i < 1 || i > 9999)
                return QValidator::Invalid;
            return QValidator::Intermediate;
        }
        int i = text.left(4).toInt(&ok);
        if (!ok || i < 1 || i > 9999)
            return QValidator::Invalid;
        if (text.at(4) != QChar::Space)
            return QValidator::Invalid;
        return QValidator::Acceptable;
    }
};

struct BytesLengthValidator : public QValidator {
    int const maxLen;

    BytesLengthValidator(int maxLen)
        : maxLen(maxLen) {}

    virtual State validate(QString &text, int &) const override {
        if (greu::core::bytes(greu::core::formatInput(text)).size() > maxLen)
            return QValidator::Invalid;
        return QValidator::Acceptable;
    }
};

greu::ui::LengthValidator languageValidator{3, 3};
CopyrightValidator copyrightValidator;
BytesLengthValidator uniqueFileIdentifierValidator{64};
BytesLengthValidator cdIdentifierValidator{804};
QIntValidator eventTimingCodesValidator{0, 999999999};

} // namespace

namespace greu::id3 {

using namespace core;

struct Tag::Detail {
    vector<Frame> additionalFrames;

    void init(Tag *that) {
        that->setAttributeAlbumArtist(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::albumArtist()), that));
        that->setAttributeAlbumArtistSort(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::albumArtistSort()), that));
        that->setAttributeAlbumTitle(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::albumTitle()), that));
        that->setAttributeAlbumTitleSort(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::albumTitleSort()), that));
        that->setAttributeBeatsPerMinute(make_unique<tag::AttributeTypes::PlainUintAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::beatsPerMinute()), that));
        that->setAttributeCdIdentifier(make_unique<tag::AttributeTypes::PlainBytesAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::cdIdentifier()), that));
        that->setAttributeComments(make_unique<tag::AttributeTypes::StringKeyedListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::comments()), that));
        that->setAttributeConductor(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::conductor()), that));
        that->setAttributeComposers(make_unique<tag::AttributeTypes::ListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::composers()), that));
        that->setAttributeCopyright(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::copyright()), that));
        that->setAttributeDiscNumber(make_unique<tag::AttributeTypes::PlainUintAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::discNumber()), that));
        that->setAttributeDiscsTotal(make_unique<tag::AttributeTypes::PlainUintAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::discsTotal()), that));
        that->setAttributeEncodedBy(make_unique<tag::AttributeTypes::ListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::encodedBy()), that));
        that->setAttributeEncodingSettings(make_unique<tag::AttributeTypes::ListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::encodingSettings()), that));
        that->setAttributeEncodingTime(make_unique<tag::AttributeTypes::PlainTimeAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::encodingTime()), that));
        that->setAttributeEventTimingCodesUnit(make_unique<tag::AttributeTypes::PlainUintAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::eventTimingCodesUnit()), that));
        that->setAttributeEventTimingCodesValues(
            make_unique<tag::AttributeTypes::UintValuedUintKeyedListUintAttribute>(
                tag::AttributeTypes::type(tag::AttributeTypes::eventTimingCodesValues()), that));
        that->setAttributeFileOwner(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::fileOwner()), that));
        that->setAttributeFileType(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::fileType()), that));
        that->setAttributeGenres(
            make_unique<tag::AttributeTypes::ListStringAttribute>(tag::AttributeTypes::type(
                                                                      tag::AttributeTypes::genres()),
                                                                  that));
        that->setAttributeImages(
            make_unique<::ImagesAttribute>(tag::AttributeTypes::type(tag::AttributeTypes::images()),
                                           that));
        that->setAttributeInvolvedPeople(
            make_unique<tag::AttributeTypes::StringKeyedListStringAttribute>(
                tag::AttributeTypes::type(tag::AttributeTypes::involvedPeople()), that));
        that->setAttributeIsrc(
            make_unique<tag::AttributeTypes::PlainStringAttribute>(tag::AttributeTypes::type(
                                                                       tag::AttributeTypes::isrc()),
                                                                   that));
        that->setAttributeKey(
            make_unique<tag::AttributeTypes::PlainStringAttribute>(tag::AttributeTypes::type(
                                                                       tag::AttributeTypes::key()),
                                                                   that));
        that->setAttributeLanguages(make_unique<tag::AttributeTypes::ListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::languages()), that));
        that->setAttributeLength(
            make_unique<tag::AttributeTypes::PlainUintAttribute>(tag::AttributeTypes::type(
                                                                     tag::AttributeTypes::length()),
                                                                 that));
        that->setAttributeLyricists(make_unique<tag::AttributeTypes::ListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::lyricists()), that));
        that->setAttributeMediaType(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::mediaType()), that));
        that->setAttributeMoods(
            make_unique<tag::AttributeTypes::ListStringAttribute>(tag::AttributeTypes::type(
                                                                      tag::AttributeTypes::moods()),
                                                                  that));
        that->setAttributeMusicians(make_unique<tag::AttributeTypes::StringKeyedListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::musicians()), that));
        that->setAttributeOriginalAlbumTitle(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::originalAlbumTitle()), that));
        that->setAttributeOriginalArtist(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::originalArtist()), that));
        that->setAttributeOriginalFileName(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::originalFileName()), that));
        that->setAttributeOriginalLyricists(make_unique<tag::AttributeTypes::ListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::originalLyricists()), that));
        that->setAttributeOriginalReleaseTime(make_unique<tag::AttributeTypes::PlainTimeAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::originalReleaseTime()), that));
        that->setAttributeOtherTexts(
            make_unique<tag::AttributeTypes::StringKeyedListStringAttribute>(
                tag::AttributeTypes::type(tag::AttributeTypes::otherTexts()), that));
        that->setAttributePlaylistDelay(make_unique<tag::AttributeTypes::PlainUintAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::playlistDelay()), that));
        that->setAttributeProduced(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::produced()), that));
        that->setAttributePublisher(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::publisher()), that));
        that->setAttributeRadioStationName(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::radioStationName()), that));
        that->setAttributeRadioStationOwner(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::radioStationOwner()), that));
        that->setAttributeRecordingTime(make_unique<tag::AttributeTypes::PlainTimeAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::recordingTime()), that));
        that->setAttributeReleaseTime(make_unique<tag::AttributeTypes::PlainTimeAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::releaseTime()), that));
        that->setAttributeRemixedBy(make_unique<tag::AttributeTypes::ListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::remixedBy()), that));
        that->setAttributeSetSubtitle(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::setSubtitle()), that));
        that->setAttributeSongArtist(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::songArtist()), that));
        that->setAttributeSongCategory(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::songCategory()), that));
        that->setAttributeSongSubtitle(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::songSubtitle()), that));
        that->setAttributeSongTitle(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::songTitle()), that));
        that->setAttributeSongTitleSort(make_unique<tag::AttributeTypes::PlainStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::songTitleSort()), that));
        that->setAttributeTaggingTime(make_unique<tag::AttributeTypes::PlainTimeAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::taggingTime()), that));
        that->setAttributeTrackNumber(make_unique<tag::AttributeTypes::PlainUintAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::trackNumber()), that));
        that->setAttributeTracksTotal(make_unique<tag::AttributeTypes::PlainUintAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::tracksTotal()), that));
        that->setAttributeUniqueFileIdentifier(
            make_unique<tag::AttributeTypes::StringKeyedListBytesAttribute>(
                tag::AttributeTypes::type(tag::AttributeTypes::uniqueFileIdentifier()), that));
        that->setAttributeUrls(make_unique<tag::AttributeTypes::UintKeyedListStringAttribute>(
            tag::AttributeTypes::type(tag::AttributeTypes::urls()), that));

        that->attributeCdIdentifier()->setValidator(&cdIdentifierValidator);
        that->attributeCopyright()->setValidator(&copyrightValidator);
        that->attributeEventTimingCodesUnit()->setSelector(&eventTimingCodesUnits);
        that->attributeEventTimingCodesValues()->setKeySelector(&eventTimingCodesTypes);
        that->attributeEventTimingCodesValues()->setValueValidator(&eventTimingCodesValidator);
        that->attributeEventTimingCodesValues()->setUnit(that->attributeEventTimingCodesUnit());
        that->attributeFileType()->setSelector(FileTypeSelector::instance());
        that->attributeGenres()->setSelector(tag::CommonSelectors::genres());

        that->attributeImages()->setAllowsOnlyOneValue(false);
        that->attributeImages()->setDescriptionDependent(true);
        that->attributeImages()->setDescriptionMandatory(false);
        that->attributeImages()->setKeySelector(ImageTypeSelector::instance(), true);
        that->attributeImages()->setAllowsOnlyOneValue(ImageTypeSelector::fileIcon, true);
        that->attributeImages()->setAllowsOnlyOneValue(ImageTypeSelector::fileOtherIcon, true);
        that->attributeImages()->setAllowsOnlyOneValue(true);
        that->attributeImages()
            ->setDefaultValue({ImageTypeSelector::frontCover,
                               ImageTypeSelector::instance()->value(ImageTypeSelector::frontCover),
                               QImage()},
                              [](tag::KeyedListAttribute<uint, QImage> &att,
                                 tag::KeyedListAttribute<uint, QImage>::SingleValue &val) {
                                  auto num = att.nextDuplicateNumber();
                                  val.description = QObject::tr("%1 %2").arg(val.description,
                                                                             QString::number(num));
                              });

        that->attributeInvolvedPeople()->setKeySelector(tag::CommonSelectors::peopleFunctions());
        that->attributeLanguages()->setValidator(&languageValidator);
        that->attributeMediaType()->setSelector(MediaTypeSelector::instance());
        that->attributeMoods()->setSelector(tag::CommonSelectors::moods());
        that->attributeMusicians()->setKeySelector(tag::CommonSelectors::instruments());
        that->attributeProduced()->setValidator(&copyrightValidator);
        that->attributeOtherTexts()->setAllowsOnlyOneValue(true);

        that->attributeUniqueFileIdentifier()->setValueValidator(&uniqueFileIdentifierValidator);
        that->attributeUniqueFileIdentifier()->setAllowsOnlyOneValue(true);

        auto urls = that->attributeUrls();
        auto ut = UrlTypeSelector::instance();
        urls->setDescriptionDependent(false);
        urls->setKeySelector(ut, true);
        urls->setDescriptionDependent(true);
        for (auto u = ut->begin(); u != ut->end(); u++) {
            urls->setDescriptionDependent(u->second.type, u->second.descriptionDependent);
            urls->setAllowsOnlyOneValue(u->second.type, u->second.onlyOneValue);
        }
    }
};

unordered_map<QString, QString> const &Tag::genres() {
    return Genres;
}

Tag::Tag(File *f)
    : tag::Tag(f)
    , d(GREU_MKDET()) {
    d->init(this);
}

QString Tag::genreName(QString i) {
    try {
        return Genres.at(i.trimmed());
    } catch (...) {
        return {};
    }
}

void Tag::addAdditionalFrame(Frame const &f) {
    d->additionalFrames.push_back(f);
}

void Tag::clearAdditionalFrames() {
    d->additionalFrames.clear();
}

void Tag::clear() {
    tag::Tag::clear();
    d->additionalFrames.clear();
}

vector<Frame> const &Tag::additionalFrames() const {
    return d->additionalFrames;
}

StaticSelector &Tag::eventTimingCodesTypesSelector() {
    return eventTimingCodesTypes;
}

StaticSelector &Tag::eventTimingCodesUnitsSelector() {
    return eventTimingCodesUnits;
}

} // namespace greu::id3

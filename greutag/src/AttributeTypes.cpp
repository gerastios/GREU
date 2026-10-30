#include "AttributeTypes.h"
#include <QObject>
#include "AttributeType.h"

#define ID_DECL(NAME, UNAME, TYPE) uint NAME##_;

#define ID_GET_PRIV_IMPL(NAME, UNAME, TYPE) \
    uint NAME() const { \
        return NAME##_; \
    }

#define ID_GET_PUB_IMPL(NAME, UNAME, TYPE) \
    uint AttributeTypes::NAME() { \
        return Detail::instance()->NAME(); \
    }

#define T(A) QObject::tr(A)

#define R registerAttributeType(make_unique<AttributeType>

using namespace std;

namespace {

using namespace std;
using namespace greu::tag;

struct Detail {
    unordered_map<uint, unique_ptr<AttributeType>> types;
    uint lastUsed = 0;
    static mutex mtx;

    GREU_GRYLIZO_META_ATT(ID_DECL);

    static Detail *instance() {
        static Detail i;
        return &i;
    }

    using Si = AttributeType::StringsInitializer;

    // clang-format off
    static constexpr int __GREU_GRYLIZO_ATTRIBUTE_REG_BEGIN = __LINE__;
    Detail() {
        lock_guard<mutex> guard(mtx);
        filePath_ =                 R(Si{T("path"), T("Path"), T("file path"), T("File path"), T("file paths"), T("File paths")}, false, true));
        fileName_ =                 R(Si{T("name"), T("Name"), T("file name"), T("File name"), T("file names"), T("File names")}, false));
        fileExtension_ =            R(Si{T("extension"), T("Extension"), T("file extension"), T("File extension"), T("file extensions"), T("File extensions")}, false, true));
        albumArtist_ =              R(Si{T("artist"), T("Artist"), T("album artist"), T("Album artist"), T("album artists"), T("Album artists")}));
        albumArtistSort_ =          R(Si{T("artist (sort)"), T("Artist (sort)"), T("album artist (sort)"), T("Album artist (sort)"), T("album artists (sort)"), T("Album artists (sort)")}));
        albumTitle_ =               R(Si{T("title"), T("Title"), T("album title"), T("Album title"), T("album titles"), T("Album titles")}));
        albumTitleSort_ =           R(Si{T("title (sort)"), T("Title (sort)"), T("album title (sort)"), T("Album title (sort)"), T("album titles (sort)"), T("Album titles (sort)")}));
        beatsPerMinute_ =           R(Si{T("bpm"), T("BPM"), T("beats per minute"), T("Beats per minute"), T("beats per minute"), T("Beats per minute")}));
        cdIdentifier_ =             R(Si{T("CD identifier"), T("CD identifier"), T("CD identifier"), T("CD identifier"), T("CD identifiers"), T("CD identifiers")}));
        conductor_ =                R(Si{T("conductor"), T("Conductor"), T("conductor"), T("Conductor"), T("conductors"), T("Conductors")}));
        comments_ =                 R(Si{T("comment"), T("Comment"), T("comment"), T("Comment"), T("comments"), T("Comments")}));
        composers_ =                R(Si{T("composer"), T("Composer"), T("composer"), T("Composer"), T("composers"), T("Composers")}));
        copyright_ =                R(Si{T("copyright"), T("Copyright"), T("copyright ©"), T("Copyright ©"), T("copyrights"), T("Copyrights")}));
        discNumber_ =               R(Si{T("disc #"), T("Disc #"), T("disc number"), T("Disc number"), T("disc numbers"), T("Disc numbers")}));
        discsTotal_ =               R(Si{T("discs"), T("Discs"), T("discs total"), T("Discs total"), T("discs totals"), T("Discs totals")}));
        encodedBy_ =                R(Si{T("encoded by"), T("Encoded by"), T("encoded by"), T("Encoded by"), T("encoded by"), T("Encoded by")}));
        encodingSettings_ =         R(Si{T("encoding settings"), T("Encoding settings"), T("encoding settings"), T("Encoding settings"), T("encoding settings"), T("Encoding settings")}));
        encodingTime_ =             R(Si{T("encoded"), T("Encoded"), T("encoding time"), T("Encoding time"), T("encoding times"), T("Encoding times")}));
        eventTimingCodesUnit_ =     R(Si{T("unit"), T("Unit"), T("event timing code unit"), T("Event timing code unit"), T("event timing code units"), T("Event timing code units")}));
        eventTimingCodesValues_ =   R(Si{T("event code"), T("Event code"), T("event timing code"), T("Event timing code"), T("event timing codes"), T("Event timing codes"), T("event type"), T("Event type")}));
        fileOwner_ =                R(Si{T("file owner"), T("File owner"), T("file owner/licensee"), T("File owner/licensee"), T("file owners/licensees"), T("File owners/licensees")}));
        fileType_ =                 R(Si{T("file type"), T("File type"), T("file type"), T("File type"), T("file types"), T("File types")}));
        genres_ =                   R(Si{T("genre"), T("Genre"), T("genre"), T("Genre"), T("genres"), T("Genres")}));
        images_ =                   R(Si{T("image"), T("Image"), T("image"), T("Image"), T("images"), T("Images"), T("type"), T("Type")}));
        involvedPeople_ =           R(Si{T("involved"), T("Involved"), T("involved"), T("Involved"), T("involved People"), T("Involved People"), T("function"), T("Function")}));
        isrc_ =                     R(Si{T("ISRC"), T("ISRC"), T("international standard recording code"), T("International standard recording code"), T("international standard recording codes"), T("International standard recording codes")}));
        key_ =                      R(Si{T("key"), T("Key"), T("initial key"), T("Initial key"), T("initial keys"), T("Initial keys")}));
        languages_ =                R(Si{T("language"), T("Language"), T("language"), T("Language"), T("languages"), T("Languages")}));
        length_ =                   R(Si{T("length"), T("Length"), T("length (ms)"), T("Length (ms)"), T("lengths (ms)"), T("Lengths (ms)")}));
        lyricists_ =                R(Si{T("lyricist"), T("Lyricist"), T("lyricist"), T("Lyricist"), T("lyricists"), T("Lyricists")}));
        mediaType_ =                R(Si{T("media type"), T("Media type"), T("media type"), T("Media type"), T("media types"), T("Media types")}));
        moods_ =                    R(Si{T("mood"), T("Mood"), T("mood"), T("Mood"), T("moods"), T("Moods")}));
        musicians_ =                R(Si{T("musician"), T("Musician"), T("musician"), T("Musician"), T("musicians"), T("Musicians"), T("instrument"), T("Instrument")}));
        originalAlbumTitle_ =       R(Si{T("original title"), T("Original title"), T("original album title"), T("Original album title"), T("original album titles"), T("Original album titles")}));
        originalArtist_ =           R(Si{T("original artist"), T("Original artist"), T("original artist"), T("Original artist"), T("original artists"), T("Original artists")}));
        originalFileName_ =         R(Si{T("original file name"), T("Original file name"), T("original file name"), T("Original file name"), T("original file names"), T("Original file names")}));
        originalLyricists_ =        R(Si{T("original lyricist"), T("Original lyricist"), T("original lyricist"), T("Original lyricist"), T("original lyricists"), T("Original lyricists")}));
        originalReleaseTime_ =      R(Si{T("original released"), T("Original released"), T("original release time"), T("Original release time"), T("original release times"), T("Original release times")}));
        otherTexts_ =               R(Si{T("other text"), T("Other text"), T("other text information"), T("Other text information"), T("other text information"), T("Other text information"), T("description"), T("Description")}));
        playlistDelay_ =            R(Si{T("playlist delay"), T("Playlist delay"), T("playlist delay"), T("Playlist delay"), T("playlist delays"), T("Playlist delays")}));
        produced_ =                 R(Si{T("produced"), T("Produced"), T("produced ℗"), T("Produced ℗"), T("produced"), T("Produced")}));
        publisher_ =                R(Si{T("publisher"), T("Publisher"), T("publisher"), T("Publisher"), T("publishers"), T("Publishers")}));
        radioStationName_ =         R(Si{T("radio station"), T("Radio station"), T("radio station name"), T("Radio station name"), T("radio station names"), T("Radio station names")}));
        radioStationOwner_ =        R(Si{T("radio owner"), T("Radio owner"), T("radio station owner"), T("Radio station owner"), T("radio station owners"), T("Radio station owners")}));
        recordingTime_ =            R(Si{T("recorded"), T("Recording time"), T("recording time"), T("Recording time"), T("recording times"), T("Recording times")}));
        releaseTime_ =              R(Si{T("released"), T("Released"), T("release time"), T("Release time"), T("release times"), T("Release times")}));
        remixedBy_ =                R(Si{T("remixed by"), T("Remixed by"), T("remixed/modified by"), T("Remixed/modified by"), T("remixed/modified by"), T("Remixed/modified by")}));
        taggingTime_ =              R(Si{T("tagged"), T("Tagged"), T("tagging time"), T("Tagging time"), T("tagging times"), T("Tagging times")}));
        trackNumber_ =              R(Si{T("track #"), T("Track #"), T("track number"), T("Track number"), T("track numbers"), T("Track numbers")}));
        tracksTotal_ =              R(Si{T("tracks"), T("Tracks"), T("tracks total"), T("Tracks total"), T("tracks totals"), T("Tracks totals")}));
        setSubtitle_ =              R(Si{T("set subtitle"), T("Set subtitle"), T("set subtitle"), T("Set subtitle"), T("set subtitles"), T("Set subtitles")}));
        songArtist_ =               R(Si{T("artist"), T("Artist"), T("song artist"), T("Song artist"), T("song artists"), T("Song artists")}));
        songCategory_ =             R(Si{T("category"), T("Category"), T("song category"), T("Song category"), T("song categories"), T("Song categories")}));
        songSubtitle_ =             R(Si{T("subtitle"), T("Subtitle"), T("song subtitle"), T("Song subtitle"), T("song subtitles"), T("Song subtitles")}));
        songTitle_ =                R(Si{T("title"), T("Title"), T("song title"), T("Song title"), T("song titles"), T("Song titles")}));
        songTitleSort_ =            R(Si{T("title (sort)"), T("Title (sort)"), T("song title (sort)"), T("Song title (sort)"), T("song titles (sort)"), T("Song titles (sort)")}));
        uniqueFileIdentifier_ =     R(Si{T("identifier"), T("Identifier"), T("unique file identifier"), T("Unique file identifier"), T("unique file identifiers"), T("Unique file identifiers"), T("owner"), T("Owner")}));
        urls_ =                     R(Si{T("url"), T("URL"), T("url"), T("URL"), T("urls"), T("URLs"), T("type"), T("Type")}));
    }
    static constexpr int __GREU_GRYLIZO_ATTRIBUTE_REG_COUNT = __LINE__ - __GREU_GRYLIZO_ATTRIBUTE_REG_BEGIN - 4;
    static_assert (__GREU_GRYLIZO_ATTRIBUTE_TYPE_COUNT == __GREU_GRYLIZO_ATTRIBUTE_REG_COUNT, "Attribute type not registered");
    // clang-format on

    uint registerAttributeType(unique_ptr<AttributeType> value) {
        ++lastUsed;
        value->setId(lastUsed);
        types[lastUsed] = std::move(value);
        return lastUsed;
    }

    AttributeType *type(int id) const {
        if (types.contains(id))
            return types.at(id).get();
        return nullptr;
    }

    GREU_GRYLIZO_META_ATT(ID_GET_PRIV_IMPL);
};

} // namespace

mutex Detail::mtx;

namespace greu::tag {

GREU_GRYLIZO_META_ATT(ID_GET_PUB_IMPL);

uint AttributeTypes::registerAttributeType(unique_ptr<AttributeType> value) {
    return Detail::instance()->registerAttributeType(std::move(value));
}

AttributeType *AttributeTypes::type(uint id) {
    return Detail::instance()->type(id);
}

} // namespace greu::tag

#undef ID_DECL
#undef ID_GET_PRIV_IMPL
#undef ID_GET_PUB_IMPL
#undef T
#undef R

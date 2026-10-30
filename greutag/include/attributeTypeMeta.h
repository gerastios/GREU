#pragma once

// clang-format off
static int const __GREU_GRYLIZO_ATTRIBUTE_TYPE_BEGIN = __LINE__;
#define GREU_GRYLIZO_META_ATT(MACRO) \
    MACRO(filePath, FilePath, PlainStringAttribute) \
    MACRO(fileName, FileName, PlainStringAttribute) \
    MACRO(fileExtension, FileExtension, PlainStringAttribute) \
    MACRO(albumArtist, AlbumArtist, PlainStringAttribute) \
    MACRO(albumArtistSort, AlbumArtistSort, PlainStringAttribute) \
    MACRO(albumTitle, AlbumTitle, PlainStringAttribute) \
    MACRO(albumTitleSort, AlbumTitleSort, PlainStringAttribute) \
    MACRO(beatsPerMinute, BeatsPerMinute, PlainUintAttribute) \
    MACRO(cdIdentifier, CdIdentifier, PlainBytesAttribute) \
    MACRO(conductor, Conductor, PlainStringAttribute) \
    MACRO(comments, Comments, StringKeyedListStringAttribute) \
    MACRO(composers, Composers, ListStringAttribute) \
    MACRO(copyright, Copyright, PlainStringAttribute) \
    MACRO(discNumber, DiscNumber, PlainUintAttribute) \
    MACRO(discsTotal, DiscsTotal, PlainUintAttribute) \
    MACRO(encodedBy, EncodedBy, ListStringAttribute) \
    MACRO(encodingSettings, EncodingSettings, ListStringAttribute) \
    MACRO(encodingTime, EncodingTime, PlainTimeAttribute) \
    MACRO(eventTimingCodesUnit, EventTimingCodesUnit, PlainUintAttribute) \
    MACRO(eventTimingCodesValues, EventTimingCodesValues, UintValuedUintKeyedListUintAttribute) \
    MACRO(fileOwner, FileOwner, PlainStringAttribute) \
    MACRO(fileType, FileType, PlainStringAttribute) \
    MACRO(genres, Genres, ListStringAttribute) \
    MACRO(images, Images, UintKeyedListImageAttribute) \
    MACRO(involvedPeople, InvolvedPeople, StringKeyedListStringAttribute) \
    MACRO(isrc, Isrc, PlainStringAttribute) \
    MACRO(key, Key, PlainStringAttribute) \
    MACRO(languages, Languages, ListStringAttribute) \
    MACRO(length, Length, PlainUintAttribute) \
    MACRO(lyricists, Lyricists, ListStringAttribute) \
    MACRO(mediaType, MediaType, PlainStringAttribute) \
    MACRO(moods, Moods, ListStringAttribute) \
    MACRO(musicians, Musicians, StringKeyedListStringAttribute) \
    MACRO(originalAlbumTitle, OriginalAlbumTitle, PlainStringAttribute) \
    MACRO(originalFileName, OriginalFileName, PlainStringAttribute) \
    MACRO(originalLyricists, OriginalLyricists, ListStringAttribute) \
    MACRO(originalReleaseTime, OriginalReleaseTime, PlainTimeAttribute) \
    MACRO(originalArtist, OriginalArtist, PlainStringAttribute) \
    MACRO(otherTexts, OtherTexts, StringKeyedListStringAttribute) \
    MACRO(playlistDelay, PlaylistDelay, PlainUintAttribute) \
    MACRO(produced, Produced, PlainStringAttribute) \
    MACRO(publisher, Publisher, PlainStringAttribute) \
    MACRO(radioStationName, RadioStationName, PlainStringAttribute) \
    MACRO(radioStationOwner, RadioStationOwner, PlainStringAttribute) \
    MACRO(recordingTime, RecordingTime, PlainTimeAttribute) \
    MACRO(releaseTime, ReleaseTime, PlainTimeAttribute) \
    MACRO(remixedBy, RemixedBy, ListStringAttribute) \
    MACRO(taggingTime, TaggingTime, PlainTimeAttribute) \
    MACRO(trackNumber, TrackNumber, PlainUintAttribute) \
    MACRO(tracksTotal, TracksTotal, PlainUintAttribute) \
    MACRO(setSubtitle, SetSubtitle, PlainStringAttribute) \
    MACRO(songArtist, SongArtist, PlainStringAttribute) \
    MACRO(songCategory, SongCategory, PlainStringAttribute) \
    MACRO(songSubtitle, SongSubtitle, PlainStringAttribute) \
    MACRO(songTitle, SongTitle, PlainStringAttribute) \
    MACRO(songTitleSort, SongTitleSort, PlainStringAttribute) \
    MACRO(uniqueFileIdentifier, UniqueFileIdentifier, StringKeyedListBytesAttribute) \
    MACRO(urls, Urls, UintKeyedListStringAttribute)
    static constexpr int __GREU_GRYLIZO_ATTRIBUTE_TYPE_COUNT = __LINE__ - __GREU_GRYLIZO_ATTRIBUTE_TYPE_BEGIN - 2;
// clang-format on

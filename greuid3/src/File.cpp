#include "File.h"
#include <QFile>
#include <QFileInfo>
#include "Reader.h"
#include "Tag.h"
#include "Writer.h"
#include "greucore/include/FileException.h"
#include <QSaveFile>

#define BUFFERSZ 1024

using namespace std;

namespace {
struct FileContent {
    QFile &file;
    uchar *mapped = nullptr;
    QByteArray fileData;

    explicit FileContent(QFile &f)
        : file(f) {
        auto const size = file.size();
        if (size <= 0)
            return;
        mapped = file.map(0, size);
        if (mapped)
            fileData = QByteArray::fromRawData(reinterpret_cast<char const *>(mapped),
                                               static_cast<qsizetype>(size));
        else
            fileData = file.readAll();
    }

    ~FileContent() {
        fileData = QByteArray();
        if (mapped)
            file.unmap(mapped);
    }

    QByteArray const &data() const { return fileData; }
};
} // namespace

namespace greu::id3 {

using namespace core;

struct File::Detail {
    unique_ptr<id3::Tag> tag;
    vector<TagLocation> locations;
    uint64_t audioSize = 0;
    int padding = 0;
    bool loaded = false;

    vector<TagLocation> scanTags(QByteArray const &data) {
        vector<TagLocation> locations;
        size_t const n = data.size();
        size_t limit = n;

        bool hasV1 = false;
        TagLocation v1;
        if (n >= Reader::V1TagSize && Reader::isV1Tag(data, n - Reader::V1TagSize)) {
            hasV1 = true;
            v1.offset = n - Reader::V1TagSize;
            v1.length = Reader::V1TagSize;
            v1.majorVersion = 1;
            v1.revision = (data.at(v1.offset + 125) == '\0' && data.at(v1.offset + 126) != '\0')
                              ? 1
                              : 0;
            limit = static_cast<qsizetype>(v1.offset);
        }

        size_t from = 0;
        while (from + Reader::V2HeaderSize <= limit) {
            auto const idx = data.indexOf("ID3", from);
            if (idx < 0 || (size_t) idx >= limit)
                break;
            if (!Reader::looksLikeV2Tag(data, idx)) {
                from = idx + 1;
                continue;
            }
            int64_t total = Reader::v2TagSize(data, idx);
            if ((size_t) idx + total > limit)
                total = limit - idx;

            TagLocation loc;
            loc.offset = idx;
            loc.length = total;
            loc.majorVersion = static_cast<uint8_t>(data.at(idx + 3));
            loc.revision = static_cast<uint8_t>(data.at(idx + 4));
            locations.push_back(loc);
            from = idx + static_cast<size_t>(total);
        }

        if (hasV1)
            locations.push_back(v1);
        return locations;
    }

    void analyzeFile(QFile &physFile, File *that) {
        assert(physFile.isOpen() && "File not open");
        locations.clear();
        audioSize = 0;
        loaded = false;
        tag = make_unique<id3::Tag>(that);

        FileContent const content(physFile);
        QByteArray const &data = content.data();

        locations = scanTags(data);

        int64_t tagBytes = 0;
        for (auto const &loc : locations) {
            Reader reader;
            Tag part{that};
            QByteArray const chunk = data.mid(static_cast<size_t>(loc.offset),
                                              static_cast<size_t>(loc.length));
            loc.isV1() ? reader.readV1(chunk, part) : reader.readV2(chunk, part);

            QString const prefix
                = QStringLiteral("ID3v%1 tag at offset %2: ").arg(loc.majorVersion).arg(loc.offset);

            tag->merge(part);
            tagBytes += loc.length;
        }

        audioSize = data.size() - tagBytes;
        loaded = true;
    }

    bool saveContent(File *that) {

        QFile in(that->info().filePath());
        if (!in.open(QIODevice::ReadOnly))
            throw FileException(in.errorString(), that->info());

        Writer writer;
        QByteArray const newTag = writer.write(*tag);

        QSaveFile out(that->info().filePath());
        int64_t audioBytes = 0;
        {
            FileContent const content(in);
            QByteArray const &data = content.data();
            auto const locations = scanTags(
                data); // re-scan: the file may have changed meanwhile

            if (!out.open(QIODevice::WriteOnly))
                throw FileException(out.errorString(), that->info());

            auto const writeFailed = [&]() {
                throw FileException(out.errorString(), that->info());
            };

            if (!newTag.isEmpty() && out.write(newTag) != newTag.size())
                writeFailed();

            // Copy everything that is not part of a tag, in original order.
            auto const copyRange = [&](size_t from, size_t to) {
                if (to <= from)
                    return true;
                int64_t const length = to - from;
                if (out.write(data.constData() + from, length) != length)
                    return false;
                audioBytes += length;
                return true;
            };

            size_t pos = 0;
            for (auto const &loc : locations) {
                if (!copyRange(pos, static_cast<qsizetype>(loc.offset)))
                    writeFailed();
                pos = static_cast<qsizetype>(loc.offset + loc.length);
            }
            if (!copyRange(pos, data.size()))
                writeFailed();
        }
        in.close(); // must be closed before the temporary file replaces it (Windows)

        if (!out.commit())
            throw FileException(out.errorString(), that->info());

        locations.clear();
        if (!newTag.isEmpty()) {
            TagLocation loc;
            loc.offset = 0;
            loc.length = newTag.size();
            loc.majorVersion = 4;
            loc.revision = 0;
            locations.push_back(loc);
        }
        audioSize = audioBytes;
        tag->setModified(false);
        return true;
    }

    // bool saveContent(File *) {
    // auto buffer = readOtherContent(that->info());
    // auto tag = id3V2Tag->render();
    // QFile file(that->info().filePath());
    // if (!file.open(QIODevice::WriteOnly))
    //     throw FileException(file.errorString(), that->info());
    // if (tag.size() > 0)
    //     file.write(tag);
    // file.write(buffer);
    // file.close();
    // id3V2Tag->setPosition(0);
    // id3V2Tag->setTotalLength(tag.size());
    // auto atts = id3V2Tag->attributes();
    // for (auto a = atts->begin(); a != atts->end(); a++)
    //     a->second->save(); // actually set modified to false
    //     return true;
    // }

    // QByteArray readOtherContent(QFileInfo const &info) {
    //     QFile file(info.filePath());
    //     if (!file.open(QIODevice::ReadOnly))
    //         throw FileException(file.errorString(), info);
    //     QByteArray result;
    //     auto pos = id3V2Tag->position();
    //     auto len = id3V2Tag->totalLength();
    //     if (len == 0) {
    //         result = file.readAll();
    //     } else if (pos == 0) {
    //         file.seek(len);
    //         result = file.readAll();
    //     } else {
    //         file.seek(0);
    //         auto b1 = file.read(pos);
    //         file.seek(pos + len);
    //         result = b1 + file.readAll();
    //     }
    //     return result;
    // }
};

File::File(QFileInfo const &info)
    : tag::File(info)
    , d(GREU_MKDET()) {
    d->tag = make_unique<id3::Tag>(this);
}

tag::Tag *File::tag() const {
    return d->tag.get();
}

void File::analyzeFile(QFile *file) {
    return d->analyzeFile(*file, this);
}

bool File::saveContent() {
    return d->saveContent(this);
}

} // namespace greu::id3

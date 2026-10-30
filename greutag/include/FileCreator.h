#pragma once
#include "greucore/include/greuGlobal.h"
#include <memory>

class QFileInfo;

namespace greu::tag {

class File;

struct GREU_PUBLIC(TAG) FileCreator {

    virtual bool handles(QFileInfo const &) const = 0;
    virtual std::unique_ptr<File> createFile(QFileInfo const &) = 0;

};

} // namespace greu::tag

#define GREU_TAG_FILE_CREATOR_PLUGIN_API_VERSION 1

#define GREU_TAG_FILE_CREATOR_PLUGIN_API_VERSION_SYMBOL "greu_tag_file_creator_plugin_api_version"
#define GREU_TAG_FILE_CREATOR_PLUGIN_CREATE_SYMBOL "greu_tag_create_file"

#if defined(Q_OS_WIN) || defined(_WIN32)
#define GREU_TAG_FILE_CREATOR_EXPORT __declspec(dllexport)
#else
#define GREU_TAG_FILE_CREATOR_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
typedef int (*TagFileCreatorApiVersionFunction)();
typedef greu::tag::FileCreator *(*TagCreateFileCreatorFunction)();
}



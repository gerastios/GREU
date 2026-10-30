#pragma once

#include "greucore/include/Log.h"
#include "greutag/include/FileContainer.h"
#include "greuui/include/SavableDispatcher.h"

namespace greu::tag {
class File;
class Attribute;
class AudioCollection;
} // namespace greu::tag

namespace greu::grylizo {

class Dispatcher : public ui::SavableDispatcher {
public:
    Dispatcher();

    void setAudioPath(QString const &);
    void setPackageSize(uint);
    void setMessageSize(uint);
    void setNoSplash(bool = true);

    virtual void initialize() override;
    virtual ui::MainWindow *mainWindow() const override;
    virtual size_t modifiedCount() const override;
    virtual size_t savedCount() const override;

    core::Log &log() const;
    tag::AudioCollection *audioCollection() const;

    void tagSelectedFiles(tag::Attribute *) const;
    void tagContainer(tag::Attribute *) const;

    greu::tag::FileContainer *fileContainer() const;

    QString audioPath() const;

    virtual void saveOtherSettings() override;
    virtual void restoreOtherSettings() override;

private:
    virtual void updateUi() override;
    virtual core::Saver *saver() const override;

    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo

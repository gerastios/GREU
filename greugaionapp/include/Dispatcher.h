#pragma once
#include "greugaion/include/Dispatcher.h"
#include "greucore/include/Uuid.h"

namespace greu::movies {
struct SearchOptions;
}
namespace greu::gaion::app {

class GREU_PUBLIC(GAIONAPP) Dispatcher : public gaion::Dispatcher {
public:
    Dispatcher();
    virtual void initialize() override;
    virtual bool canClose() override { return true; }
    virtual ui::MainWindow *mainWindow() const override;

    void showMovies() const;
    void showMovieCollections() const;
    void showCollections() const;
    void showSeries() const;

    void showMoviesByGenre() const;
    void showSeriesByGenre() const;

    static void sortByName(core::Uuids &);
    static void sortByDate(core::Uuids &);

    void search(movies::SearchOptions const &);

    void setNoSplash(bool);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::app

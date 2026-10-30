#include "Application.h"
#include "greucore/include/Exception.h"
#include <iostream>

using namespace std;
using namespace greu::core;

int main(int argc, char **argv) {
    try {
        Application a(argc, argv);
        return a.run();
    } catch (greu::core::Exception &e) {
        cout << e.text().toStdString() << endl;
        exit(8);
    }
    // TheMovieDatabase t(*(Language::fromCode2(Language::Code2::German).value()),
    //                    *(Language::fromCode2(Language::Code2::English).value()));
    // {
    //     auto r = PersonRequester(65);
    //     auto p = r.read();
    //     cout << p.name.toStdString() << endl;
    // }
    // {
    //     auto r = MovieRequester(562);
    //     auto m = r.read();
    //     cout << m.title.toStdString() << endl;
    //     cout << m.overview.toStdString() << endl;
    //     CreditRequester cr = CreditRequester(r.creditUrl());
    //     auto c = cr.read();
    //     for (auto p : c.castCredits)
    //         cout << p.character.toStdString() << endl;
    //     for (auto p : c.crewCredits)
    //         cout << p.job.toStdString() << endl;
    // }
    // {
    //     auto r = SeriesRequester(3452);
    //     auto t = r.read();
    //     cout << t.name.toStdString() << endl;
    //     cout << t.overview.toStdString() << endl;
    //     for(auto c: t.creators) cout << c.personId << endl;
    //         CreditRequester cr = CreditRequester(r.creditUrl());
    //         auto c = cr.read();
    //         for (auto p : c.castCredits)
    //             cout << p.character.toStdString() << endl;
    //         for (auto p : c.crewCredits)
    //             cout << p.job.toStdString() << endl;
    // }
    // {
    //     auto r = MovieCollectionRequester(1570);
    //     auto t = r.read();
    //     cout << t.name.toStdString() << endl;
    //     cout << t.overview.toStdString() << endl;
    //     for(auto c: t.movieIds) cout << c << endl;
    // }
    // {
    //     auto r = SeasonRequester(3452, 1, "EN");
    //     auto m = r.read();
    //     cout << m.name.toStdString() << endl;
    //     cout << m.overview.toStdString() << endl;
    //     CreditRequester cr = CreditRequester(r.creditUrl());
    //     auto c = cr.read();
    //     for (auto p : c.castCredits)
    //         cout << p.character.toStdString() << endl;
    //     for (auto p : c.crewCredits)
    //         cout << p.job.toStdString() << endl;
    // }
    // {
    //     auto r = EpisodeRequester(3452, 1, 1, "EN");
    //     auto m = r.read();
    //     cout << m.name.toStdString() << endl;
    //     cout << m.overview.toStdString() << endl;
    //     CreditRequester cr = CreditRequester(r.creditUrl());
    //     auto c = cr.read();
    //     for (auto p : c.castCredits)
    //         cout << p.character.toStdString() << endl;
    //     for (auto p : c.crewCredits)
    //         cout << p.job.toStdString() << endl;
    //     for (auto p : c.guestStarsCredits)
    //         cout << p.character.toStdString() << endl;
    // }

    return 0;
}

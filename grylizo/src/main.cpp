#include "Application.h"
#include "Dispatcher.h"

using namespace greu::grylizo;

int main(int argc, char *argv[]) {
    Application a(argc, argv);
    Dispatcher d;
    a.parseCommandLine(&d);
    d.initialize();
    return a.exec();
}

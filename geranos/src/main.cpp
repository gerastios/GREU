#include "Application.h"
#include "Dispatcher.h"

using namespace greu::geranos;

int main(int argc, char *argv[]) {
    Application a(argc, argv);
    Dispatcher d;
    d.initialize();
    return a.exec();
}

#include "Application.h"
#include "Dispatcher.h"

using namespace greu::glaphyria;

int main(int argc, char *argv[]) {
    Application a(argc, argv);
    Dispatcher d;
    d.initialize();
    return a.exec();
}

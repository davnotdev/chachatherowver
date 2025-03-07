#include "mainui.hpp"
#include "serialin.hpp"

int main(int argc, char* argv[]) {
    auto t = initDataReader();

    MainUI ui = MainUI();
    ui.run();

    terminateDataReader();
    t.join();
}

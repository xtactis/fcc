#include "IR2AVR.hpp"

int main(int argc, char **argv) {
    std::vector<IR> ir = { // numbers are random, don't worry about it
        (static_cast<int>(IRt::nop)   << 24) | 0x000000,
        (static_cast<int>(IRt::pop)   << 24) | 0xF00000,
        (static_cast<int>(IRt::push)  << 24) | 0xE00000,
        (static_cast<int>(IRt::movcr) << 24) | 0x2A3000,
        (static_cast<int>(IRt::movrr) << 24) | 0xA38000,
        (static_cast<int>(IRt::retc)  << 24) | 0xDEAD00,
        (static_cast<int>(IRt::ret)   << 24) | 0x000000,
    };
    IR2AVR(ir);
    return 0;
}

#include <vector>
#include "IR2AVR.hpp"

int main(int argc, char **argv) {
    std::vector<IR> ir = {
        {IRt::nop, {}, 0},
        {IRt::pop, {0xF}, 0},
        {IRt::push, {0xE}, 0},
        {IRt::mov, {0x2A, 3}, 0x2},
        {IRt::mov, {5, 8}, 0},
        {IRt::retc, {0xDEAD}, 0x1},
        {IRt::ret, {}, 0},
        {IRt::add, {3, 5}, 0},
        {IRt::add, {71, 3}, 0x2},
    };
    IR2AVR(ir);
    return 0;
}

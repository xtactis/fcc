#include <vector>
#include "IR2AVR.hpp"

inline u16 swapendiannes16(u16 x) {
    return ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
}

inline u32 swapendiannes(u32 x) {
    return (swapendiannes16((x & 0xFFFF0000) >> 16) << 16) | swapendiannes16(x & 0x0000FFFF);
}

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
    std::vector<u32> AVRinstructions = {
        0x0C942A00,
        0x0C943600,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x0C943400,
        0x11240000,
        0x1FBE0000,
        0xCFE50000,
        0xD4E00000,
        0xDEBF0000,
        0xCDBF0000,
        0x0E944500,
        0x0C944800,
        0x0C940000,
        0x1F920000,
        0x0F920000,
        0x0FB60000,
        0x0F920000,
        0x11240000,
        0x8F930000,
        0x8BB30000,
        0x880F0000,
        0x8BBB0000,
        0x8F910000,
        0x0F900000,
        0x0FBE0000,
        0x0F900000,
        0x1F900000,
        0x18950000,
        0x8AE20000,
        0x90E00000,
        0x08950000,
        0xF8940000,
        0xFFCF0000,
    };
    for (u32 &e: AVRinstructions) {
        //printf("%x\n", e);
        e = swapendiannes(e);
        //printf("%x\n", e);
    }
    //IR2AVR(ir, AVRinstructions);
    printAVR(AVRinstructions);
    return 0;
}

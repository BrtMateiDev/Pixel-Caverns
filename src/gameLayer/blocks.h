#ifndef MYGAME_BLOCKS_H
#define MYGAME_BLOCKS_H

#pragma once
#include <cstdint>

struct Block {
    enum {
        air=0,
        dirt,
        stone,

        BLOCKS_COUNT,
    };

    std::uint16_t type=0; //each block will be represented by a 16-bit number
};

#endif //MYGAME_BLOCKS_H
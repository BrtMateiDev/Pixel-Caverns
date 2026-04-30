#ifndef MYGAME_BLOCKS_H
#define MYGAME_BLOCKS_H

#pragma once
#include <cstdint>

struct Block {
    enum {
        air,
        stone,
        dirt,
        grass,
        PLACEHOLDER,

        BLOCKS_COUNT,
    };

    std::uint16_t type = 0; //each block will be represented by a 16-bit number

    void sanitize() {
        if (type >= BLOCKS_COUNT) type = 0;
    }

    bool isCollidable() {
        switch (type) {
            case air:
                //case [NAME]_bg;
                return false;
            default:
                return true;
        }
        return false;
    }
};

#endif

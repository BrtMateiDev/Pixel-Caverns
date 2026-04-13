#ifndef MYGAME_ENTITYIDHOLDER_H
#define MYGAME_ENTITYIDHOLDER_H

#pragma once
#include <cstdint>
#include <unordered_map>
#include <memory>
#include "entities/slime.h"
#include "entity.h"

constexpr static std::uint64_t PLAYER_ID = 1;
//constexpr means this will be calculated during compilation, imagine having to generate this 64-bit number everytime you open the game

struct EntityIdHolder {
    std::uint64_t idCounter = 2;

    std::uint64_t getEntityIdAndIncrement();
};

struct EntityHolder {
    EntityIdHolder idHolder;
    std::unordered_map<std::uint64_t, std::unique_ptr<Entity> > entities;
    //The smart pointer will handle the heap memory for us!
};
#endif

#include "pickaxe.h"

#include "blocks.h"

PickaxeProperties PickaxeRegistry[PickaxeType::COUNT];

void initPickaxeRegistry() {
    /*COPY PASTE:
    PickaxeRegistry[PickaxeType::] = {"Pickaxe", .0f, .0f, , {
        {Block::, },
        {Block::, },
    }};
    */

    // Wood
    PickaxeRegistry[PickaxeType::Wood] = {
        "Wood Pickaxe", 0.5f, 2.0f, 0, {}
    };

    // Stone
    PickaxeRegistry[PickaxeType::Stone] = {
        "Stone Pickaxe", 1.0f, 2.5f, 1, {
            {Block::stone_shallow, 15}
        }
    };

    // Iron
    PickaxeRegistry[PickaxeType::Iron] = {
        "Iron Pickaxe", 2.0f, 3.0f, 2, {
            {Block::coal, 10},
            {Block::iron, 15}
        }
    };

    // Silver
    PickaxeRegistry[PickaxeType::Silver] = {
        "Silver Pickaxe", 3.0f, 3.5f, 3, {
            {Block::coal, 5},
            {Block::iron, 10},
            {Block::silver, 15}
        }
    };

    // Gold
    PickaxeRegistry[PickaxeType::Gold] = {
        "Gold Pickaxe", 4.0f, 3.5f, 4, {
            {Block::iron, 15},
            {Block::gold, 30}
        }
    };

    // Diamond
    PickaxeRegistry[PickaxeType::Diamond] = {
        "Diamond Pickaxe", 5.0f, 4.0f, 5, {
            {Block::iron, 30},
            {Block::silver, 10},
            {Block::gold, 5},
            {Block::diamond, 15},
        }
    };

    // Shhh, don't tell anyone
    PickaxeRegistry[PickaxeType::God] = {
        "Wood Pickaxe", 999.f, 999.0f, 6, {}
    };
}

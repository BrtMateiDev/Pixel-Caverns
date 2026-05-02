#include "blocks.h"

BlockProperties BlockRegistry[Block::BLOCKS_COUNT];

bool Block::isCollidable() {
    return BlockRegistry[type].isCollidable;
}

bool Block::isMineable() {
    return BlockRegistry[type].isMineable;
}

float Block::getDurability() {
    return BlockRegistry[type].durability;
}

/*
DEFAULTS:
name -> "Unknown", isCollidable and isMinable -> true, durability -> 1.0f,
indexes -> 0, dropType -> -1

COPY PASTE:

BlockRegistry[Block::].name = "";
BlockRegistry[Block::].isCollidable = ;
BlockRegistry[Block::].isMineable = ;
BlockRegistry[Block::].durability = ;
BlockRegistry[Block::].textureIndex = ;
BlockRegistry[Block::].bgIndex = ;
BlockRegistry[Block::].oreIndex = ;
BlockRegistry[Block::].dropType = ;

*/

void initBlockRegistry() {
    // Air
    BlockRegistry[Block::air].isCollidable = false;
    BlockRegistry[Block::air].isMineable = false;

    // Dirt
    BlockRegistry[Block::dirt].name = "Dirt";
    BlockRegistry[Block::dirt].durability = 0.5f;
    BlockRegistry[Block::dirt].textureIndex = 1;
    BlockRegistry[Block::dirt].bgIndex = 1;

    // Grass
    BlockRegistry[Block::grass].name = "Grass";
    BlockRegistry[Block::grass].durability = 0.5f;
    BlockRegistry[Block::grass].textureIndex = 2;
    BlockRegistry[Block::grass].bgIndex = 1;

    // Stone Shallow
    BlockRegistry[Block::stone_shallow].name = "Stone";
    BlockRegistry[Block::stone_shallow].durability = 1.0f;
    BlockRegistry[Block::stone_shallow].textureIndex = 3;
    BlockRegistry[Block::stone_shallow].bgIndex = 2;


    // Coal
    BlockRegistry[Block::coal].name = "Coal";
    BlockRegistry[Block::coal].durability = 2.0f;
    BlockRegistry[Block::coal].textureIndex = 3;
    BlockRegistry[Block::coal].bgIndex = 2;
    BlockRegistry[Block::coal].oreIndex = 1;

    // Iron
    BlockRegistry[Block::iron].name = "Iron";
    BlockRegistry[Block::iron].durability = 3.0f;
    BlockRegistry[Block::iron].textureIndex = 3;
    BlockRegistry[Block::iron].bgIndex = 2;
    BlockRegistry[Block::iron].oreIndex = 2;

    // Stone Deepslate
    BlockRegistry[Block::stone_deepslate].name = "Stone";
    BlockRegistry[Block::stone_deepslate].durability = 1.5f;
    BlockRegistry[Block::stone_deepslate].textureIndex = 4;
    BlockRegistry[Block::stone_deepslate].bgIndex = 2;

    // PLACEHOLDERS
    BlockRegistry[Block::PLACEHOLDER].isCollidable = true;
    BlockRegistry[Block::PLACEHOLDER].isMineable = false;
    BlockRegistry[Block::PLACEHOLDER].textureIndex = 5;

    BlockRegistry[Block::PLACEHOLDER_BG].isCollidable = false;
    BlockRegistry[Block::PLACEHOLDER_BG].isMineable = false;
}


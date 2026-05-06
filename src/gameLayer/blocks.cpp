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
name -> "Unknown", isCollidable and isMinable -> true,
durability -> 1.0f, indexes -> 0, dropType -> -1

COPY PASTE:

BlockRegistry[Block::].name = "";
BlockRegistry[Block::].isCollidable = false;
BlockRegistry[Block::].isMineable = false;
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
    BlockRegistry[Block::grass].dropType = 1;

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

    // Silver
    BlockRegistry[Block::silver].name = "Silver";
    BlockRegistry[Block::silver].durability = 2.5f;
    BlockRegistry[Block::silver].textureIndex = 3;
    BlockRegistry[Block::silver].bgIndex = 2;
    BlockRegistry[Block::silver].oreIndex = 3;

    //Gold
    BlockRegistry[Block::gold].name = "Gold";
    BlockRegistry[Block::gold].durability = 4.0f;
    BlockRegistry[Block::gold].textureIndex = 3;
    BlockRegistry[Block::gold].bgIndex = 2;
    BlockRegistry[Block::gold].oreIndex = 4;

    //Diamond
    BlockRegistry[Block::diamond].name = "Diamond";
    BlockRegistry[Block::diamond].durability = 6.0f;
    BlockRegistry[Block::diamond].textureIndex = 3;
    BlockRegistry[Block::diamond].bgIndex = 2;
    BlockRegistry[Block::diamond].oreIndex = 5;

    // Stone Deepslate
    BlockRegistry[Block::stone_deepslate].name = "Stone";
    BlockRegistry[Block::stone_deepslate].durability = 2.0f;
    BlockRegistry[Block::stone_deepslate].textureIndex = 4;
    BlockRegistry[Block::stone_deepslate].bgIndex = 2;
    BlockRegistry[Block::stone_deepslate].dropType = 3;

    // Stone End (end of the demo)
    BlockRegistry[Block::stone_end].isCollidable = true;
    BlockRegistry[Block::stone_end].isMineable = false;
    BlockRegistry[Block::stone_end].textureIndex = 5;

    // MISC
    BlockRegistry[Block::INV].isCollidable = true;
    BlockRegistry[Block::INV].isMineable = false;
    BlockRegistry[Block::INV].textureIndex = 0;

    BlockRegistry[Block::PLACEHOLDER].isCollidable = true;
    BlockRegistry[Block::PLACEHOLDER].isMineable = false;
    BlockRegistry[Block::PLACEHOLDER].textureIndex = 5;

    BlockRegistry[Block::PLACEHOLDER_BG].isCollidable = false;
    BlockRegistry[Block::PLACEHOLDER_BG].isMineable = false;
}


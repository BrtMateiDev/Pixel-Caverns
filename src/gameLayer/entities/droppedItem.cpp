#include "droppedItem.h"
#include "helpers.h"
#include "entityIdHolder.h"

void DroppedItem::render(AssetManager &assetManager) {
    auto aabb = getRectangleForEntity(physics.transform, 1, 1);

    DrawTexturePro(
        assetManager.textures,
        getTextureAtlas(itemType, 4, 32, 32), //rendering the bottom row from the atlas
        aabb,
        {0, 0},
        0.0f,
        WHITE
    );
}

bool DroppedItem::update(float dt, EntityUpdateData entityUpdateData) {
    for (auto &e: entityUpdateData.entityHolder.entities) //iterate over entities
        if (e.first != entityUpdateData.ownId) //that are not the current entity
            if (e.second->getEntityType() == EntityType::EntityType_DroppedItem) {
                //that are also other items
                DroppedItem *other = reinterpret_cast<DroppedItem *>(e.second.get());
                if (itemType == other->itemType) //if the current entity stores the same item type
                    if (Vector2Distance(getPosition(), other->getPosition()) < 0.7) {
                        //and they are close
                        other->itemCounter += itemCounter; //then donate the entity
                        return 0; //and lastly, delete it
                    }
            }

    return true;
}

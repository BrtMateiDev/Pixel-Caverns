#include "physics.h"
#include "gameMap.h"
#include <raymath.h>

void PhysicalEntity::resolveConstraints(GameMap &mapData) {
    //In case of lag spikes, this function will put the player back
    upTouch=false;
    downTouch=false;
    leftTouch=false;
    rightTouch=false;

    Vector2 &pos=transform.pos;
    float distance=Vector2Distance(lastPosition, pos);
    if (distance==0) return; //means no movement happened

    float GRANULARITY=0.8; //arbitrary value

    if (distance<=GRANULARITY) {
        checkCollisionOnce(pos, mapData);
    }
    else {
        //Moving the player in small increments
        Vector2 newPos=lastPosition;
        Vector2 delta=pos-lastPosition;
        delta=Vector2Normalize(delta);
        delta*=GRANULARITY*0.99;

        do {
            newPos+=delta;
            Vector2 posTest=newPos;
            checkCollisionOnce(newPos, mapData);
            //if a collision happened
            if (newPos!=posTest) {
                pos=newPos;
                goto end; //go to the last (end) step of the function
            }
        }while (Vector2Length((newPos+delta)-pos)>GRANULARITY);
        //one more check
        checkCollisionOnce(pos, mapData);
    }
    end:
    //Restrict the player from going outside of bounds/falling through the map
    if (pos.x-transform.w/2<0) pos.x=transform.w/2;
    if (pos.x+transform.w/2>mapData.w) pos.x=mapData.w-transform.w/2;
    if (pos.y+transform.h/2>mapData.h) pos.y=mapData.h-transform.h/2;

    //Prevention for gaining velocity when standing on a block
    if (leftTouch && velocity.x<0) velocity.x=0;
    if (rightTouch && velocity.x>0) velocity.x=0;

    if (upTouch && velocity.y<0) velocity.y=0;
    if (downTouch && velocity.y>0) velocity.y=0;
}

void PhysicalEntity::checkCollisionOnce(Vector2 &pos, GameMap &mapData) {
    //Delta represents how much the player moved
    Vector2 delta=pos-lastPosition;

    //Moving on the x-axis, notice the use of lastPosition for y and delta for x
    Vector2 newPos=performCollisionOnOneAxis(mapData, {pos.x, lastPosition.y}, {delta.x, 0});

    //Moving on the y-axis using the new value for x
    pos=performCollisionOnOneAxis(mapData, {newPos.x, pos.y}, {0, delta.y});
}

Vector2 PhysicalEntity::performCollisionOnOneAxis(GameMap &mapData, Vector2 pos, Vector2 delta) {
    if (delta.x==0 && delta.y==0) return pos;

    //The actual hitbox checks will be performed around the player, not the entire map
    Vector2 dimensions={transform.w, transform.h};

    int minX=floor(pos.x-dimensions.x/2.f-1);
    int maxX=ceil(pos.x+dimensions.x/2.f+1);
    int minY=floor(pos.y-dimensions.y/2.f-1);
    int maxY=ceil(pos.y+dimensions.y/2.f+1);

    //Making sure the values are within the map;
    minX=std::max(0, minX);
    minY=std::max(0, minY);
    maxX=std::min(mapData.w, maxX);
    maxY=std::min(mapData.h, maxY);

    //Checking the nearby area for collisions
    for (int y=minY; y<maxY; ++y)
        for (int x=minX; x<maxX; ++x)
            if (mapData.getBlockUnsafe(x,y).isCollidable()) {
                Transform2D entity;
                entity.pos=pos;
                entity.w=dimensions.x;
                entity.h=dimensions.y;

                Transform2D block;
                block.pos={x+0.5f, y+0.5f}; //0.5 for the center
                block.w=1;
                block.h=1;

                //The "-0.00005" value makes it harder to intersect due to numerical imprecision
                if (entity.intersectTransform(block, -0.00005f)) {
                    //The "Touch" variables represent the corners of the block
                        // and dimension/2 is for reaching the center of the hitbox
                    if (delta.x!=0) {
                        if (delta.x<0) {
                            leftTouch=true;
                            pos.x=x+1+dimensions.x/2;
                            return pos;
                        }
                        else {
                            rightTouch=true;
                            pos.x=x-dimensions.x/2;
                            return pos;
                        }
                    }
                    else if (delta.y!=0) {
                        if (delta.y<0) {
                            upTouch=true;
                            pos.y=y+1+dimensions.y/2;
                            return pos;
                        }
                        else {
                            downTouch=true;
                            pos.y=y-dimensions.y/2;
                            return pos;
                        }
                    }
                }
            }

    return pos;
}

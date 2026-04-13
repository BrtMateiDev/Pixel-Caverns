#ifndef MYGAME_ENTITYANIMATION_H
#define MYGAME_ENTITYANIMATION_H

#pragma once

struct EntityAnimation {
    //Storing the frame position (in the sprite sheet)
    int positionX = 0;
    int positionY = 0;
    //The timer before moving on to the next frame
    float timer = 0;

    void update(float dt, float frameDuration, int framesCount) {
        timer -= dt;

        if (timer <= 0) {
            timer += frameDuration;
            //not using just equal so that we can take into account if a frame was loaded slightly late
            positionX++;
        }
        positionX %= framesCount; //looping back to 0
    }

    //Function for changing the row in the sprite sheet
    void setAnimation(int animation) {
        if (positionY != animation) {
            positionX = 0;
            positionY = animation;
        }
    }
};

#endif

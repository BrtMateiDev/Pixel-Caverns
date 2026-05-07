#ifndef MYGAME_PHYSICS_H
#define MYGAME_PHYSICS_H

#pragma once
#include <raylib.h>
#include <raymath.h>

struct Transform2D {
    Vector2 pos = {}; //center
    float w = 0;
    float h = 0;

    Vector2 getCenter() const { return {pos.x, pos.y}; }
    Vector2 getTop() const { return {pos.x, pos.y - h * 0.5f}; }
    Vector2 getBottom() const { return {pos.x, pos.y + h * 0.5f}; }
    Vector2 getLeft() const { return {pos.x - w * 0.5f, pos.y}; }
    Vector2 getRight() const { return {pos.x + w * 0.5f, pos.y}; }
    Vector2 getTopLeft() const { return {pos.x - w * 0.5f, pos.y - h * 0.5f}; }
    Vector2 getTopRight() const { return {pos.x + w * 0.5f, pos.y - h * 0.5f}; }
    Vector2 getBottomLeft() const { return {pos.x - w * 0.5f, pos.y + h * 0.5f}; }
    Vector2 getBottomRight() const { return {pos.x + w * 0.5f, pos.y + h * 0.5f}; }

    //AABB stands for Axis Aligned Bounding Box, in other words, the hitbox (without rotations)
    Rectangle getAABB() {
        //Reminder: raylib renders starting with the top-left corner
        return {pos.x - w * 0.5f, pos.y - h * 0.5f, w, h};
    }

    //Intersection between a hitbox and a point
    bool intersectPoint(Vector2 point, float delta = 0) {
        //delta is a parameter to modify the size of the hitbox
        Rectangle aabb = getAABB();
        aabb.x -= delta;
        aabb.y -= delta;
        aabb.width += 2 * delta;
        aabb.height += 2 * delta;

        return CheckCollisionPointRec(point, aabb);
    }

    //Intersection between two hitboxes
    bool intersectTransform(Transform2D other, float delta = 0) {
        Rectangle a = getAABB();
        Rectangle b = other.getAABB();

        a.x -= delta;
        a.y -= delta;
        a.width += 2 * delta;
        a.height += 2 * delta;

        b.x -= delta;
        b.y -= delta;
        b.width += 2 * delta;
        b.height += 2 * delta;

        return CheckCollisionRecs(a, b);
    }
};

//This is a forward declaration for using the address of mapData
struct GameMap;

struct PhysicalEntity {
    Transform2D transform;
    Vector2 lastPosition = {}; //needed for calculating the direction vector

    Vector2 velocity = {};
    Vector2 acceleration = {};

    bool upTouch = false;
    bool downTouch = false;
    bool leftTouch = false;
    bool rightTouch = false;

    void teleport(Vector2 pos) {
        //If the last position wouldn't be reset, then the "resolveConstraints" function
        //would flag it as a lag spike and teleport back
        transform.pos = pos;
        lastPosition = pos;
    }

    void updateForces(float dt) {
        velocity += acceleration * dt;
        transform.pos += velocity * dt;

        //Below is calculating the drag formula in physics, note the use of "abs" to keep the sign when squaring
        Vector2 dragVector = Vector2{velocity.x * std::abs(velocity.x), velocity.y * std::abs(velocity.y)};
        float drag = 0.01f;
        //arbitrary value which represents the 1/2*density*friction*area part of the formula (because they are constant)

        //This prevents a bug where your character starts vibrating when moving very little
        if (Vector2Length(dragVector) * drag * dt > Vector2Length(velocity)) velocity = {};
        else velocity -= dragVector * drag * dt;

        if (Vector2Length(velocity) < 0.01) velocity = {};

        acceleration = {};
        //Last note: These formulas can be improved, but for now they're good enough.
    }

    void applyGravity() {
        acceleration += {0, 20.f}; //arbitrary value
    }

    bool jump(float force) {
        if (downTouch) {
            velocity.y = -force;
            return true;
        }
        return false;
    }

    //called at the end of the frame
    void updateFinal() {
        lastPosition = {transform.pos.x, transform.pos.y};
    }

    Vector2 &getPosition() {
        return transform.pos;
    }

    void resolveConstraints(GameMap &mapData); //edge cases

    void checkCollisionOnce(Vector2 &pos, GameMap &mapData);

    Vector2 performCollisionOnOneAxis(GameMap &mapData, Vector2 pos, Vector2 delta);
};
#endif

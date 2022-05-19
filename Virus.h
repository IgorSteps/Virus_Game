#pragma once


#include "Sprite.h"
#include "GameObject.h"
class Virus : public GameObject
{
public:
    // virus state	
    float Speed;
    float Radius;

    // constructors
    Virus();
    Virus(glm::vec2 pos, glm::vec2 direction, float radius);
    // moves the ball, keeping it constrained within the petri dish bounds and returns new position
    void Move(glm::vec2 direction, float dt);
};


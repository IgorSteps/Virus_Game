#pragma once
#include "GameObject.h"
#include <vector>

class Enemy : public GameObject
{
public:
    // enemy state	
    float Radius;
    float Speed;
    glm::vec2 Point;
    glm::vec3 Scale=glm::vec3(1.0f);

    // constructors/destructors
    ~Enemy();
    Enemy();
    Enemy(glm::vec2 pos, glm::vec2 direction, float radius, std::vector<Enemy*> &enemies);

    // checks if coordinates overlap with existing enemy
    void GenStartCoord(std::vector<Enemy*> &enemies);
    // generates a rand point for enemy to move to
    void GenPoint();
    // moves the enemy circle
    void Move(float dt);
};


#pragma once

#include "Sprite.h"
class GameObject
{
public:
    // object state
    glm::vec2 Position, Direction;
    float Radius;
    bool IsSolid;
    bool Dead;
    // render state
    Sprite mySprite;
    // constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 direction, float radius);
    // draw sprite
    virtual void Draw(Shader& myShader, glm::mat4 view, glm::mat4 projection, glm::vec3 scale);
    virtual void Init( Shader& shader, float colour[3], std::string filename, float repeatS, float repeatT);
    // check for collisons
    bool checkCollision(GameObject& one, GameObject& two);
};


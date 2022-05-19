#include "Enemy.h"

Enemy::~Enemy() {};

Enemy::Enemy() : GameObject() {
    this->Radius = 0.0f;
    this->Point = glm::vec2(0.0f);
}

Enemy::Enemy(glm::vec2 pos, glm::vec2 direction, float radius, std::vector<Enemy*> &enemies) : GameObject(pos, direction, radius)
{
    this->Radius = radius;
    // generate starting coord
    GenStartCoord(enemies);
    // generate new point to move to
    GenPoint();
}

// todo: need to be generated in the region of a petridish or will they notice?
void Enemy::GenStartCoord(std::vector<Enemy*> &enemies) {
    bool canExist = false;
    // generate random coordinates
    do {
        canExist = true;
        this->Position.x = (float)(rand() % 800);
        this->Position.y = (float)(rand() % 800);
        this->Position.x -= 400.0f;
        this->Position.y -= 400.0f;

        float radius = this->Radius;

        //distance from center
        float Dx1 = this->Position.x - 0.0f;
        float Dy1 = this->Position.y - 0.0f;
        float dist1 = ((Dx1 * Dx1) + (Dy1 * Dy1));
        // check if it's bigger than radius
        if (dist1 > 400.0f * 400.0f) {
            canExist = false;
        }

        for (Enemy* e : enemies) {
            float Dx = e->Position.x - this->Position.x;
            float Dy = e->Position.y - this->Position.y;
            float radii = e->Radius + this->Radius;
            float dist = (Dx * Dx + Dy * Dy);
            // if overlap break and generate coord again
            if (dist < radii*radii) {
                canExist = false;
                break;
            }
        }
    } while (canExist == false);
    

    enemies.push_back(this);
}

void Enemy::GenPoint() {
    // rand point coordinates
    this->Point = glm::vec2(rand() % 800, rand() % 800);
    this->Point -= 400.0f;
    // distance to point from center of the circle
    float Dx = this->Point.x;
    float Dy = this->Point.y;
    float dist = ((Dx * Dx) + (Dy * Dy));
    // radius
    float radius = 400.0f - this->Radius;
    // check if point outside petri-dish
    while (dist > radius * radius) {
        // if so recalculate coords
        this->Point = glm::vec2(rand() % 800, rand() % 800);
        this->Point -= 400.0f;

        Dx = this->Point.x;
        Dy = this->Point.y;
        dist = ((Dx * Dx) + (Dy * Dy));
    }
}

void Enemy::Move(float dt)
{
    // set constant speed
    this->Speed = 30.0f * dt;
    float speed = this->Speed;

    // distance from enemy pos to point pos
    float Dx = this->Point.x - this->Position.x;
    float Dy = this->Point.y - this->Position.y;
    float distToPoint = sqrt((Dx * Dx) + (Dy * Dy));

    // move towards
    this->Direction.x = (Dx / distToPoint) * speed;
    this->Direction.y = (Dy / distToPoint) * speed;

    // once reached point recalculate point
    if (distToPoint < speed) {
        this->Position = this->Point;
        GenPoint();
    }
    else {
        this->Position += this->Direction ;
    }

}

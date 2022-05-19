#include "Virus.h"
Virus::Virus() : GameObject(){
	this->Radius = 0.0f;
	this->Speed = 0.0f;
}

Virus::Virus(glm::vec2 pos, glm::vec2 direction, float radius ) : GameObject(pos, direction, radius) {
	this->Speed = 100.0f;
	this->Radius = radius;
}

void Virus::Move(glm::vec2 distance, float dt) {
	
	
	// change position every frame
	this->Position += glm::vec2(distance.x * this->Speed * dt, distance.y * this->Speed * dt);
	
	// calculate dist from center to virus
	float Dx = this->Position.x - 0;
	float Dy = this->Position.y - 0;
	float dist = ((Dx * Dx) + (Dy * Dy));

	// check dist is smaller than radius
	float radius = 410.0f - this->Radius;
	if (radius*radius <= dist)
	{
		glm::vec2 revVec(-Dx, -Dy);
		this->Position += glm::normalize(revVec) * this->Speed * dt;
	}

}

#include "GameObject.h"


GameObject::GameObject() {
    Position = glm::vec2(0.0f);
    Direction = glm::vec2(0.0f);
}
GameObject::GameObject(glm::vec2 pos, glm::vec2 direction, float radius)
{
    this->Radius = radius;
    this->Position = pos;
    this->Direction = direction;
    IsSolid = false;
    Dead = false;
}

void GameObject::Draw(Shader& myShader,  glm::mat4 view, glm::mat4 projection, glm::vec3 scale)
{
    
        glm::mat4 model = glm::translate(view, glm::vec3(this->Position, 0.0f));
        model = glm::scale(model, scale);

        this->mySprite.Render(myShader, model, projection);
    
}

void GameObject::Init(Shader& shader, float color[3], std::string name, float repeatS, float repeatT) {
        this->mySprite.SetWidth(this->Radius);
        this->mySprite.SetHeight(this->Radius);
        this->mySprite.Init(shader, color, name, repeatS, repeatT);
}


bool GameObject::checkCollision(GameObject& one, GameObject& two) {
    bool collision = false;
    float Dx, Dy, radii;
    Dx = one.Position.x - two.Position.x;
    Dy = one.Position.y - two.Position.y;
    radii = one.Radius/2 + two.Radius;
    if (( Dx*Dx + Dy * Dy) <= (radii*radii) )
    {
        collision = true;
    }
    return collision;
}



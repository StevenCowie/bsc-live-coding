#include "Raycast.h"

Raycast::Raycast()
{
	rayStartPosition = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	rayEndPosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Raycast::update(Camera* camera, btDynamicsWorld* dynamicsWorld)
{

}

void Raycast::destroy()
{

}

Raycast::~Raycast()
{
	destroy();
}
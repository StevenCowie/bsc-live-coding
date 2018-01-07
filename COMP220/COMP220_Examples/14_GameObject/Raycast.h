#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "btBulletDynamicsCommon.h"

#include "Camera.h"

class Raycast
{
public:
	Raycast();
	~Raycast();

	void update(Camera* camera, btDynamicsWorld* dynamicsWorld);

	void destroy();

private:
	//Ray positions
	glm::vec4 rayStartPosition;
	glm::vec4 rayEndPosition;
};
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"

class Camera
{
public:
	Camera();
	~Camera();

	void Mouse(float X, float Y);
	void Forward();
	void Backward();
	void Right();
	void Left();
	void FPSUpdate();
	void Update();
	void Render(GLint ShaderID);

	//Sets Camera Position
	void setPosition(const glm::vec3& position)
	{
		m_CameraPosition = position;
	};

	//Gets Camera Position
	glm::vec3& getPosition()
	{
		return m_CameraPosition;
	};

	//Sets Camera Direction
	void setDirection(const glm::vec3& direction)
	{
		m_CameraDirection = direction;
	};

	//Gets Camera Direction
	glm::vec3& getDirection()
	{
		return m_CameraDirection;
	}

	//Sets Camera Target
	void setTarget(const glm::vec3& target)
	{
		m_CameraTarget = target;
	};

	//Gets Camera Target
	glm::vec3& getTarget()
	{
		return m_CameraTarget;
	};

	//Sets UP
	void setUp(const glm::vec3& up)
	{
		m_CameraUP = up;
	};

	//Gets UP
	glm::vec3& getUp()
	{
		return m_CameraUP;
	};

	//Sets FPSCamera
	void setFPSCamera(const glm::vec3& fpsCamera)
	{
		m_FPSCamera = fpsCamera;
	};

	//Gets FPS Camera
	glm::vec3& getFPSCamera()
	{
		return m_FPSCamera;
	};

	//Sets Camera X
	void setCameraX(float cameraX)
	{
		m_CameraX = cameraX;
	};

	//Gets Camera X
	float getCameraX()
	{
		return m_CameraX;
	};

	//Sets Camera Y
	void setCameraY(float cameraY)
	{
		m_CameraY = cameraY;
	};

	//Gets Camera Y
	float getCameraY()
	{
		return m_CameraY;
	};

	//Sets Camera Distance
	void setDistance(float cameraDistance)
	{
		m_CameraDistance = cameraDistance;
	};

	//Gets Camera Distance
	float getCameraDistance()
	{
		return m_CameraDistance;
	};

	//Gets ShaderProgramID
	GLuint getShaderProgramID()
	{
		return m_ShaderProgramID;
	};

	//Gets View Matrix
	void setViewMatrix(const glm::mat4& viewMatrix)
	{
		m_ViewMatrix = viewMatrix;
	};

	//Sets View Matrix
	glm::mat4& getViewMatrix()
	{
		return m_ViewMatrix;
	};

	//Gets Projection Matrix
	void setProjectionMatrix(const glm::mat4& projectionMatrix)
	{
		m_ProjectionMatrix = projectionMatrix;
	};

	//Sets Projection Matrix
	glm::mat4& getProjectionMatrix()
	{
		return m_ProjectionMatrix;
	};

	glm::vec3 Cross(const glm::vec3& A, const glm::vec3& B)
	{
		return cross(A, B);
	}

private:
	//Camera Properties
	glm::vec3 m_CameraPosition;
	glm::vec3 m_CameraDirection;
	glm::vec3 m_CameraTarget;
	glm::vec3 m_CameraUP;
	glm::vec3 m_FPSCamera;

	//Camera X,Y and distance as floats
	float m_CameraX;
	float m_CameraY;
	float m_CameraDistance;

	GLuint m_ShaderProgramID;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
};
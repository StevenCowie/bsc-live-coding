#include "Camera.h"

Camera::Camera()
{
	//Camera Properties
	m_CameraPosition = glm::vec3(0.0f, -20.0f, -12.0f);
	m_CameraDirection = glm::vec3(0.0f);
	m_CameraTarget = glm::vec3(0.0f, 5.0f, 0.0f);
	m_CameraUP = glm::vec3(0.0f, 1.0f, 0.0f);
	m_FPSCamera = glm::vec3(0.0f);
	m_CameraX = 0.0f;
	m_CameraY = 0.0f;
	m_CameraDistance = (float)(m_CameraTarget - m_CameraPosition).length();

	m_ShaderProgramID = 0;
	m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraTarget, m_CameraUP);
	m_ProjectionMatrix = glm::perspective(glm::radians(90.0f), float(800 / 600), 0.1f, 100.0f);
}

Camera::~Camera()
{

}

//Updates camera position
void Camera::FPSUpdate()
{
	m_CameraPosition += m_FPSCamera;
	m_CameraTarget += m_FPSCamera;
};

//Updates Camera
void Camera::Update()
{
	m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraTarget, m_CameraUP);
}

//Renders Camera
void Camera::Render(GLint ShaderID)
{
	glUseProgram(ShaderID);
	GLint viewMatrixLocation = glGetUniformLocation(ShaderID, "viewMatrix");
	GLint projectionMatrixLocation = glGetUniformLocation(ShaderID, "projectionMatrix");
	GLint cameraPositionLocation = glGetUniformLocation(ShaderID, "cameraPosition");

	glUniform3fv(cameraPositionLocation, 1, value_ptr(m_CameraPosition));
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(m_ViewMatrix));
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(m_ProjectionMatrix));
};

//Moves Camera Forward
void Camera::Forward()
{
	m_FPSCamera = m_CameraDirection * 0.3f;
}

//Moves Camera Backward
void Camera::Backward()
{
	m_FPSCamera = -m_CameraDirection * 0.3f;
}

//Moves Camera Right
void Camera::Right()
{
	m_FPSCamera = cross(m_CameraDirection, m_CameraUP) * 0.3f;
}

//Moves Camera Left
void Camera::Left()
{
	m_FPSCamera = -cross(m_CameraDirection, m_CameraUP) * 0.3f;
}

//Gets Camera direction based on mouse movement
void Camera::Mouse(float X, float Y)
{
	//Get Mouse Motion of X and Y
	m_CameraX += X / 200.0f;
	m_CameraY += Y / 200.0f;
	// Limit camera range
	if (m_CameraY > 150.0f) m_CameraY = 150.0f; else if (m_CameraY < -150.0f) m_CameraY = -150.0f;
	// Calculate camera target using CameraX and CameraY
	m_CameraTarget = m_CameraPosition + m_CameraDistance * glm::vec3(cos(m_CameraX), tan(m_CameraY), sin(m_CameraX));
	// Normalised camera direction
	m_CameraDirection = normalize(m_CameraTarget - m_CameraPosition);
};
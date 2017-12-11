#pragma once

#include <vector>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void setPosition(const glm::vec3& position)
	{
		m_Position = position;
	};

	glm::vec3& getPosition()
	{
		return m_Position;
	};

	//In studio write get/set for rotation and scale
	//In studio write get/set for ambient, diffuse and specular

	glm::mat4& getModelMatrix()
	{
		return m_ModelMatrix;
	};

	//void setSpecularPower(float power)
	//{
	//	m_SpecularPower = power;
	//};

	//float getSpecularPower()
	//{
	//	return m_SpecularPower;
	//};

	GLuint getShaderProgramID()
	{
		return m_ShaderProgramID;
	};

	void loadMesh(const std::string& filename);
	void loadDiffuseMap(const std::string& filename);
	void loadShadersProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);

	void destroy();

	void update();
	void preRender();
	void render();

private:
	std::vector<Mesh*> m_Meshes;

	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
	glm::mat4 m_ModelMatrix;

	GLuint m_DiffuseMapID;
	glm::vec4 m_DiffuseMaterialColour;
	//add other mateirals below
	GLuint m_ShaderProgramID;

};
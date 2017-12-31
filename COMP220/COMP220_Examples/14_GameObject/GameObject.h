#pragma once

#include <vector>
#include <string.h>

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
	
	void loadMeshesFromFile(const std::string& filename);
	void loadDiffuseTextureFromFile(const std::string& filename);
	void loadShaderProgram(const std::string& VertexShaderFilename, const std::string& fragmentShaderFilename);

	void update();
	void destroy();
	void prerender();
	void render();

	//Sets the position of the game object
	void setPosition(const glm::vec3& position)
	{
		m_Position = position;
	};
	//Gets the position of game object
	const glm::vec3& getPosition()
	{
		return m_Position;
	};

	//Sets the scale of game object
	void setScale(const glm::vec3& scale)
	{
		m_Scale = scale;
	};
	//Gets the scale of game object
	const glm::vec3& getScale()
	{
		return m_Scale;
	};

	//Sets rotation of game object
	void setRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;
	};
	//Gets rotation of game object
	const glm::vec3& getRotation()
	{
		return m_Rotation;
	};

	//Gets model matrix
	const glm::mat4& getModelMatrix()
	{
		return m_ModelMatrix;
	};

	//Gets diffuse map
	const GLuint getDiffuseMap()
	{
		return m_DiffuseMap;
	};

	//Sets ambient material colour
	void setAmbientMaterialColour(const glm::vec4& colour)
	{
		m_AmbientMaterialColour = colour;
	};

	//Gets ambient material colour 
	const glm::vec4& getAmbientMaterialColour()
	{
		return m_AmbientMaterialColour;
	};

	//Sets diffuse material colour
	void setDiffuseMaterialColour(const glm::vec4& colour)
	{
		m_DiffuseMaterialColour = colour;
	};

	//Gets diffuse material colour 
	const glm::vec4& getDiffuseMaterialColour()
	{
		return m_DiffuseMaterialColour;
	};

	//Sets specular material colour
	void setSpecularMaterialColour(const glm::vec4& colour)
	{
		m_SpecularMaterialColour = colour;
	};

	//Gets specular material colour 
	const glm::vec4& getSpecularMaterialColour()
	{
		return m_SpecularMaterialColour;
	};

	//Gets specular power
	void setSpecularPower(const float power)
	{
		m_SpecularPower = power;
	};

	//Sets specular power
	const float getSpecularPower()
	{
		return m_SpecularPower;
	};

	const GLuint getShaderProgramID()
	{
		return m_ShaderProgramID;
	};

private:
	//The visible mesh
	std::vector<Mesh*> m_Meshes;

	//Transform
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
	glm::mat4 m_ModelMatrix;

	//Textures
	GLuint m_DiffuseMap;

	//material
	glm::vec4 m_AmbientMaterialColour;
	glm::vec4 m_DiffuseMaterialColour;
	glm::vec4 m_SpecularMaterialColour;
	float m_SpecularPower;

	GLuint m_ShaderProgramID;
};
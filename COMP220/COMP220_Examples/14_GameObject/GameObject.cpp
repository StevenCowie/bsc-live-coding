#include "GameObject.h"

GameObject::GameObject()
{
	m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	m_ModelMatrix = glm::mat4(1.0f);

	m_DiffuseMap = 0;

	m_AmbientMaterialColour = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DiffuseMaterialColour = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	m_SpecularMaterialColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpecularPower = 25.0f;

	m_ShaderProgramID = 0;
}

GameObject::~GameObject()
{
}

void GameObject::loadMesh(const std::string & filename)
{
	loadMeshesFromFile(filename, m_Meshes);
}

void GameObject::loadDiffuseTextureFromFile(const std::string & filename)
{
	m_DiffuseMap = loadTextureFromFile(filename);
}

void GameObject::loadShaderProgram(const std::string & VertexShaderFilename, const std::string & fragmentShaderFilename)
{
	m_ShaderProgramID = LoadShaders(VertexShaderFilename.c_str(), fragmentShaderFilename.c_str());
}

void GameObject::update()
{
	glm::mat4 translationMatrix = glm::translate(m_Position);
	glm::mat4 scaleMatrix = glm::scale(m_Scale);
	glm::mat4 rotationMatrix = glm::rotate(m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))*
		glm::rotate(m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

	m_ModelMatrix = translationMatrix * rotationMatrix*scaleMatrix;
}

void GameObject::destroy()
{
	glDeleteTextures(1, &m_DiffuseMap);
	glDeleteProgram(m_ShaderProgramID);
	auto iter = m_Meshes.begin();
	while (iter != m_Meshes.end())
	{
		if ((*iter))
		{
			(*iter)->destroy();
			delete (*iter);
			iter = m_Meshes.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void GameObject::prerender()
{
	glUseProgram(m_ShaderProgramID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseMap);

	//GLint fragColourLocation = glGetUniformLocation(m_ShaderProgramID, "fragColour");
	GLint modelMatrixLocation = glGetUniformLocation(m_ShaderProgramID, "modelMatrix");
	//GLint viewMatrixLocation = glGetUniformLocation(m_ShaderProgramID, "viewMatrix");
	//GLint projectionMatrixLocation = glGetUniformLocation(m_ShaderProgramID, "projectionMatrix");
	GLint textureLocation = glGetUniformLocation(m_ShaderProgramID, "baseTexture");
	//GLint cameraPositionLocation = glGetUniformLocation(m_ShaderProgramID, "cameraPosition");

	//GLint lightDirectionLocation = glGetUniformLocation(m_ShaderProgramID, "lightDirection");
	//GLint ambientLightColourLocation = glGetUniformLocation(m_ShaderProgramID, "ambientLightColour");
	//GLint diffuseLightColourLocation = glGetUniformLocation(m_ShaderProgramID, "diffuseLightColour");
	//GLint specularLightColourLocation = glGetUniformLocation(m_ShaderProgramID, "specularLightColour");

	GLint ambientMaterialColourLocation = glGetUniformLocation(m_ShaderProgramID, "ambientMaterialColour");
	GLint diffuseMaterialColourLocation = glGetUniformLocation(m_ShaderProgramID, "diffuseMaterialColour");
	GLint specularMaterialColourLocation = glGetUniformLocation(m_ShaderProgramID, "specularMaterialColour");
	GLint specularPowerLocation = glGetUniformLocation(m_ShaderProgramID, "specularPower");

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(m_ModelMatrix));

	glUniform1i(textureLocation, 0);

	//glUniform3fv(lightDirectionLocation, 1, value_ptr(lightDirection));
	//glUniform4fv(ambientLightColourLocation, 1, value_ptr(ambientLightColour));
	//glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
	//glUniform4fv(specularLightColourLocation, 1, value_ptr(specularLightColour));

	glUniform4fv(ambientMaterialColourLocation, 1, value_ptr(m_AmbientMaterialColour));
	glUniform4fv(diffuseMaterialColourLocation, 1, value_ptr(m_DiffuseMaterialColour));
	glUniform4fv(specularMaterialColourLocation, 1, value_ptr(m_SpecularMaterialColour));
	glUniform1f(specularPowerLocation, m_SpecularPower);
}

void GameObject::render()
{
	// Draw
	for (Mesh* currentMesh : m_Meshes)
	{
		currentMesh->render();
	}
}
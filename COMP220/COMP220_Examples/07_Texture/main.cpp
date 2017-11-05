//main.cpp - define the entry point of the application

#include "main.h"

int main(int argc, char* args[])
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_Init failed", SDL_GetError(), NULL);
		return 1;
	}

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	//Create a window, note we have to free the pointer returned using the DestroyWindow Function
	//https://wiki.libsdl.org/SDL_CreateWindow
	SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind
	if (window == nullptr)
	{
		//Show error
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateWindow failed", SDL_GetError(), NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return 1;
	}

	//Request 3.1 core OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_CreateContent failed", SDL_GetError(), NULL);

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 1;
	}

	//Init GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "GLEW initialisation failed", (char*)glewGetErrorString(err), NULL);

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 1;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// An array of 3 vectors which represents 3 vertices
	/*static const GLfloat g_vertex_buffer_data[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f,  0.5f, 0.0f,
	};*/


	Vertex triangleVertices[] = {
		{ -0.5, 0.5, 0.5, 1.0f, 1.0f, 1.5f, 0.0f, 0.0f, 0.0f },		//A 0
		{ 0.5, 0.5, 0.5, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f },		//B 1
		{ -0.5, -0.5, 0.5, 1.0f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f },	//C 2
		{ 0.5, -0.5, 0.5, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },		//D 3
		{ -0.5, 0.5, -0.5, 1.0f, 0.5f, 0.0f, 1.0f },	//E 4
		{ 0.5, 0.5, -0.5, 1.0f, 0.5f, 0.0f, 0.0f },		//F 5
		{ -0.5, -0.5, -0.5, 1.0f, 0.5f, 0.3f, 1.0f },	//G 6
		{ 0.5, -0.5, -0.5, 1.0f, 0.3f,0.0f, 0.3f },		//H 7

	};

	unsigned int triangleIndices[] =
	{
		//Front
		0, 1, 2,
		1, 2, 3,
		//Back
		4, 5, 6,
		5, 6, 7,
		//Left
		0, 4, 6,
		0, 6, 2,
		//Right
		3, 1, 5,
		5, 3, 7,
		//Bottom
		6, 7, 2,
		2, 3, 7,
		//Top
		4, 5, 1,
		0, 1, 4

	};

	//Identify vetex buffer
	GLuint vertexBuffer;
	//Generate 1 buffer, put the resulting identifier in vertexBuffer
	glGenBuffers(1, &vertexBuffer);
	// Following commands talk about 'vertexBuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	// Give vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), triangleVertices, GL_STATIC_DRAW);

	//Identify element buffer
	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), triangleIndices, GL_STATIC_DRAW);

	GLuint textureID = loadTextureFromFile("Crate.jpg");


	vec3 trianglePosition = vec3(0.0f, 0.0f, 0.0f);
	vec3 triangleScale = vec3(1.0f, 1.0f, 1.0f);
	vec3 triangleRotation = vec3(0.0f, 0.0f, 0.0f);

	mat4 rotationMatrix = rotate(triangleRotation.x, vec3(1.0f, 0.0f, 0.0f))*rotate(triangleRotation.y, vec3(0.0f, 1.0f, 0.0f))*rotate(triangleRotation.z, vec3(1.0f, 0.0f, 1.0f));
	mat4 scaleMatrix = scale(triangleScale);
	mat4 translationMatrix = translate(trianglePosition);

	mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

	vec3 cameraPosition = vec3(1.0f, 2.0f, -5.0f);
	vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

	mat4 viewMatrix = lookAt(cameraPosition, cameraTarget, cameraUp);

	mat4 projectionMatrix = perspective(radians(90.0f), float(4 / 3), 0.1f, 100.0f);

	//Create and compile GLSL program from shaders
	GLuint programID = LoadShaders("textureVert.glsl", "textureFrag.glsl");

	GLint fragColorLocation = glGetUniformLocation(programID, "fragColor");
	if (fragColorLocation < 0)
	{
		printf("Unable to find %s uniform", "fragColor");
	}

	static const GLfloat fragColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	//GLint modelMatrixLocation = glGetUniformLocation(programID, "modelMatrix");
	//GLint viewMatrixLocation = glGetUniformLocation(programID, "viewMatrix");
	//GLint projectionMatrixLocation = glGetUniformLocation(programID, "projectionMatrix");
	GLint textureLocation = glGetUniformLocation(programID, "baseTexture");

	GLint currentTimeLocation = glGetUniformLocation(programID, "time");
	if (currentTimeLocation < 0)
	{
		printf("Unable to find %s uniform", "time");
	}

	GLint modelMatrixLocation = glGetUniformLocation(programID, "modelMatrix");
	if (modelMatrixLocation < 0)
	{
		printf("Unable to find %s uniform", "modelMatrix");
	}

	GLint viewMatrixLocation = glGetUniformLocation(programID, "viewMatrix");
	if (viewMatrixLocation < 0)
	{
		printf("Unable to find %s uniform", "viewMatrix");
	}

	GLint projectionMatrixLocation = glGetUniformLocation(programID, "projectionMatrix");
	if (projectionMatrixLocation < 0)
	{
		printf("Unable to find %s uniform", "projectionMatrix");
	}

	int lastTicks = SDL_GetTicks();
	int currentTicks = SDL_GetTicks();


	//Event loop, we will loop until running is set to false, usually if escape has been pressed or window is closed
	bool running = true;
	//SDL Event structure, this will be checked in the while loop
	SDL_Event ev;
	while (running)
	{
		//Poll for the events which have happened in this frame
		//https://wiki.libsdl.org/SDL_PollEvent
		while (SDL_PollEvent(&ev))
		{
			//Switch case for every message we are intereted in
			switch (ev.type)
			{
				//QUIT Message, usually called when the window has been closed
			case SDL_QUIT:
				running = false;
				break;
				//KEYDOWN Message, called when a key has been pressed down
			case SDL_KEYDOWN:
				//Check the actual key code of the key that has been pressed
				switch (ev.key.keysym.sym)
				{
					//Escape key
				case SDLK_ESCAPE:
					running = false;
					break;

					//Right Key
				case SDLK_RIGHT:
					triangleRotation.y += 0.5;
					break;

					//Left Key
				case SDLK_LEFT:
					triangleRotation.y += -0.5;
					break;

					//Up Key
				case SDLK_UP:
					triangleRotation.x += 0.5;
					break;

					//Down Key
				case SDLK_DOWN:
					triangleRotation.x += -0.5;
					break;
				}
			}
		}

		//Recalculate translations
		rotationMatrix = rotate(triangleRotation.x, vec3(1.0f, 0.0f, 0.0f))*rotate(triangleRotation.y, vec3(0.0f, 1.0f, 0.0f))*rotate(triangleRotation.z, vec3(1.0f, 0.0f, 1.0f));
		modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		currentTicks = SDL_GetTicks();
		float deltaTime = (float)(currentTicks - lastTicks) / 1000.0f;

		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

		glUseProgram(programID);
		glUniform4fv(fragColorLocation, 1, fragColor);
		glUniform1f(currentTimeLocation, (float)(currentTicks) / 1000.0f);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(modelMatrix));
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));
		glUniform1i(textureLocation, 0);

		//1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,					//Attribute 0, no reason for 0, but must match layout in the shader
			3,					//Size 
			GL_FLOAT,			//Type
			GL_FALSE,			//Normalized?
			sizeof(Vertex),		//Stride
			(void*)0			//Array buffer offset
		);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(float)));

		//Draw the triangle
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
		SDL_GL_SwapWindow(window);

		lastTicks = currentTicks;
	}

	glDeleteProgram(programID);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &elementBuffer);
	glDeleteTextures(1, &textureID);
	glDeleteVertexArrays(1, &VertexArrayID);
	//Delete content
	SDL_GL_DeleteContext(glContext);
	//Destroy the window and quit SDL2, NB we should do this after all cleanup in this order!!!
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);

	IMG_Quit();
	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();

	return 0;
}
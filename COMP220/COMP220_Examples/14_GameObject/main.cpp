//main.cpp - defines the entry point of the application

#include "main.h"

int main(int argc, char* args[])
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_Init failed", NULL);
		return 1;
	}

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	//Create a window, note we have to free the pointer returned using the DestroyWindow Function
	//https://wiki.libsdl.org/SDL_CreateWindow
	SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 640, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind
	if (window == nullptr)
	{
		//Show error
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_CreateWindow failed", NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return 1;
	}

	//lets ask for a 3.2 core profile version of OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext GL_Context = SDL_GL_CreateContext(window);
	if (GL_Context == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL GL Create Context failed", NULL);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, (char*)glewGetErrorString(glewError), "GLEW Init Failed", NULL);
	}
	//Ebales depth testing
	glEnable(GL_DEPTH_TEST);

	//Sets projection matrix
	mat4 projectionMatrix = perspective(radians(90.0f), float(800 / 600), 0.1f, 100.0f);

	//light
	vec4 ambientLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec3 lightDirection = vec3(0.0f, 0.0f, -1.0f);
	vec4 diffuseLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 specularLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Gameobject
	std::vector<GameObject*> GameObjectList;
	//Loop for gameobject, if there are less than 5 spawned in, spawn in 1 more
	for (int i = 0; i < 5; ++i)
	{
		//Create GameObject & sets properties
		GameObject * pDroid = new GameObject();
		pDroid->setRotation(vec3(1.0f, 1.2f, 1.0f));
		pDroid->setPosition(vec3(0.5f, 0.5f, (float)i));
		pDroid->setScale(vec3(0.2f, 0.2f, 0.2f));
		pDroid->loadMesh("GNK_Droid.FBX");
		pDroid->loadDiffuseTextureFromFile("GNK_BaseColor.png");
		pDroid->loadShaderProgram("textureVert.glsl", "textureFrag.glsl");
		GameObjectList.push_back(pDroid);
	};
	
	//Creates new seperate gameobject with different mesh
	GameObject * pTank = new GameObject();
	pTank = new GameObject();
	pTank->setPosition(vec3(0.5f, 0.2f, -0.5f));
	pTank->setScale(vec3(0.2f, 0.2f, 0.2f));
	pTank->loadMesh("Tank1.FBX");
	pTank->loadDiffuseTextureFromFile("Tank1DF.png");
	pTank->loadShaderProgram("textureVert.glsl", "textureFrag.glsl");
	GameObjectList.push_back(pTank);

	//Creates camera
	Camera * camera = new Camera();

	//Creates raycast
	Raycast * raycast = new Raycast();

	//material
	vec4 ambientMaterialColour = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4 diffuseMaterialColour = vec4(0.0f, 0.6f, 0.6f, 0.1f);
	vec4 specularMaterialColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float specularPower = 25.0f;

	//Colour Buffer Texture
	GLuint colourBufferID = createTexture(800, 640);

	//Create Depth Buffer (Can only have 1 depth buffer)
	GLuint depthRenderBufferID;
	glGenRenderbuffers(1, &depthRenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 800, 640);

	//Create framebuffer
	GLuint frameBufferID;
	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourBufferID, 0);

	//Checks to see if framebuffer is set up right
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to create frame buffer for post processing","Frame Buffer Error", NULL);
	}

	//Create screen aligned quad
	GLfloat screenVerts[] =
	{
		-1,-1,
		1,-1,
		-1,1,
		1,1,
	};

	//
	GLuint screenQuadVBOID;
	glGenBuffers(1, &screenQuadVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), screenVerts, GL_STATIC_DRAW);

	//Generates vertex array objects
	GLuint screenVAO;
	glGenVertexArrays(1, &screenVAO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//Put post processing shaders here
	GLuint postProcessingProgramID = LoadShaders("passThroughVert.glsl", "postBlueAndRed.glsl");
	GLint texture0Location = glGetUniformLocation(postProcessingProgramID, "texture0");

	//Loads shaders
	GLuint programID = LoadShaders("lightingVert.glsl", "lightingFrag.glsl");

	static const GLfloat fragColour[] = { 0.0f,1.0f,0.0f,1.0f };

	GLint fragColourLocation = glGetUniformLocation(programID, "fragColour");
	
	GLint currentTimeLocation = glGetUniformLocation(programID, "time");
	GLint modelMatrixLocation = glGetUniformLocation(programID, "modelMatrix");
	GLint viewMatrixLocation = glGetUniformLocation(programID, "viewMatrix");
	GLint projectionMatrixLocation = glGetUniformLocation(programID, "projectionMatrix");
	GLint textureLocation = glGetUniformLocation(programID, "baseTexture");
	GLint cameraPositionLocation = glGetUniformLocation(programID, "cameraPosition");

	GLint lightDirectionLocation = glGetUniformLocation(programID, "lightDirection");
	GLint ambientLightColourLocation = glGetUniformLocation(programID, "ambientLightColour");
	GLint diffuseLightColourLocation = glGetUniformLocation(programID, "diffuseLightColour");
	GLint specularLightColourLocation = glGetUniformLocation(programID, "specularLightColour");

	GLint ambientMaterialColourLocation = glGetUniformLocation(programID, "ambientMaterialColour");
	GLint diffuseMaterialColourLocation = glGetUniformLocation(programID, "diffuseMaterialColour");
	GLint specularMaterialColourLocation = glGetUniformLocation(programID, "specularMaterialColour");
	GLint specularPowerLocation = glGetUniformLocation(programID, "specularPower");
	
	//Bullet Physics Initialised
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(2.), btScalar(50.)));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -10, 0));

	btScalar mass(0.);
	btVector3 localInertia(0, 0, 0);
	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	btRigidBody* groundRigidBody = new btRigidBody(rbInfo);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(groundRigidBody);

	//Creates new collision shape for tank
	btCollisionShape* tankCollisionShape = new btBoxShape(btVector3(2, 2, 2));
	
	/// Create Dynamic Objects
	btTransform tankTransform;
	tankTransform.setIdentity();
	btVector3 tankInertia(0, 0, 0);
	btScalar tankMass(1.f);
	tankCollisionShape->calculateLocalInertia(tankMass, tankInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* tankMotionState = new btDefaultMotionState(tankTransform);
	btRigidBody::btRigidBodyConstructionInfo tankRbInfo(tankMass, tankMotionState, tankCollisionShape, tankInertia);
	btRigidBody* tankRigidBody = new btRigidBody(tankRbInfo);
	tankRigidBody->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addRigidBody(tankRigidBody);

	int invertGravity = -10;

	//Force Movement
	btVector3 tankForceJump = btVector3(0, 20, 0);
	btVector3 tankForceLeft = btVector3(-20, 0, 0);
	btVector3 tankForceRight = btVector3(20, 0, 0);
	btVector3 tankForceForward = btVector3(0, 0, 20);
	btVector3 tankForceBackward = btVector3(0, 0, -20);

	//Impulse Movement
	btVector3 tankImpulse = btVector3(4, 0, 0);

	//Torque Movement
	btVector3 tankTorque = btVector3(4, 0, 0);

	//SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_bool(SDL_ENABLE));

	glEnable(GL_DEPTH_TEST);
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

			case SDL_MOUSEBUTTONDOWN:
				// Check button code of the pressed mouse button
				switch (ev.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					// Fires raycast
					raycast->update(camera, dynamicsWorld);
					break;
				}
				}

			case SDL_MOUSEMOTION:
				// Get Mouse Motion of X and Y
				camera->Mouse(ev.motion.xrel, -ev.motion.yrel);
				break;

				//KEYDOWN Message, called when a key has been pressed down
			case SDL_KEYDOWN:
				//Check the actual key code of the key that has been pressed
				switch (ev.key.keysym.sym)
				{
					// Keys
				//Quits application
				case SDLK_ESCAPE:
					running = false;
					break;
				//Moves tnak "right"
				case SDLK_RIGHT:
					tankRigidBody->applyCentralForce(tankForceLeft);
					break;
				//Moves tank "left"
				case SDLK_LEFT:
					tankRigidBody->applyCentralForce(tankForceRight);
					break;
				//Moves tank "forward"
				case SDLK_UP:
					tankRigidBody->applyCentralForce(tankForceForward);
					break;
				//Moves tank "backward"
				case SDLK_DOWN:
					tankRigidBody->applyCentralForce(tankForceBackward);
					break;

				//Makes tank jump
				case SDLK_SPACE:
					tankRigidBody->applyCentralForce(tankForceJump);
					break;

				//Inverts Gravity for game idea, probably not enough time to do
				case SDLK_LSHIFT:
					invertGravity *= -1;
					dynamicsWorld->setGravity(btVector3(0, invertGravity, 0));
					break;
				//Moves camera forward
				case SDLK_w:
					camera->Forward();
					camera->FPSUpdate();
					break;
				//Moves camera backward
				case SDLK_s:
					camera->Backward();
					camera->FPSUpdate();
					break;
				//Moves camera left
				case SDLK_a:
					camera->Left();
					camera->FPSUpdate();
					break;
				//Moves camera right
				case SDLK_d:
					camera->Right();
					camera->FPSUpdate();
					break;
				}				
			}
		}
		//Update Game and Draw with OpenGL!!

		currentTicks = SDL_GetTicks();
		float deltaTime = (float)(currentTicks - lastTicks) / 1000.0f;
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);


		//Updates tank position after physics is applied
		tankTransform = tankRigidBody->getWorldTransform();
		btVector3 tankOrigin = tankTransform.getOrigin();
		pTank->setPosition(glm::vec3(tankOrigin.x(), tankOrigin.y(), tankOrigin.z()));
		
		//Updates Camera
		camera->Update();

		//Updates gameobjects
		for (GameObject* pObj : GameObjectList)
		{
			pObj->update();
		}



		glEnable(GL_DEPTH_TEST);
		//glGenFramebuffers(1, &frameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClear(GL_COLOR_BUFFER_BIT);

		for (GameObject* pObj : GameObjectList)
		{
			pObj->prerender();
			GLuint currentProgramID = pObj->getShaderProgramID();

			//Retrieve the shader values
			//Take Lighting across too
			GLint viewMatrixLocation = glGetUniformLocation(currentProgramID, "viewMatrix");
			GLint projectionMatrixLocation = glGetUniformLocation(currentProgramID, "projectionMatrix");
			GLint lightDirectionLocation = glGetUniformLocation(currentProgramID, "lightDirection");
			GLint ambientLightColourLocation = glGetUniformLocation(currentProgramID, "ambientLightColour");
			GLint diffuseLightColourLocation = glGetUniformLocation(currentProgramID, "diffuseLightColour");
			GLint specularLightColourLocation = glGetUniformLocation(currentProgramID, "specularLightColour");

			//Send shader values
			//glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));

			glUniform3fv(lightDirectionLocation,1,value_ptr(lightDirection));
			glUniform4fv(ambientLightColourLocation, 1, value_ptr(ambientLightColour));
			glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
			glUniform4fv(specularLightColourLocation, 1, value_ptr(specularLightColour));
			camera->Render(currentProgramID);
			pObj->render();
		}

		////Retrieve the shader values
		////Take Lighting across too
		//GLint viewMatrixLocation = glGetUniformLocation(currentProgramID, "viewMatrix");
		//GLint projectionMatrixLocation = glGetUniformLocation(currentProgramID, "projectionMatrix");

		////Send shader values
		//glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
		//glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));


		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureID);

		//glUseProgram(programID);

		//glUniform4fv(fragColourLocation, 1, fragColour);
		//glUniform1f(currentTimeLocation, (float)(currentTicks) / 1000.0f);
		//glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(modelMatrix));
		//glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
		//glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));

		//glUniform3fv(cameraPositionLocation, 1, value_ptr(cameraPosition));

		//glUniform1i(textureLocation, 0);

		//glUniform3fv(lightDirectionLocation,1,value_ptr(lightDirection));
		//glUniform4fv(ambientLightColourLocation, 1, value_ptr(ambientLightColour));
		//glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
		//glUniform4fv(specularLightColourLocation, 1, value_ptr(specularLightColour));

		//glUniform4fv(ambientMaterialColourLocation, 1, value_ptr(ambientMaterialColour));
		//glUniform4fv(diffuseMaterialColourLocation, 1, value_ptr(diffuseMaterialColour));
		//glUniform4fv(specularMaterialColourLocation, 1, value_ptr(specularMaterialColour));
		//glUniform1f(specularPowerLocation, specularPower);


		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		//Bind post processing shaders
		glUseProgram(postProcessingProgramID);
		
		//Activate texture unit 0 for the colour buffer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colourBufferID);
		glUniform1i(texture0Location, 0);

		glBindVertexArray(screenVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		SDL_GL_SwapWindow(window);

	}

	//deletes tank physics
	dynamicsWorld->removeRigidBody(tankRigidBody);
	delete tankCollisionShape;
	delete tankRigidBody->getMotionState();
	delete tankRigidBody;

	//removes rigidbody from ground
	dynamicsWorld->removeRigidBody(groundRigidBody);

	//delete ground
	delete groundShape;
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	//deletes collision
	delete collisionConfiguration;

	auto GameObjectIter = GameObjectList.begin();
	while (GameObjectIter != GameObjectList.end())
	{
		if ((*GameObjectIter))
		{
			(*GameObjectIter)->destroy();
			delete (*GameObjectIter);
			GameObjectIter = GameObjectList.erase(GameObjectIter);
		}
	}

	//Deletes Camera
	delete camera;

	//Delete Raycast
	delete raycast;

	glDeleteProgram(postProcessingProgramID);
	glDeleteVertexArrays(1, &screenVAO);
	glDeleteBuffers(1, &screenQuadVBOID);

	glDeleteFramebuffers(1, &frameBufferID);
	glDeleteRenderbuffers(1, &depthRenderBufferID);
	glDeleteTextures(1, &colourBufferID);

	//Delete context
	SDL_GL_DeleteContext(GL_Context);

	//Destroy the window and quit SDL2, NB we should do this after all cleanup in this order!!!
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);

	IMG_Quit();

	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();

	return 0;
}
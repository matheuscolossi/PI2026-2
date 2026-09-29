#include "Scene1.h"


Scene1::Scene1()
{
	// Cria a câmera
	pCamera = NULL;
	pCamera = new CCamera(glm::vec3(0.0f, 3.0f, 10.0f));
	pCamera->MovementSpeed = 100.0f;

	// Cria o Timer
	pTimer = NULL;
	pTimer = new CTimer();
	pTimer->Init();

	// Cria gerenciador de impressão de texto na tela
	pText = NULL;
	pText = new CText();
	pText->InitTextManager();

	// Cria os shaders
	pShader = NULL;
	pShader = new CShader();
	pShader->LoadShader("Grid", "Scenes/Common/grid.vert", "Scenes/Common/grid.frag");
	pShader->LoadShader("Axis", "Scenes/Common/axis.vert", "Scenes/Common/axis.frag");
	pShader->LoadShader("Text2D", "Scenes/Common/Text2D.vert", "Scenes/Common/Text2D.frag");

	pShader->LoadShader("Primitives", 
		"Scenes/Scene1/Primitives.vert", 
		"Scenes/Scene1/Primitives.frag");


	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureAnisotropic(0, "Scenes/Textures/cx2.BMP");
	pTextures->LoadTextureAnisotropic(1, "Scenes/Textures/grass.BMP");
	pTextures->LoadTextureAnisotropic(2, "Scenes/Textures/rock.jpg");
		
	// Cria as primitivas
	cube.generateCube(2.0);
	cube.setupBuffers();

	sphere.generateSphere(2.0);
	sphere.setupBuffers();

	cylinder.generateCylinder(0.5, 3.0);
	cylinder.setupBuffers();

	cone.generateCone(2.0, 3.0);
	cone.setupBuffers();
}

Scene1::~Scene1()
{
	cube.cleanup();
	sphere.cleanup();
	cylinder.cleanup();
	cone.cleanup();

	if (pCamera)
	{
		delete pCamera;
		pCamera = NULL;
	}

	if (pShader)
	{
		delete pShader;
		pShader = NULL;
	}

	if (pGridAxis)
	{
		delete pGridAxis;
		pGridAxis = NULL;
	}

	if (pTextures)
	{
		delete pTextures;
		pTextures = NULL;
	}

	if (pTimer)
	{
		delete pTimer;
		pTimer = NULL;
	}

	if (pText)
	{
		delete pText;
		pText = NULL;
	}


}

void Scene1::DrawScene(float deltaTime)
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

	// Clear the color and depth buffers
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create transformations 
	// Note: In a real application, you would typically calculate the projection and view matrices 
	// once per frame and pass them to your shaders
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw the 3D grid
	pShader->Use("Grid");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pShader->SetFloat("uGridSpacing", 1.0f);
	pShader->SetFloat("uFadeStart", 18.f);
	pShader->SetFloat("uFadeEnd", 100.f);
	pShader->SetVec3("uCamPos", pCamera->Position);
	pGridAxis->DrawGrid();
	// Draw the 3D axis
	pShader->Use("Axis");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pGridAxis->DrawAxis();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (INÍCIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Desenha o Cubo
	pShader->Use("Primitives");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-5.0f, 3.0f, 0.0f));
	pShader->SetMat4("model", model);

	// Habilita a textura
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(0);
	pShader->SetInt("Texture0", 0);

	pShader->SetBool("isTextured", false);
	pShader->SetVec4("Color", glm::vec4(1.0, 1.0, 0.0, 1.0));

	cube.render(); // Renderiza o cubo

///////////////////////////////////////////////////////////////////////////

	// Desenha a esfera
	pShader->Use("Primitives");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(5.0f, 3.0f, 0.0f));
	pShader->SetMat4("model", model);

	// Habilita a textura
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(2);
	pShader->SetInt("Texture0", 0);

	pShader->SetBool("isTextured", true);
	pShader->SetVec4("Color", glm::vec4(0.0, 1.0, 0.5, 1.0));

	sphere.render(); // Renderiza a esfera

////////////////////////////////////////////////////////////////////////////////////

	// Desenha Árvore
	pShader->Use("Primitives");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	DrawTree(glm::vec3(-10.0f, 1.5f, -10.0));
	DrawTree(glm::vec3( 10.0f, 6.0f, -10.0), glm::vec3(2.0f, 4.0f, 2.0f));
	DrawTree(glm::vec3(  7.0f, 0.7f, -10.0), glm::vec3(1.0f, 0.5f, 1.0f));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (FIM)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw text on the screen (disable depth test and use orthographic projection)
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

	pShader->Use("Text2D");
	pShader->SetMat4("projection", projection);
	pShader->SetVec3("textColor", 1.0f, 1.0f, 1.0f);

	pText->RenderText(std::format("CamPosition\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Position.x, pCamera->Position.y, pCamera->Position.z), 5.0f, 100.0f, 0.3f);
	pText->RenderText(std::format("CamFoward\tx: {0:.2f} \t y: {1:.2f} \t z: {2:.2f}", pCamera->Front.x, pCamera->Front.y, pCamera->Front.z), 5.0f, 80.0f, 0.3f);

	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene1::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	// Camera movement
	if (input.IsKeyPressed(GLFW_KEY_W))
		pCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_S))
		pCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_A))
		pCamera->ProcessKeyboard(LEFT, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_D))
		pCamera->ProcessKeyboard(RIGHT, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_E))
		pCamera->ProcessKeyboard(UP, deltaTime);
	if (input.IsKeyPressed(GLFW_KEY_Q))
		pCamera->ProcessKeyboard(DOWN, deltaTime);

	// Increase/decrease movement speed
	if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
		pCamera->MovementSpeed = SPEED_MULTIPLIER;
	else
		pCamera->MovementSpeed = SPEED;

	// Process mouse movement
	glm::vec2 mouseDelta = input.GetMouseDelta();
	if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
	{
		pCamera->ProcessMouseMovement(mouseDelta.x, mouseDelta.y);
	}

	// Process scroll
	float scrollOffset = input.GetScrollOffset();
	if (scrollOffset != 0.0f)
	{
		pCamera->ProcessMouseScroll(scrollOffset);
	}
}

void Scene1::DrawTree(glm::vec3 pos, glm::vec3 size)
{
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, pos);
	model = glm::scale(model, size);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	pShader->SetMat4("model", model);
	pShader->SetBool("isTextured", false);
	pShader->SetVec4("Color", glm::vec4(0.58, 0.39, 0.17, 1.0));
	cylinder.render();

	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
	pShader->SetMat4("model", model);
	pShader->SetBool("isTextured", false);
	pShader->SetVec4("Color", glm::vec4(0.0, 0.5, 0.0, 1.0));
	cone.render();
}
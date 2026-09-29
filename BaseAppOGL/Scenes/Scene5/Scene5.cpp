#include "Scene5.h"


Scene5::Scene5()
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

	pShader->LoadShader("MultiTexture", 
		"Scenes/Scene5/MultiTexture.vert", 
		"Scenes/Scene5/MultiTexture.frag");

	pShader->LoadShader("LightMapping",
		"Scenes/Scene5/LightMapping.vert",
		"Scenes/Scene5/LightMapping.frag");

	pShader->LoadShader("MaskTexture",
		"Scenes/Scene5/MaskTexture.vert",
		"Scenes/Scene5/MaskTexture.frag");

	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureMipMap(0, "Scenes/Textures/brick.bmp");
	pTextures->LoadTextureMipMap(1, "Scenes/Textures/b1.jpg");
	pTextures->LoadTextureMipMap(2, "Scenes/Textures/water.bmp");
	pTextures->LoadTextureMipMap(3, "Scenes/Textures/Blood.bmp");
	pTextures->LoadTextureMipMap(4, "Scenes/Textures/grass.bmp");
	pTextures->LoadTextureMipMap(5, "Scenes/Textures/mask.bmp");
	pTextures->LoadTextureMipMap(6, "Scenes/Textures/mud.bmp");
	pTextures->LoadTextureMipMap(7, "Scenes/Textures/lightmap.bmp");
	

	// Cria as primitivas
	ground.generatePlane(20.0f, 10.0f);
	ground.setupBuffers();

}

Scene5::~Scene5()
{
	// Destrói as primitivas
	ground.cleanup();

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

void Scene5::DrawScene(float deltaTime)
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
	//pShader->Use("Grid");
	//pShader->SetMat4("uProj", projection);
	//pShader->SetMat4("uView", view);
	//pShader->SetFloat("uGridSpacing", 1.0f);
	//pShader->SetFloat("uFadeStart", 18.f);
	//pShader->SetFloat("uFadeEnd", 100.f);
	//pShader->SetVec3("uCamPos", pCamera->Position);
	//pGridAxis->DrawGrid();
	// Draw the 3D axis
	pShader->Use("Axis");
	pShader->SetMat4("uProj", projection);
	pShader->SetMat4("uView", view);
	pGridAxis->DrawAxis();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                               DESENHA OS OBJETOS DA CENA (INÍCIO)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Exemplo de Multi-Textura (DECAL)
	pShader->Use("MultiTexture");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	pShader->SetMat4("model", model);

	// Configura a textura para a camada 0
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(4);
	pShader->SetInt("Texture0", 0);

	// Configura a textura para a camada 1
	glActiveTexture(GL_TEXTURE1);
	pTextures->ApplyTexture(3);
	pShader->SetInt("Texture1", 1);

	// Desenha o chão
	ground.render();
////////////////////////////////////////////////////////////////

	// Exemplo de Multi-Textura (LIGHTMAP)
	pShader->Use("LightMapping");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), 
		glm::vec3(1.0, 0.0, 0.0));
	pShader->SetMat4("model", model);

	// Configura a textura para a camada 0
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(0);
	pShader->SetInt("Texture0", 0);

	// Configura a textura para a camada 1
	glActiveTexture(GL_TEXTURE1);
	pTextures->ApplyTexture(7);
	pShader->SetInt("Texture1", 1);

	// Passa o timer
	pShader->SetFloat("uTime", glfwGetTime());

	// Desenha o chão
	ground.render();

	///////////////////////////////////////////////////////////
	// Exemplo de Multi-Textura (MASK)
	pShader->Use("MaskTexture");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f));
	pShader->SetMat4("model", model);

	// Configura a textura para a camada 0 (Grass)
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(4);
	pShader->SetInt("Texture0", 0);

	// Configura a textura para a camada 1 (Mask)
	glActiveTexture(GL_TEXTURE1);
	pTextures->ApplyTexture(5);
	pShader->SetInt("Texture1", 1);

	// Configura a textura para a camada 2 (Mud)
	glActiveTexture(GL_TEXTURE2);
	pTextures->ApplyTexture(6);
	pShader->SetInt("Texture2", 2);

	// Desenha o chão
	ground.render();













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


void Scene5::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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


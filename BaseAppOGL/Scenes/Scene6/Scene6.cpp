#include "Scene6.h"


Scene6::Scene6()
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

	pShader->LoadShader("SkyBox", 
		"Scenes/Common/SkyBox.vert", 
		"Scenes/Common/SkyBox.frag");

	pShader->LoadShader("Asset", 
		"Scenes/Scene6/Assets.vert", 
		"Scenes/Scene6/Assets.frag");


	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cria o gerenciador de texturas
	pTextures = NULL;
	//pTextures = new CTextures();
	

	// Cria o Skybox e carrega suas texturas
	pSkyBox = NULL;
	pSkyBox = new CSkybox({
		"Scenes/SkyBox/right.bmp",
		"Scenes/SkyBox/left.bmp",
		"Scenes/SkyBox/top.bmp",
		"Scenes/SkyBox/bottom.bmp",
		"Scenes/SkyBox/front.bmp",
		"Scenes/SkyBox/back.bmp"
		});

	// Carrega a Estatua
	pStatue = new CAsset("Scenes/Scene6/Statue/Statue.obj");
	pCastle = new CAsset("Scenes/Assets/Castle/Castle.obj");
}

Scene6::~Scene6()
{
	if (pSkyBox)
	{
		delete pSkyBox;
		pSkyBox = NULL;
	}


	if (pStatue)
	{
		delete pStatue;
		pStatue = NULL;
	}

	if (pCastle)
	{
		delete pCastle;
		pCastle = NULL;
	}

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

void Scene6::DrawScene(float deltaTime)
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

	// Statue
	pShader->Use("Asset");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(6.0f, 0.0f, 30.0f));
	pShader->SetMat4("model", model);
	pStatue->Draw(pShader->GetProgram("Asset"));

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-6.0f, 0.0f, 30.0f));
	pShader->SetMat4("model", model);
	pStatue->Draw(pShader->GetProgram("Asset"));


	// Desenha o Castelo
	pShader->Use("Asset");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	pShader->SetMat4("model", model);
	pCastle->Draw(pShader->GetProgram("Asset"));







	// Desenha o Skybox
	pShader->Use("SkyBox");
	pShader->SetMat4("projection", projection);

	view = glm::mat4(glm::mat3(pCamera->GetViewMatrix()));
	pShader->SetMat4("view", view);
	pShader->SetInt("skyboxTextures", 0);
	pSkyBox->DrawSkybox();












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


void Scene6::ProcessSceneInput(GLFWwindow* window, float deltaTime)
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


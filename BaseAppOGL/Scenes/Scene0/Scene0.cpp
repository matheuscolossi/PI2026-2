#include "Scene0.h"


Scene0::Scene0()
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
	
	pShader->LoadShader("SolidColor", "Scenes/Scene0/Solid.vert", 
		"Scenes/Scene0/Solid.frag");

	pShader->LoadShader("Color", "Scenes/Scene0/Color.vert",
		"Scenes/Scene0/Color.frag");

	pShader->LoadShader("TextureColor", "Scenes/Scene0/TextureColor.vert",
		"Scenes/Scene0/TextureColor.frag");

	pShader->LoadShader("Textured", "Scenes/Scene0/Textured.vert",
		"Scenes/Scene0/Textured.frag");


	// Cria o gerenciador de grid e axis
	pGridAxis = NULL;
	pGridAxis = new CGridAxis(50.0f);

	// Cor do Framebuffer (Cor de fundo)
	vFramebufferColor = glm::vec4(0.1f, 0.1f, 0.15f, 1.0f);

	// Entra no modo GUI
	bActiveGUI = false;

	// Cria o gerenciador de texturas
	pTextures = NULL;
	pTextures = new CTextures();
	pTextures->LoadTextureAnisotropic(0, "Scenes/Textures/cx2.BMP");
	pTextures->LoadTextureAnisotropic(1, "Scenes/Textures/grass.bmp");
	pTextures->LoadTextureAnisotropic(2, "Scenes/Textures/diceD.png");
	pTextures->LoadTextureAnisotropic(3, "Scenes/Textures/TriangleFaces.bmp");

	
	
	// Aloca os dados do triângulo na GPU
	CreateTriangle();
	CreateCube();
	CreatePlane();

	// Inicializa a posição do triângulo
	posX = 0.0f;
	posY = 0.0f;
	posZ = 0.0f;

	// Velocidade de movimento do triângulo
	speed = 10.0f; 

	bBlendEnabled = true;
	fAlpha = 1.0f;

	piramidColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	fTileS = 1.0f;
	fTileT = 1.0f;
}

Scene0::~Scene0()
{
	DestroyTriangle();
	DestroyCube();
	DestroyPlane();

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

void Scene0::DrawScene(float deltaTime)
{
	// Update timer (must be called at start of frame)
	pTimer->Update();

	

	// Clear the color and depth buffers
	glClearColor(vFramebufferColor.r, vFramebufferColor.g, vFramebufferColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create transformations 
	// Note: In a real application, you would typically calculate the projection and view matrices 
	// once per frame and pass them to your shaders
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->Zoom), ASPECT_RATIO, Z_NEAR, Z_FAR);
	glm::mat4 view = pCamera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// Draw the 3D grid
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

	// Renderiza o triangulo
	pShader->Use("TextureColor");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	// Reseta a matriz de modelagem para a identidade
	model = glm::mat4(1.0);	

	// Translada o modelo para a posição desejada
	model = glm::translate(model, glm::vec3(posX, posY, posZ));

	// Rotaciona o modelo em torno do eixo Y
	model = glm::rotate(model, glm::radians((float)glfwGetTime() * 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Escala o modelo
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	pShader->SetMat4("model", model);

	// Passa a textura para o shader
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(3);
	pShader->SetInt("Texture0", 0);

	DrawTriangle();

///////////////////////////////////////////////////////////////////////////////

	// Renderiza o cubo
	pShader->Use("TextureColor");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(3.0f, 3.0f, 0.0f));
	pShader->SetMat4("model", model);

	// Passa a textura para o shader
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(2);
	pShader->SetInt("Texture0", 0);

	// Passa se deve habilitar o BLEND
	pShader->SetBool("bBlendEnabled", bBlendEnabled);
	pShader->SetFloat("fAlpha", fAlpha);

	// Habilita transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawCube();

	glDisable(GL_BLEND);	// Desabilita transparência



	///////////////////////////////////////////////////////////////////////////////

	// Renderiza o Plano
	pShader->Use("Textured");
	pShader->SetMat4("projection", projection);
	pShader->SetMat4("view", view);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	pShader->SetMat4("model", model);

	// Passa a textura para o shader
	glActiveTexture(GL_TEXTURE0);
	pTextures->ApplyTexture(1);
	pShader->SetInt("Texture0", 0);

	// Passa os valores para Tile Texture
	pShader->SetFloat("fTileS", fTileS);
	pShader->SetFloat("fTileT", fTileT);

	DrawPlane();


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
	pText->RenderText(std::format("[ENTER] GUI mode: {0}", bActiveGUI ? "true" : "false"), 5.0f, 60.0f, 0.3f);
	pText->RenderText(std::format("[DELETE] BLEND Enabled: {0}", bBlendEnabled), 5.0f, 40.0f, 0.3f);


	pText->RenderText(std::format("FPS: {0:.0f}\tDeltatime: {1:.2f}ms\tTimer: {2:.2f}s", pTimer->GetFPS(), pTimer->GetDeltaTime(), pTimer->GetTime() / 1000), 5.0f, 10.0f, 0.3f);
	glEnable(GL_DEPTH_TEST);
}


void Scene0::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	if (input.IsKeyJustPressed(GLFW_KEY_ENTER))
	{
		bActiveGUI = !bActiveGUI;
		input.SetCursorMode(bActiveGUI ?  GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

		std::cout << "GUI mode: '" << bActiveGUI << "'" << std::endl;
	}

	if (input.IsKeyJustPressed(GLFW_KEY_DELETE))
	{
		bBlendEnabled = !bBlendEnabled;
	}


	if (bActiveGUI == false)
	{
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

		// Triangle movement
		if (input.IsKeyPressed(GLFW_KEY_UP))
			posZ -= speed * deltaTime;
		if (input.IsKeyPressed(GLFW_KEY_DOWN))
			posZ += speed * deltaTime;
		if (input.IsKeyPressed(GLFW_KEY_LEFT))
			posX -= speed * deltaTime;
		if (input.IsKeyPressed(GLFW_KEY_RIGHT))
			posX += speed * deltaTime;
		if (input.IsKeyPressed(GLFW_KEY_PAGE_DOWN))
			posY -= speed * deltaTime;
		if (input.IsKeyPressed(GLFW_KEY_PAGE_UP))
			posY += speed * deltaTime;







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
	else
	{
		ShowGUI(); // Show Graphics User Interface
	}

}

void Scene0::ShowGUI()
{
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float;

	ImGui::Begin("Scene Options:");
	ImGui::SliderFloat("Alpha Value", &fAlpha, 0.0f, 1.0f, "%.1f");
	ImGui::ColorEdit4("Piramid Color", (float*)&piramidColor, flags);
	ImGui::Text("Framebuffer Color:");
	ImGui::ColorEdit4(" ", (float*)&vFramebufferColor);
	if (ImGui::Button("Reset Color"))
		vFramebufferColor = glm::vec4(0.1f, 0.1f, 0.15f, 1.0f);

	ImGui::InputFloat("TileS", &fTileS);
	ImGui::InputFloat("TileT", &fTileT);
	ImGui::End();
	
}



void Scene0::CreateTriangle()
{
	// Definição do triangulo
	float triangleData[] =
	{
	// Posições dos vértices	// Cores				// Tex Coords
    //    x     y     z           R      G      B		  S		 T
		-1.0f, 0.0f, 1.0f,		0.24f, 0.56f, 0.92f,	0.02f, 0.56f,  
		 1.0f, 0.0f, 1.0f,		0.24f, 0.56f, 0.92f,	0.435f, 0.56f, 
		 0.0f, 2.0f, 0.0f,		0.24f, 0.56f, 0.92f,	0.23f, 0.9f,   
														               
		 1.0f, 0.0f,  1.0f,		1.0f,  0.0f,  0.0f,  	0.53f, 0.56f,  
		 1.0f, 0.0f, -1.0f,		1.0f,  0.0f,  0.0f,		 0.93f, 0.56f, 
		 0.0f, 2.0f,  0.0f,		1.0f,  0.0f,  0.0f,		0.73f, 0.9f,   
														               
		 1.0f, 0.0f, -1.0f,		1.0f,  1.0f,  0.0f,		 0.02f, 0.1f,  
		-1.0f, 0.0f, -1.0f,		1.0f,  1.0f,  0.0f,		 0.435f, 0.1f, 
		 0.0f, 2.0f,  0.0f,		1.0f,  1.0f,  0.0f,		 0.23f, 0.45f, 
														               
		-1.0f, 0.0f, -1.0f,		0.0f,  1.0f,  0.0f,		 0.52f, 0.1f,  
		-1.0f, 0.0f,  1.0f,		0.0f,  0.0f,  1.0f,		 0.94f, 0.1f,  
		 0.0f, 2.0f,  0.0f,		1.0f,  0.0f,  0.0f,		 0.73f, 0.45f  
	};

	// Cria o VAO
	glGenVertexArrays(1, &triangleVAO);

	// Cria o VBO
	glGenBuffers(1, &triangleVBO);
	
	// Ativa o VAO
	glBindVertexArray(triangleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleData), triangleData, GL_STATIC_DRAW);

	// Define os atributos de cada vértice (VAO)
	// Diz à GPU como interpretar os dados do VBO
	int stride = 8 * sizeof(float);

	// Atributo 0: Posição do vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// Atributo 1: Cor do vértice
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Atributo 1: Cor do vértice
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Desvincula o VAO atual
	glBindVertexArray(0);
}

void Scene0::DrawTriangle()
{
	glBindVertexArray(triangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);
}

void Scene0::DestroyTriangle()
{
	glDeleteVertexArrays(1, &triangleVAO);
	glDeleteBuffers(1, &triangleVBO);
}

void Scene0::CreateCube()
{
	float cubeData[] =
	{
		// positions         // colors				// Coord. Textura
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,		0.25f, 0.25f,
	   -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,		0.5f, 0.25f,
	   -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,		0.5f, 0.5f,
	   -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,		0.5f, 0.5f,
		0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,		0.25f, 0.5f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,		0.25f, 0.25f,

	   -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		0.5f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		0.75f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		0.75f, 0.25f,
		0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		0.75f, 0.25f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		0.5f, 0.25f,
	   -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		0.5f, 0.0f,

	   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,		0.0f, 0.25f,
	   -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,		0.25f, 0.25f,
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,		0.25f, 0.5f,
	   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,		0.25f, 0.5f,
	   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,		0.0f, 0.5f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,		0.0f, 0.25f,

		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,		0.75f, 0.75f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,		0.5f, 0.75f,
		0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,		0.5f, 0.5f,
		0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,		0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,		0.75f, 0.5f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,		0.75f, 0.75f,

	   -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,		0.75f, 0.25f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,		1.0f, 0.25f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,		1.0f, 0.5f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,		1.0f, 0.5f,
	   -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,		0.75f, 0.5f,
	   -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,		0.75f, 0.25f,

	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,		0.5f, 0.25f,
		0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,		0.75f, 0.25f,
		0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,		0.75f, 0.5f,
		0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,		0.75f, 0.5f,
	   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,		0.5f, 0.5f,
	   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,		0.5f, 0.25f,
	};

	// Create VAO and VBO for the cube
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	int stride = 8 * sizeof(float);

	// Bind and set up the cube VAO and VBO
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// TexCoords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);


}

void Scene0::DrawCube()
{
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Scene0::DestroyCube()
{
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);	
}	


void Scene0::CreatePlane()
{
	float planeData[] =
	{
		// Positions            // Coord. Textura
	   -50.0f,  0.0f,  50.0f,	0.0f, 0.0f,
		50.0f,  0.0f,  50.0f,	1.0f, 0.0f,
		50.0f,  0.0f, -50.0f,	1.0f, 1.0f,
		50.0f,  0.0f, -50.0f,	1.0f, 1.0f,
	   -50.0f,  0.0f, -50.0f,	0.0f, 1.0f,
	   -50.0f,  0.0f,  50.0f,	0.0f, 0.0f
	};

	// Create VAO and VBO for the cube
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	int stride = 5 * sizeof(float);

	// Bind and set up the cube VAO and VBO
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeData), planeData, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// TexCoords attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Scene0::DrawPlane()
{
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Scene0::DestroyPlane()
{
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
}
#include "CSceneManager.h"
#include "Scenes/Scene6/Scene6.h"
#include "Scenes/UltimoSinal/UltimoSinal.h"

// Construtor
	
CSceneManager::CSceneManager()
{
	this->Scene = NULL;

	bWireframeMode = false; 
	bVsyncEnabled = true;

	// Inicializa ponteiros das cenas	
	// Cria a cena principal
	uiCurrentScene = 2; // Cena do grupo; aulas 0, 1 e 5 preservadas.
	iMenuCurrentScene = uiCurrentScene;
	ChangeScene(uiCurrentScene);
}


// Destrutor
CSceneManager::~CSceneManager(void)
{
	if (this->Scene)
	{
		delete this->Scene;
		this->Scene = NULL;
	}
}

void CSceneManager::DrawScene(float deltaTime)
{
	this->Scene->DrawScene(deltaTime);
}

void CSceneManager::ProcessSceneInput(GLFWwindow* window, float deltaTime)
{
	InputManager& input = InputManager::GetInstance();

	if (ImGui::GetIO().WantTextInput) { Scene->ProcessSceneInput(window, deltaTime); return; }

	// Close window on ESC
	if (input.IsKeyJustPressed(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	


	// Toggle cursor mode with TAB or VSync
	if (input.IsKeyJustPressed(GLFW_KEY_V))
	{
		bVsyncEnabled = !bVsyncEnabled;
		glfwSwapInterval(bVsyncEnabled);

		std::cout << "Vsync: '" << bVsyncEnabled << "'" << std::endl;
	}

	if (input.IsKeyJustPressed(GLFW_KEY_CAPS_LOCK))
	{
		// Toggle wireframe mode
		bWireframeMode = !bWireframeMode;

		std::cout << "Wireframe: '" << bWireframeMode << "'" << std::endl;
	}
	if (bWireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill mode
	
	// Seleciona a cena 0
	if (input.IsKeyJustPressed(GLFW_KEY_0))
		ChangeScene(0);

	// Seleciona a cena 1
	if (input.IsKeyJustPressed(GLFW_KEY_1))
		ChangeScene(1);
	
	if (input.IsKeyJustPressed(GLFW_KEY_2))
		ChangeScene(2);

	if (input.IsKeyJustPressed(GLFW_KEY_6)) ChangeScene(6);

	// Seleciona a cena 5
	if (input.IsKeyJustPressed(GLFW_KEY_5))
		ChangeScene(5);




	// Processa input da cena atual
	Scene->ProcessSceneInput(window, deltaTime);
}


void CSceneManager::ChangeScene(unsigned int _uiCurrentScene)
{
	std::system("cls"); // Clear Console on Windows

	// Disable Cursor for all scenes
	InputManager& input = InputManager::GetInstance();
	input.SetCursorMode(GLFW_CURSOR_DISABLED);

	this->uiCurrentScene = _uiCurrentScene;

	if (this->Scene)
	{
		delete this->Scene;
		this->Scene = NULL;
	}


	switch (uiCurrentScene)
	{
	case 2:
		this->Scene = new UltimoSinal();
		break;

	case 0:
		this->Scene = new Scene0();
		break;

	case 1:
		this->Scene = new Scene1();
		break;

	case 6:
		this->Scene = new Scene6();
		break;

	case 5:
		this->Scene = new Scene5();
		break;

	}
}


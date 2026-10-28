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

void CSceneManager::ProcessSceneInput(GLFWwindow* window,float dt){
 auto& input=InputManager::GetInstance();
 bool control=input.IsKeyPressed(GLFW_KEY_LEFT_CONTROL)||input.IsKeyPressed(GLFW_KEY_RIGHT_CONTROL);
 if(control&&!ImGui::GetIO().WantTextInput){
  for(auto pair:{std::pair<int,unsigned>{GLFW_KEY_0,0}, {GLFW_KEY_1,1},{GLFW_KEY_2,2},{GLFW_KEY_5,5},{GLFW_KEY_6,6}})if(input.IsKeyJustPressed(pair.first))ChangeScene(pair.second);
  if(input.IsKeyJustPressed(GLFW_KEY_V)){bVsyncEnabled=!bVsyncEnabled;glfwSwapInterval(bVsyncEnabled);}
 }
 if(uiCurrentScene!=2&&input.IsKeyJustPressed(GLFW_KEY_ESCAPE))glfwSetWindowShouldClose(window,true);
 glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);Scene->ProcessSceneInput(window,dt);
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


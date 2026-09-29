#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"



class Scene0 : public CSceneBase
{
public:
		Scene0();
		~Scene0();

		virtual void DrawScene(float deltaTime);
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);

		void ShowGUI();

		void CreateTriangle();
		void DrawTriangle();
		void DestroyTriangle();

		void CreateCube();
		void DrawCube();
		void DestroyCube();

		void CreatePlane();
		void DrawPlane();
		void DestroyPlane();



private:

	// Camera
	CCamera* pCamera;

	// Axis
	CGridAxis* pGridAxis;

	// Shader
	CShader* pShader;

	// Textures
	CTextures* pTextures;

	// Timer
	CTimer* pTimer;			

	// Texto
	CText* pText;			

	// Cor do Framebuffer
	glm::vec4 vFramebufferColor;

	bool bActiveGUI;

	GLuint triangleVAO, triangleVBO;
	GLuint cubeVAO, cubeVBO;
	GLuint planeVAO, planeVBO;

	float posX, posY, posZ;
	float speed;

	bool bBlendEnabled;
	float fAlpha;

	glm::vec4 piramidColor;

	float fTileS, fTileT;
};


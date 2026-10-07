#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CGridAxis.h"
#include "Engine/CShader.h"
#include "Engine/CTextures.h"
#include "Engine/CTimer.h"
#include "Engine/CText.h"
#include "Engine/CAsset.h"
#include "Engine/CSkyBox.h"


class Scene6 : public CSceneBase
{
public:
		Scene6();
		~Scene6();

		virtual void DrawScene(float deltaTime);
        virtual void ProcessSceneInput(GLFWwindow* window, float deltaTime);


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

	// SkyBox
	CSkybox* pSkyBox;
	
	// Asset Estátua
	CAsset* pStatue;
	CAsset* pCastle;


	
};


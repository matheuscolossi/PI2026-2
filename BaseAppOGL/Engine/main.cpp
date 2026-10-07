#include <fstream>
#include "ResourceRoot.h"
#include "Scenes/UltimoSinal/UltimoSinal.h"
#include "CSceneManager.h"

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Framebuffer size callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void PrintOpenGLInfo()
{
	std::ofstream outFile("OpenGL_Info.txt", std::ios::out | std::ios::trunc);
	if (outFile.is_open())
	{
		outFile << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
		outFile << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
		outFile << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		outFile << "Major OpenGL Version: " << major << std::endl;
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		outFile << "Minor OpenGL Version: " << minor << std::endl;

		outFile << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		GLint numExtensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
		outFile << "\nNumber of Supported Extensions : " << numExtensions << std::endl;
		outFile << "List of Supported Extensions:" << std::endl;
		for (int i = 0; i < numExtensions; i++) {
			const GLubyte* extension = glGetStringi(GL_EXTENSIONS, i);
			outFile << extension << std::endl;
		}

		outFile.close();
		std::cout << "OpenGL info written to OpenGL_Info.txt" << std::endl;
	}
	else
	{
		std::cout << "Failed to write OpenGL info to file" << std::endl;
	}
}

int main(int argc,char** argv)
{
	if(!PrepareResourceRoot())return 2;
    bool verify=argc>1 && std::string(argv[1])=="--verify-game";
    bool benchmark=argc>1 && std::string(argv[1])=="--benchmark";
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Configure GLFW to support OpenGL 4.6 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Get primary monitor info (optional, can be used for fullscreen or positioning)
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);
	int monitorX, monitorY;
	glfwGetMonitorPos(primary, &monitorX, &monitorY);

	// Create window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "O Ultimo Sinal | BaseAppOGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Calculate the centered position
	int posX = monitorX + (mode->width - SCR_WIDTH) / 2;
	int posY = monitorY + (mode->height - SCR_HEIGHT) / 2;
	glfwSetWindowPos(window, posX, posY);
	glfwShowWindow(window);

	// Initialize input manager
	InputManager& input = InputManager::GetInstance();
	input.Initialize(window);
	input.SetCursorMode(GLFW_CURSOR_DISABLED); // Capture cursor for FPS controls

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Configure OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// Culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	// Print OpenGL info on file
	PrintOpenGLInfo();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
    static const ImWchar textRanges[]={0x0020,0x00FF,0x2000,0x206F,0};
    if(std::filesystem::exists("Fonts/arial.ttf"))io.Fonts->AddFontFromFileTTF("Fonts/arial.ttf",17.f,nullptr,textRanges);
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


	// Initialize scene manager
	CSceneManager* pSceneManager = new CSceneManager();

    if(benchmark){
        bool complete=false;
        {
            UltimoSinal test;std::vector<double> samples;
            double begin=glfwGetTime(),previous=begin;
            while(!glfwWindowShouldClose(window)){
                double now=glfwGetTime(),elapsed=now-begin,ms=(now-previous)*1000.0;previous=now;
                if(elapsed>=150.0){complete=true;break;}
                if(elapsed>=30.0)samples.push_back(ms);
                ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();ImGui::NewFrame();
                test.BenchmarkPose(float(elapsed));test.DrawScene(float(ms*.001));
                ImGui::Render();ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                input.Update();glfwSwapBuffers(window);glfwPollEvents();
            }
            std::ofstream csv("benchmark_frames.csv");csv<<"frame_ms\n";for(double ms:samples)csv<<ms<<"\n";
            std::sort(samples.begin(),samples.end());
            if(!samples.empty())std::cout<<"BENCHMARK complete="<<complete<<" warmup=30s sample=120s vsync=on resolution=1280x720 count="<<samples.size()<<" median_ms="<<samples[samples.size()/2]<<" p95_ms="<<samples[std::size_t(std::ceil(samples.size()*.95))-1]<<std::endl;
            std::cout<<"GPU="<<glGetString(GL_RENDERER)<<" Driver="<<glGetString(GL_VERSION)<<std::endl;
        }
        delete pSceneManager;ImGui_ImplOpenGL3_Shutdown();ImGui_ImplGlfw_Shutdown();ImGui::DestroyContext();glfwTerminate();return complete?0:4;
    }
    if(verify){
        bool ok=true;
        {UltimoSinal test;ok=test.VerifyRuntime();}
        for(unsigned int id:{0u,1u,5u,6u,2u}){
            pSceneManager->ChangeScene(id);
            while(glGetError()!=GL_NO_ERROR){};
            ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();ImGui::NewFrame();
            pSceneManager->DrawScene(1.f/60.f);ImGui::Render();ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glFinish();auto error=glGetError();if(error!=GL_NO_ERROR)ok=false;
            std::cout<<"Scene "<<id<<" rendered; GL error="<<error<<std::endl;
        }
        delete pSceneManager;ImGui_ImplOpenGL3_Shutdown();ImGui_ImplGlfw_Shutdown();ImGui::DestroyContext();glfwTerminate();
        return ok?0:3;
    }
	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Calculate delta time
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process input
		pSceneManager->ProcessSceneInput(window, deltaTime);

		// Render scene
		pSceneManager->DrawScene(deltaTime);

		// Update input manager (must be called at end of frame)
		input.Update();

		// Render ImGui on top of the scene
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Cleanup scene manager
	if (pSceneManager)
	{
		delete pSceneManager;
		pSceneManager = NULL;
	}

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Terminate GLFW
	glfwTerminate();

	return 0;
}

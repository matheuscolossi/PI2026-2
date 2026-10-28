#include <fstream>
#include "ResourceRoot.h"
#include "Scenes/UltimoSinal/UltimoSinal.h"
#include "CSceneManager.h"

#ifdef _WIN32
// Per-process preference for Optimus notebooks. OS/OEM profiles retain priority.
// https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
extern "C" { __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; }
#endif

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
    bool benchmarkSmoke=argc>1 && std::string(argv[1])=="--benchmark-smoke";
    bool benchmark=benchmarkSmoke||(argc>1 && std::string(argv[1])=="--benchmark");
    bool previewTables=argc>1 && std::string(argv[1])=="--preview-tables";
    bool previewExperience=argc>1 && std::string(argv[1])=="--preview-experience";
    if(verify||benchmark||previewTables||previewExperience){wchar_t temp[32768]{};GetTempPathW(32768,temp);auto isolated=std::filesystem::path(temp)/(L"UltimoSinalSelfTest_"+std::to_wstring(GetCurrentProcessId()));SetEnvironmentVariableW(L"ULTIMO_TEST_HOME",isolated.c_str());}
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
    if(benchmark)glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);

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
	if(!benchmark)glfwShowWindow(window);

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
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


    // Read-only visual fixture: never loads or writes the player's profile.
    // Normal Jogar.cmd/Visual Studio startup is unchanged.
    if(previewTables){
        glfwSetWindowTitle(window,"O Ultimo Sinal | Conferencia das mesas");
        {
            UltimoSinal preview;preview.PreviewTables();
            while(!glfwWindowShouldClose(window)){
                ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();ImGui::NewFrame();
                preview.DrawScene(0.f);
                ImGui::Render();ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                if(input.IsKeyJustPressed(GLFW_KEY_ESCAPE))glfwSetWindowShouldClose(window,true);
                input.Update();glfwSwapBuffers(window);glfwPollEvents();
            }
        }
        ImGui_ImplOpenGL3_Shutdown();ImGui_ImplGlfw_Shutdown();ImGui::DestroyContext();glfwTerminate();return 0;
    }
	// Initialize scene manager
	CSceneManager* pSceneManager = new CSceneManager();

    if(benchmark){
        // Render an engine-owned fixed-size target. Hidden/minimized windows may
        // skip presentation, so a swap-loop alone is not a valid workload.
        while(glGetError()!=GL_NO_ERROR){};
        GLuint framebuffer=0,colorBuffer=0,depthBuffer=0,resolveFramebuffer=0,resolveBuffer=0;
        GLint maxSamples=0;glGetIntegerv(GL_MAX_SAMPLES,&maxSamples);
        glGenFramebuffers(1,&framebuffer);glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
        glGenRenderbuffers(1,&colorBuffer);glBindRenderbuffer(GL_RENDERBUFFER,colorBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER,4,GL_RGBA8,SCR_WIDTH,SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,colorBuffer);
        glGenRenderbuffers(1,&depthBuffer);glBindRenderbuffer(GL_RENDERBUFFER,depthBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER,4,GL_DEPTH24_STENCIL8,SCR_WIDTH,SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,depthBuffer);
        GLenum drawBuffer=GL_COLOR_ATTACHMENT0;glDrawBuffers(1,&drawBuffer);
        bool healthy=maxSamples>=4&&glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE&&glGetError()==GL_NO_ERROR;
        glGenFramebuffers(1,&resolveFramebuffer);glBindFramebuffer(GL_FRAMEBUFFER,resolveFramebuffer);
        glGenRenderbuffers(1,&resolveBuffer);glBindRenderbuffer(GL_RENDERBUFFER,resolveBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA8,SCR_WIDTH,SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,resolveBuffer);
        glDrawBuffers(1,&drawBuffer);
        healthy=healthy&&glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE&&glGetError()==GL_NO_ERROR;
        auto nonuniformFrame=[&](){
            glBindFramebuffer(GL_READ_FRAMEBUFFER,resolveFramebuffer);glReadBuffer(GL_COLOR_ATTACHMENT0);
            GLint alignment=4;glGetIntegerv(GL_PACK_ALIGNMENT,&alignment);glPixelStorei(GL_PACK_ALIGNMENT,1);
            std::array<unsigned char,3> first{},pixel{};bool varying=false;
            for(int y=0;y<5;++y)for(int x=0;x<5;++x){
                glReadPixels((x+1)*SCR_WIDTH/6,(y+1)*SCR_HEIGHT/6,1,1,GL_RGB,GL_UNSIGNED_BYTE,pixel.data());
                if(x==0&&y==0)first=pixel;else varying=varying||pixel!=first;
            }
            glPixelStorei(GL_PACK_ALIGNMENT,alignment);return varying&&glGetError()==GL_NO_ERROR;
        };
        bool complete=false;
        const double warmup=benchmarkSmoke?2.0:30.0,sampleTime=benchmarkSmoke?3.0:120.0;
        {
            UltimoSinal test;std::vector<double> samples;
            double begin=glfwGetTime();bool firstFrame=true;
            while(healthy&&!glfwWindowShouldClose(window)){
                const double frameBegin=glfwGetTime(),elapsed=frameBegin-begin;
                if(elapsed>=warmup+sampleTime){complete=true;break;}
                glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
                ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();
                io.DisplaySize=ImVec2(float(SCR_WIDTH),float(SCR_HEIGHT));io.DisplayFramebufferScale=ImVec2(1,1);
                ImGui::NewFrame();test.BenchmarkPose(float(elapsed));test.DrawScene(1.f/60.f);
                ImGui::Render();ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glBindFramebuffer(GL_READ_FRAMEBUFFER,framebuffer);glBindFramebuffer(GL_DRAW_FRAMEBUFFER,resolveFramebuffer);
                glBlitFramebuffer(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,SCR_WIDTH,SCR_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
                // Serialized CPU+GPU wall time, including the multisample resolve.
                glFinish();healthy=glGetError()==GL_NO_ERROR;
                const double ms=(glfwGetTime()-frameBegin)*1000.0;
                if(firstFrame){firstFrame=false;healthy=healthy&&nonuniformFrame();}
                if(elapsed>=warmup&&healthy)samples.push_back(ms);
                input.Update();glfwPollEvents();
            }
            healthy=healthy&&!firstFrame&&nonuniformFrame();
            std::cout<<"BENCHMARK framebuffer_nonuniform="<<healthy<<" first_and_last=checked"<<std::endl;
            std::ofstream csv("benchmark_frames.csv");csv<<"frame_ms\n";for(double ms:samples)csv<<ms<<"\n";
            std::sort(samples.begin(),samples.end());
            if(!samples.empty())std::cout<<"BENCHMARK complete="<<(complete&&healthy)<<" mode=offscreen-msaa4 warmup="<<warmup<<"s sample="<<sampleTime<<"s vsync=not_applicable resolution=1280x720 count="<<samples.size()<<" median_ms="<<samples[samples.size()/2]<<" p95_ms="<<samples[std::size_t(std::ceil(samples.size()*.95))-1]<<std::endl;
            std::cout<<"GPU="<<glGetString(GL_RENDERER)<<" Driver="<<glGetString(GL_VERSION)<<std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER,0);glDeleteRenderbuffers(1,&colorBuffer);glDeleteRenderbuffers(1,&depthBuffer);glDeleteFramebuffers(1,&framebuffer);glDeleteRenderbuffers(1,&resolveBuffer);glDeleteFramebuffers(1,&resolveFramebuffer);
        delete pSceneManager;ImGui_ImplOpenGL3_Shutdown();ImGui_ImplGlfw_Shutdown();ImGui::DestroyContext();glfwTerminate();return complete&&healthy?0:4;
    }
    if(verify){
        bool ok=true;
        {UltimoSinal test;ok=test.VerifyRuntime();ok=test.RenderReview()&&ok;}
        for(unsigned int id:{0u,1u,5u,6u,2u}){
            while(glGetError()!=GL_NO_ERROR){};
            pSceneManager->ChangeScene(id);
            auto loadError=glGetError();if(loadError!=GL_NO_ERROR)ok=false;std::cout<<"Scene "<<id<<" load GL error="<<loadError<<std::endl;
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

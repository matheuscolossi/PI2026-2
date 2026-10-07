#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include "MouseMotion.h"
#include <glm/glm.hpp>

class InputManager
{
public:
    static InputManager& GetInstance()
    {
        static InputManager instance;
        return instance;
    }

    // Initialize input manager with window
    void Initialize(GLFWwindow* window)
    {
        m_Window = window;
        
        // Set callbacks
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetScrollCallback(window, ScrollCallback);
        glfwSetWindowFocusCallback(window, FocusCallback);
        
        // Initialize mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        m_LastMouseX = (float)xpos;
        m_LastMouseY = (float)ypos;
        m_FirstMouse = true;
    }

    // Check if key is currently pressed
    bool IsKeyPressed(int key) const
    {
        auto it = m_Keys.find(key);
        return it != m_Keys.end() && it->second;
    }

    // Check if key was just pressed this frame
    bool IsKeyJustPressed(int key) const
    {
        auto it = m_KeysJustPressed.find(key);
        return it != m_KeysJustPressed.end() && it->second;
    }

    // Check if mouse button is pressed
    bool IsMouseButtonPressed(int button) const
    {
        auto it = m_MouseButtons.find(button);
        return it != m_MouseButtons.end() && it->second;
    }

    // Get mouse position
    glm::vec2 GetMousePosition() const
    {
        return glm::vec2(m_MouseX, m_MouseY);
    }

    // Get mouse delta (change since last frame)
    glm::vec2 GetMouseDelta() const
    {
        return glm::vec2(float(m_Motion.dx), float(m_Motion.dy));
    }

    // Get scroll offset
    float GetScrollOffset() const
    {
        return m_ScrollOffset;
    }

    // Update - call this at the end of each frame
    void Update()
    {
        // Reset per-frame state
        m_KeysJustPressed.clear();
        m_Motion.Clear();
        m_MouseDeltaX = 0.0f;
        m_MouseDeltaY = 0.0f;
        m_ScrollOffset = 0.0f;
    }

    // Set cursor mode (normal, hidden, disabled)
    void SetCursorMode(int mode)
    {
        if (!m_Window || glfwGetInputMode(m_Window,GLFW_CURSOR)==mode)return;
        m_Motion.Reset();
        glfwSetInputMode(m_Window, GLFW_CURSOR, mode);
        if(glfwRawMouseMotionSupported())
            glfwSetInputMode(m_Window,GLFW_RAW_MOUSE_MOTION,mode==GLFW_CURSOR_DISABLED?GLFW_TRUE:GLFW_FALSE);
        m_Motion.Reset();
    }

private:
    InputManager() 
        : m_Window(nullptr), 
          m_MouseX(0.0f), 
          m_MouseY(0.0f),
          m_LastMouseX(0.0f), 
          m_LastMouseY(0.0f),
          m_MouseDeltaX(0.0f), 
          m_MouseDeltaY(0.0f),
          m_ScrollOffset(0.0f),
          m_FirstMouse(true)
    {}

    // Prevent copying
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // Static callbacks
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        InputManager& input = GetInstance();
        
        if (action == GLFW_PRESS)
        {
            input.m_Keys[key] = true;
            input.m_KeysJustPressed[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            input.m_Keys[key] = false;
        }
    }

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos)
    {
        InputManager& input = GetInstance();
        
        input.m_Motion.Add(xpos,ypos);
        input.m_MouseX=float(xpos);input.m_MouseY=float(ypos);
    }

    static void FocusCallback(GLFWwindow*,int focused) {
        auto& input=GetInstance(); input.m_Motion.Reset();
        if(!focused){input.m_Keys.clear();input.m_KeysJustPressed.clear();input.m_MouseButtons.clear();input.m_ScrollOffset=0;}
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        InputManager& input = GetInstance();
        
        if (action == GLFW_PRESS)
        {
            input.m_MouseButtons[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            input.m_MouseButtons[button] = false;
        }
    }

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        InputManager& input = GetInstance();
        input.m_ScrollOffset += (float)yoffset;
    }

    MouseMotion m_Motion;
    GLFWwindow* m_Window;
    std::map<int, bool> m_Keys;
    std::map<int, bool> m_KeysJustPressed;
    std::map<int, bool> m_MouseButtons;
    
    float m_MouseX, m_MouseY;
    float m_LastMouseX, m_LastMouseY;
    float m_MouseDeltaX, m_MouseDeltaY;
    float m_ScrollOffset;
    bool m_FirstMouse;
};

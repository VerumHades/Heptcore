#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace Heptcore
{
    class Window{
        private:
            GLFWwindow* window;
        public:
            Window(int width, int height, std::string title);
            
            bool shouldClose();
            void swapBuffers();
            void pollEvents();

            ~Window();
    };
} 

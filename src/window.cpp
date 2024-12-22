#include <window.hpp>

using namespace Heptcore;

Window::Window(int width, int height, std::string title){
    /* Initialize the library */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize glfw!" << std::endl;
        throw std::runtime_error("Failed to initialize glfw!");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    
    ///glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (!window) {
        std::cerr << "Failed to initialize glfw window!" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to initialize glfw window!");
        return;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr  << "Failed to initialize glad!" << std::endl;
        throw std::runtime_error("Failed to initialize glad!");
        return;
    }

    //std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

bool Window::shouldClose(){
    return glfwWindowShouldClose(window);
}

void Window::swapBuffers(){
    glfwSwapBuffers(window);
}

void Window::pollEvents(){
    glfwPollEvents();
}


Window::~Window(){
    glfwDestroyWindow(window);
    glfwTerminate();
}
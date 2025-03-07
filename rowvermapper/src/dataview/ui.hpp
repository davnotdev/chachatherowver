#ifndef UI_HPP
#define UI_HPP

#include <string>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/glm.hpp>

class UI {
public:
    UI();
    virtual ~UI() {}
    
    void run();
    void enableWireframe();
    void disableWireframe();
    void drawSquareCenter(glm::vec2 position, glm::vec2 scale, glm::vec3 color = glm::vec3(0.0, 0.0, 0.0));
    void drawSquareBounds(glm::vec2 leftTop, glm::vec2 rightBottom, glm::vec3 color = glm::vec3(0.0, 0.0, 0.0));
    void setCameraZoom(float cameraZoom);

    virtual void render(ImGuiIO& io) = 0;
private:
    GLFWwindow* window;
    ImGuiIO* io;

    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;

    unsigned int vao;
    unsigned int vbo;
    unsigned int program;

    float cameraZoom = 1.0f;

    unsigned int loadShader(std::string fileLoc, int shaderType);
};

#endif

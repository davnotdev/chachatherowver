#include "ui.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

UI::UI() {
    glfwInit();
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "dataview", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::exit(-1);
    }

    glfwShowWindow(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    float vertices[] = {
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,

        -0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::string filepath = __FILE__;
    size_t pos = filepath.find_last_of("/\\");
    std::string directory = filepath.substr(0, pos);

    unsigned int vertShader = loadShader(directory + "/shaders/main.vert", GL_VERTEX_SHADER);
    unsigned int fragShader = loadShader(directory + "/shaders/main.frag", GL_FRAGMENT_SHADER);

    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void UI::run() {
    ImGuiIO& io = *this->io;

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0, 1.0, 1.0, 1.0);

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        this->render(io);
        ImGui::EndFrame();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void UI::enableWireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void UI::disableWireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void UI::drawSquareCenter(glm::vec2 position, glm::vec2 scale, glm::vec3 color) {
    glBindVertexArray(vao);
    glUseProgram(program);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    projection = glm::ortho(0.0f, (float)WINDOW_WIDTH * cameraZoom, 0.0f, (float)WINDOW_WIDTH * cameraZoom, 0.1f, 100.0f);

    view = glm::translate(view, glm::vec3((float)WINDOW_WIDTH * cameraZoom / 2.0f, (float)WINDOW_HEIGHT * cameraZoom / 2.0f, -3.0f));

    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);

    glUniform3f(glGetUniformLocation(program, "u_color"), color.x, color.y, color.z);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void UI::drawSquareBounds(glm::vec2 leftTop, glm::vec2 rightBottom, glm::vec3 color) {
    float scaleX = (rightBottom.x - leftTop.x);
    float scaleY = (rightBottom.y - leftTop.y);

    float posX = (leftTop.x + rightBottom.x) / 2.0;
    float posY = (leftTop.y + rightBottom.y) / 2.0;

    drawSquareCenter(glm::vec2(posX, posY), glm::vec2(scaleX, scaleY), color);
}

void UI::setCameraZoom(float cameraZoom) {
    this->cameraZoom = cameraZoom;
}

unsigned int UI::loadShader(std::string fileLoc, int shaderType) {
    std::string shaderContent;
    std::ifstream file(fileLoc);
    std::string line;

    while (std::getline(file, line)) {
        shaderContent += line + '\n';
    }
    file.close();

    const char* cShaderContent = shaderContent.c_str();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &cShaderContent, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << fileLoc << " compilation failed:" << infoLog << std::endl;
    }

    return shader;
}

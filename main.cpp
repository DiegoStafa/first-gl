/*
This is my first opengl program following the learnopengl.com tutorial.
The program diplays a fractal made of cubes.

Libraries:
- glew  --> opengl function linker (no need to specify which library to use)
- glfw  --> cross platform context and window initializer (no need to interface with OS window API)
- glm   --> math library for matrices and stuff
- stb   --> simplifies texture/image loading

functionality:
- camera movement (mouse & arrow keys)
- camera acceleration (spacebar)
- diffuse lighting (color: 1-6, intensity: '+'. '-')
- fps (standard output)
*/

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "src/ShaderTools.h"

using glm::mat4;
using glm::vec2;
using glm::vec3;

#pragma region GLOBALS

// --- screen --- //
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
// -------------- //

// --- MVP matrices --- //
mat4 model = mat4(1.0f);
mat4 view = mat4(1.0f);
mat4 proj = glm::perspective(glm::radians(90.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
mat4 mvp;
// -------------------- //

// --- camera --- //
vec3 cameraPos = vec3(0.0f, 0.0f, 0.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
vec3 cameraDirection = vec3(0.0f, 0.0f, 1.0f);
float cameraSpeed = 2.0f;
// -------------- //

// --- cursor --- //
float lastX = (float)SCREEN_WIDTH / 2.0f;
float lastY = (float)SCREEN_HEIGHT / 2.0f;
float yaw = 0.0f;
float pitch = 0.0f;
float sensitivity = 0.2f;
float fov = 45.0f;
bool firstMouse = true;
// -------------- //

// --- time --- //
float timeCurrentFrame = 0.0f;
float timeLastFrame = 0.0f;
float deltaTime = 0.0f;
float startTime = 0.0f;
int fps = 0;
// ------------ //

// --- ambient light --- //
vec3 ambientLGT(0.5f, 0.5f, 0.5f);
float ambientSTR = 1.0f;
// --------------------- //

// --- diffuse light --- //
vec3 diffusePos(0.0f, 0.0f, 0.0f);
float diffuseSTR = 1.0f;
// --------------------- //

// --- position set --- //
const int numPositions = 180;
vec3 positions[numPositions];
// -------------------- //

#pragma endregion

/// set camera direction based on mouse input
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    /// first frame case
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    /// get mouse offsets between current and last frame
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    /// increase offsets based on mouse sensitivity
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    /// update pitch yaw and roll (roll always 0 in this case)
    yaw += xoffset;
    pitch += yoffset;

    /// set camera constraints
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    /// update camera direction
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection = glm::normalize(direction);
}

/// gen a set of positions
void genPositions()
{
    positions[0] = vec3(0.0f, 0.0f, 0.0f);
    int lastIns = 0;
    float scale = 2.0f;
    int nextScale = 1;
    int pow = 1;

    for (int i = 0; i < numPositions && lastIns + 3 < numPositions; i++)
    {
        float offset = 8.0f / scale;
        positions[lastIns + 1] = positions[i] + vec3(-offset, -offset, 0.0f);
        positions[lastIns + 2] = positions[i] + vec3(0.0f, offset, 0);
        positions[lastIns + 3] = positions[i] + vec3(offset, -offset, -0.0f);

        nextScale--;
        if (nextScale == 0)
        {
            scale *= 2.0f;
            nextScale = glm::pow(3, pow);
            pow++;
        }

        lastIns += 3;
    }
}

/// init glfw and glew
GLFWwindow *createWindow(int w, int h)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(w, h, "DiegoGL", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glewInit();

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3f, 0, 0, 0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    return window;
}

/// get input in window
void processInput(GLFWwindow *window)
{
    vec3 direction(0.0f, 0.0f, 0.0f);

    /// ambient strength
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        ambientSTR += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        ambientSTR -= 0.1f;

    /// diffuse strength
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        diffuseSTR += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        diffuseSTR -= 0.1f;


    /// ambient color
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && ambientLGT.r < 1.0f)
        ambientLGT.r += 0.005f;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && ambientLGT.r > 0.0f)
        ambientLGT.r -= 0.005f;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && ambientLGT.g < 1.0f)
        ambientLGT.g += 0.005f;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && ambientLGT.g > 0.0f)
        ambientLGT.g -= 0.005f;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && ambientLGT.b < 1.0f)
        ambientLGT.b += 0.005f;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && ambientLGT.b > 0.0f)
        ambientLGT.b -= 0.005f;

    /// camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += cameraDirection;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction += -glm::normalize(glm::cross(cameraDirection, cameraUp));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction += -cameraDirection;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += glm::normalize(glm::cross(cameraDirection, cameraUp));

    if (direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f)
        cameraPos += glm::normalize(direction) * cameraSpeed * deltaTime;

    /// camera speed
    cameraSpeed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) ? 10.0f : 2.0f;
}

/// draw a frame in window
void render1(GLFWwindow *window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

    float scale = 2.0f;
    int nextScale = 1;
    int pow = 1;

    // disegna lo stesso cubo con gli stessi colori in posizioni diverse
    for (int i = 0; i < numPositions; i++)
    {
        mat4 translation = glm::translate(model, positions[i]);
        mat4 scaling = glm::scale(model, vec3(scale, scale, scale));
        mat4 model = translation * scaling;
        mat4 mvp = proj * view * model;
        glUniform3f(3, ambientLGT.r, ambientLGT.g, ambientLGT.b);
        glUniform1f(4, ambientSTR);
        glUniform3f(5, diffusePos.x, diffusePos.y, diffusePos.z);
        glUniformMatrix4fv(9, 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(mvp));

        // chiama la draw, le uniform sono sempre le stesse tranne per mvp
        glDrawArrays(GL_TRIANGLES, 0, 36);

        nextScale--;
        if (nextScale == 0)
        {
            scale /= 2.0f;
            nextScale = glm::pow(3, pow);
            pow++;
        }
    }
    // ---
    glfwSwapBuffers(window);
}

/// draw a frame in window
void render2(GLFWwindow *window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    diffusePos.x = glm::cos(glfwGetTime()) * 3;
    diffusePos.y = glm::cos(glfwGetTime()) * 0;
    diffusePos.z = glm::sin(glfwGetTime()) * 3;

    model = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
    view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
    mvp = proj * view * model;
    glUniform3f(3, ambientLGT.r, ambientLGT.g, ambientLGT.b);
    glUniform1f(4, ambientSTR);
    glUniform3f(5, diffusePos.x, diffusePos.y, diffusePos.z);
    glUniform1f(6, diffuseSTR);
    glUniform3f(7, cameraPos.x, cameraPos.y, cameraPos.z);

    glUniformMatrix4fv(9, 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(mvp));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(mat4(1.0f), diffusePos);
    view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
    mvp = proj * view * model;

    glUniform3f(3, ambientLGT.r, ambientLGT.g, ambientLGT.b);
    glUniform1f(4, ambientSTR);
    glUniform3f(5, diffusePos.x, diffusePos.y, diffusePos.z);
    glUniform1f(6, diffuseSTR);
    glUniform3f(7, cameraPos.x, cameraPos.y, cameraPos.z);

    glUniformMatrix4fv(9, 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(2, 1, GL_FALSE, value_ptr(mvp));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
}

/// create opengl buffers and define data formats
void prepareData()
{
    // --- data --- //
    vec3 verts[36] = {
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(+0.5f, -0.5f, -0.5f),
        vec3(+0.5f, +0.5f, -0.5f),
        vec3(+0.5f, +0.5f, -0.5f),
        vec3(-0.5f, +0.5f, -0.5f),
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(-0.5f, -0.5f, +0.5f),
        vec3(+0.5f, -0.5f, +0.5f),
        vec3(+0.5f, +0.5f, +0.5f),
        vec3(+0.5f, +0.5f, +0.5f),
        vec3(-0.5f, +0.5f, +0.5f),
        vec3(-0.5f, -0.5f, +0.5f),
        vec3(-0.5f, +0.5f, +0.5f),
        vec3(-0.5f, +0.5f, -0.5f),
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(-0.5f, -0.5f, +0.5f),
        vec3(-0.5f, +0.5f, +0.5f),
        vec3(+0.5f, +0.5f, +0.5f),
        vec3(+0.5f, +0.5f, -0.5f),
        vec3(+0.5f, -0.5f, -0.5f),
        vec3(+0.5f, -0.5f, -0.5f),
        vec3(+0.5f, -0.5f, +0.5f),
        vec3(+0.5f, +0.5f, +0.5f),
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(+0.5f, -0.5f, -0.5f),
        vec3(+0.5f, -0.5f, +0.5f),
        vec3(+0.5f, -0.5f, +0.5f),
        vec3(-0.5f, -0.5f, +0.5f),
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(-0.5f, +0.5f, -0.5f),
        vec3(+0.5f, +0.5f, -0.5f),
        vec3(+0.5f, +0.5f, +0.5f),
        vec3(+0.5f, +0.5f, +0.5f),
        vec3(-0.5f, +0.5f, +0.5f),
        vec3(-0.5f, +0.5f, -0.5f),
    };
    vec2 tex[36] = {
        vec2(+0.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+1.0f, +1.0f),
        vec2(+1.0f, +1.0f),
        vec2(+0.0f, +1.0f),
        vec2(+0.0f, +0.0f),
        vec2(+0.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+1.0f, +1.0f),
        vec2(+1.0f, +1.0f),
        vec2(+0.0f, +1.0f),
        vec2(+0.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+1.0f, +1.0f),
        vec2(+0.0f, +1.0f),
        vec2(+0.0f, +1.0f),
        vec2(+0.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+1.0f, +1.0f),
        vec2(+0.0f, +1.0f),
        vec2(+0.0f, +1.0f),
        vec2(+0.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+0.0f, +1.0f),
        vec2(+1.0f, +1.0f),
        vec2(+1.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+0.0f, +0.0f),
        vec2(+0.0f, +1.0f),
        vec2(+0.0f, +1.0f),
        vec2(+1.0f, +1.0f),
        vec2(+1.0f, +0.0f),
        vec2(+1.0f, +0.0f),
        vec2(+0.0f, +0.0f),
        vec2(+0.0f, +1.0f)};
    float vertices[] = {
        //     POSITION     |      NORMALS    //
        -0.5f, -0.5f, -0.5f, +0.0f, +0.0f, -1.0f,
        +0.5f, -0.5f, -0.5f, +0.0f, +0.0f, -1.0f,
        +0.5f, +0.5f, -0.5f, +0.0f, +0.0f, -1.0f,
        +0.5f, +0.5f, -0.5f, +0.0f, +0.0f, -1.0f,
        -0.5f, +0.5f, -0.5f, +0.0f, +0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, +0.0f, +0.0f, -1.0f,

        -0.5f, -0.5f, +0.5f, +0.0f, +0.0f, +1.0f,
        +0.5f, -0.5f, +0.5f, +0.0f, +0.0f, +1.0f,
        +0.5f, +0.5f, +0.5f, +0.0f, +0.0f, +1.0f,
        +0.5f, +0.5f, +0.5f, +0.0f, +0.0f, +1.0f,
        -0.5f, +0.5f, +0.5f, +0.0f, +0.0f, +1.0f,
        -0.5f, -0.5f, +0.5f, +0.0f, +0.0f, +1.0f,

        -0.5f, +0.5f, +0.5f, -1.0f, +0.0f, +0.0f,
        -0.5f, +0.5f, -0.5f, -1.0f, +0.0f, +0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, +0.0f, +0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, +0.0f, +0.0f,
        -0.5f, -0.5f, +0.5f, -1.0f, +0.0f, +0.0f,
        -0.5f, +0.5f, +0.5f, -1.0f, +0.0f, +0.0f,

        +0.5f, +0.5f, +0.5f, +1.0f, +0.0f, +0.0f,
        +0.5f, +0.5f, -0.5f, +1.0f, +0.0f, +0.0f,
        +0.5f, -0.5f, -0.5f, +1.0f, +0.0f, +0.0f,
        +0.5f, -0.5f, -0.5f, +1.0f, +0.0f, +0.0f,
        +0.5f, -0.5f, +0.5f, +1.0f, +0.0f, +0.0f,
        +0.5f, +0.5f, +0.5f, +1.0f, +0.0f, +0.0f,

        -0.5f, -0.5f, -0.5f, +0.0f, -1.0f, +0.0f,
        +0.5f, -0.5f, -0.5f, +0.0f, -1.0f, +0.0f,
        +0.5f, -0.5f, +0.5f, +0.0f, -1.0f, +0.0f,
        +0.5f, -0.5f, +0.5f, +0.0f, -1.0f, +0.0f,
        -0.5f, -0.5f, +0.5f, +0.0f, -1.0f, +0.0f,
        -0.5f, -0.5f, -0.5f, +0.0f, -1.0f, +0.0f,

        -0.5f, +0.5f, -0.5f, +0.0f, +1.0f, +0.0f,
        +0.5f, +0.5f, -0.5f, +0.0f, +1.0f, +0.0f,
        +0.5f, +0.5f, +0.5f, +0.0f, +1.0f, +0.0f,
        +0.5f, +0.5f, +0.5f, +0.0f, +1.0f, +0.0f,
        -0.5f, +0.5f, +0.5f, +0.0f, +1.0f, +0.0f,
        -0.5f, +0.5f, -0.5f, +0.0f, +1.0f, +0.0f};
    // ------------ //

    // --- img fetch --- //
    int img_w, img_h, n_ch;
    const char *img_path = "./container.jpg";
    unsigned char *img_data = stbi_load(img_path, &img_w, &img_h, &n_ch, 0);
    // ----------------- //

    // --- buffer gen --- //
    GLuint vaoID, vboID, vboTexID, texture;
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glGenBuffers(1, &vboTexID);
    glGenTextures(1, &texture);
    glBindVertexArray(vaoID);
    // ------------------ //

    // --- cube coordinates --- //
    glBindBuffer(GL_ARRAY_BUFFER, vboID);                               // definisce il data buffer in uso
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_STATIC_DRAW); // crea il buffer in memoria
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);    // popola il buffer
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // ------------------------ //

    // --- tex coordinates --- //
    glBindBuffer(GL_ARRAY_BUFFER, vboTexID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tex), tex);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // ----------------------- //

    // --- tex uniform --- //
    glBindTexture(GL_TEXTURE_2D, texture);                        // definisce il texture buffer in uso
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // hints
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_w, img_h, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data); // produce una texture da un bitmap
    glGenerateMipmap(GL_TEXTURE_2D);                                                             // genera la mipmap della texture
    stbi_image_free(img_data);
    // ------------------- //
}

/// update current time and delta time
void timeManager()
{
    timeCurrentFrame = (float)glfwGetTime();
    deltaTime = timeCurrentFrame - timeLastFrame;
    timeLastFrame = timeCurrentFrame;
}

/// show fps
void showFPS()
{
    fps++;

    if (timeCurrentFrame - startTime >= 1.0)
    {
        std::cout << fps << " fps\n";
        fps = 0;
        startTime = timeCurrentFrame;
    }
}

int main()
{
    GLFWwindow *window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    GLuint prog = ShaderProgram("shaders/vert.glsl", "shaders/frag.glsl");
    glUseProgram(prog);
    prepareData();
    //genPositions();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);
        render2(window);
        timeManager();
        showFPS();
    }

    glfwTerminate();
    return 0;
}

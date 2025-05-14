#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <SOIL2/SOIL2.h>


GLuint sunTexture;

using namespace std;

GLchar windowTitle[] = "Beadando 3.";
GLuint windowWidth = 800;
GLuint windowHeight = 800;

static float degree = 0.0f;
static float degree2 = 0.0f;
static float degree3 = 90.0f;
static float increment = 2.0f;
static float increment2 = 2.0f;
static float increment3 = 2.0f;

float cameraRadius = 8.0f;
float cameraAngle = 0.0f;
float cameraHeight = 0.0f;

bool lightOn = true;
float deltaTime = 0.0f, lastTime = 0.0f;

GLuint VBO[2];
GLuint VAO[2];

GLFWwindow* window;
GLuint renderingProgram;

glm::mat4 model, view, projection;
glm::mat4 invTmatrix;
glm::vec3 cameraPos = glm::vec3(cameraRadius, cameraAngle, cameraHeight);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraMovingZ = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(0.0, 0.0, 5.0);
glm::vec3 cubePos = glm::vec3(0.0, 0.0, 5.0);

void loadTexture() {
    sunTexture = SOIL_load_OGL_texture(
        "sun.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (sunTexture == 0) {
        std::cerr << "Failed to load texture: sun.jpg" << std::endl;
    }

    // Optional texture settings
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

float cubeVertices[] = {
    // positions              // normals
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

vector<float> generateSphereVertices(float radius, int sectorCount, int stackCount) {
    vector<float> tempVertices;
    vector<float> finalVertices;

    float x, y, z, nx, ny, nz, s, t;
    float pi = acos(-1);

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = pi / 2 - i * pi / stackCount;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * pi / sectorCount;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            nx = x / radius;
            ny = y / radius;
            nz = z / radius;
            s = (float)j / sectorCount;
            t = (float)i / stackCount;

            tempVertices.insert(tempVertices.end(), { x, y, z, nx, ny, nz, s, t });
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        for (int j = 0; j < sectorCount; ++j) {
            int k1 = i * (sectorCount + 1) + j;
            int k2 = k1 + sectorCount + 1;

            // First triangle
            for (int a = 0; a < 8; ++a) finalVertices.push_back(tempVertices[k1 * 8 + a]);
            for (int a = 0; a < 8; ++a) finalVertices.push_back(tempVertices[k2 * 8 + a]);
            for (int a = 0; a < 8; ++a) finalVertices.push_back(tempVertices[(k1 + 1) * 8 + a]);

            // Second triangle
            for (int a = 0; a < 8; ++a) finalVertices.push_back(tempVertices[k2 * 8 + a]);
            for (int a = 0; a < 8; ++a) finalVertices.push_back(tempVertices[(k2 + 1) * 8 + a]);
            for (int a = 0; a < 8; ++a) finalVertices.push_back(tempVertices[(k1 + 1) * 8 + a]);
        }
    }

    return finalVertices;
}


void computeCameraMatrices()
{
    float x = cameraRadius * cos(glm::radians(cameraAngle));
    float y = cameraRadius * sin(glm::radians(cameraAngle));
    float z = cameraHeight;

    cameraPos = glm::vec3(x, y, z);
    view = glm::lookAt(cameraPos, cameraTarget, up);
    projection = glm::perspective(glm::radians(55.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
}

bool checkOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        cout << "glError: " << glErr << endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

void printShaderLog(GLuint shader) {
    int len = 0;
    int chWrittn = 0;
    char* log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetShaderInfoLog(shader, len, &chWrittn, log);
        cout << "Shader Info Log: " << log << endl;
        free(log);
    }
}

void printProgramLog(int prog) {
    int len = 0;
    int chWrittn = 0;
    char* log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetProgramInfoLog(prog, len, &chWrittn, log);
        cout << "Program Info Log: " << log << endl;
        free(log);
    }
}

string readShaderSource(const char* filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

GLuint createShaderProgram() {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;

    string vertShaderStr = readShaderSource("vertexShader.glsl");
    string fragShaderStr = readShaderSource("fragmentShader.glsl");

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();

    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);

    glCompileShader(vShader);
    checkOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1) {
        cout << "vertex compilation failed" << endl;
        printShaderLog(vShader);
    }

    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1) {
        cout << "fragment compilation failed" << endl;
        printShaderLog(fShader);
    }

    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);

    glLinkProgram(vfProgram);
    checkOpenGLError();
    glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        cout << "linking failed" << endl;
        printProgramLog(vfProgram);
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return vfProgram;
}

void cubecoordinate()
{
    degree3 -= increment3;
    if (degree3 <= -360.0f)
    {
        degree3 = 0.0f;
    }
    float radian = degree3 * M_PI / 180.0f;
    lightPos = glm::vec3(5 * cos(radian), 0.0, 5 * sin(radian));
}

void lightcoordinate()
{
    degree2 += increment2;
    if (degree2 >= 360.0f)
    {
        degree2 = 0.0f;
    }
    float radian = degree2 * M_PI / 180.0f;
    cubePos = glm::vec3(2 * cameraRadius * cos(radian), 2 * cameraRadius * sin(radian), 0.0);
}

void circlecoordinate(bool direction)
{
    if (direction == true)
    {
        degree += increment;
        if (degree >= 360.0f)
        {
            degree = 0.0f;
        }
    }
    else
    {
        degree -= increment;
        if (degree <= -360.0f)
        {
            degree = 0.0f;
        }
    }
    float radian = degree * M_PI / 180.0f;
    cameraPos = glm::vec3(8 * cos(radian), 8 * sin(radian), cameraPos.z);
}

void cleanUpScene()
{
    glfwDestroyWindow(window);
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteProgram(renderingProgram);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    float angleStep = 2.0f;
    float heightStep = 0.2f;

    if (key == GLFW_KEY_LEFT)
        cameraAngle -= angleStep;
    if (key == GLFW_KEY_RIGHT)
        cameraAngle += angleStep;
    if (key == GLFW_KEY_UP)
        cameraHeight += heightStep;
    if (key == GLFW_KEY_DOWN)
        cameraHeight -= heightStep;
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        lightOn = !lightOn;
    if (key == GLFW_KEY_ESCAPE)
        cleanUpScene();
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();

    vector<float> sphereVertices = generateSphereVertices(1.0f, 20, 20);

    glGenBuffers(2, VBO);
    glGenVertexArrays(2, VAO);

    // CUBES
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // SPHERE
    // SPHERE
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void display(GLFWwindow* window, double currentTime) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(renderingProgram);

    computeCameraMatrices();
    lightcoordinate();
    cubecoordinate();

    vector<float> sphereVertices = generateSphereVertices(0.5f, 20, 20);

    for (int i = 0; i < 4; i++)
    {
        if (i == 0)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else if (i == 1)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
        }
        else if (i == 2)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
        }
        else
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePos);
        }

        unsigned int modelLoc = glGetUniformLocation(renderingProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(renderingProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(renderingProgram, "projection");
        unsigned int lightOnLoc = glGetUniformLocation(renderingProgram, "lightOn");
        unsigned int isLightLoc = glGetUniformLocation(renderingProgram, "isLight");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(lightOnLoc, lightOn);
        glUniform1i(isLightLoc, i == 3);

        invTmatrix = glm::inverseTranspose(view * model);
        unsigned int modelViewLoc = glGetUniformLocation(renderingProgram, "invTMatrix");
        glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, glm::value_ptr(invTmatrix));

        glUniform3fv(glGetUniformLocation(renderingProgram, "lightPos"), 1, &lightPos[0]);

        //SPHERE
        if (i == 3) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sunTexture);
            glUniform1i(glGetUniformLocation(renderingProgram, "sunSampler"), 0);

            glBindVertexArray(VAO[1]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, sphereVertices.size() / 8); // 8 = pos+normal+texCoord
        }

        else {
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(cubeVertices) / (6 * sizeof(float)));
        }
        glBindVertexArray(0);
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    glfwSwapInterval(1);

    init(window);
    loadTexture();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cleanUpScene();
}
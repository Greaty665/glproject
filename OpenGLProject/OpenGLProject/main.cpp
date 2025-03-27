#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#define M_PI 3.14159265358979323846

using namespace std;

const int windowWidth = 600, windowHeight = 600;
float circleX = 300.0f, circleY = 300.0f;
float lineY = 300.0f;
float circleRadius = 50.0f;
float speedX = 2.0f;
bool movingRight = true;
bool isMoving = false;

float angle = 25.0f;
float directionX = cos(angle * M_PI / 180.0f) * 2.0f;
float directionY = sin(angle * M_PI / 180.0f) * 2.0f;

GLuint shaderProgram;
GLuint VAO, VBO;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        lineY -= 5.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        lineY += 5.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !isMoving) {
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void updateCircle() {
    if (!isMoving) {
        if (movingRight) {
            circleX += speedX;
            if (circleX + circleRadius >= windowWidth) movingRight = false;
        }
        else {
            circleX -= speedX;
            if (circleX - circleRadius <= 0) movingRight = true;
        }
    }
    else {
        circleX += directionX;
        circleY += directionY;
        if (circleX + circleRadius >= windowWidth || circleX - circleRadius <= 0) {
            directionX = -directionX;
        }
        if (circleY + circleRadius >= windowHeight || circleY - circleRadius <= 0) {
            directionY = -directionY;
        }
    }
}

void drawCircle() {
    bool intersecting = (circleY - circleRadius <= lineY + 1.5f &&
        circleY + circleRadius >= lineY - 1.5f &&
        circleX + circleRadius >= 200.0f &&
        circleX - circleRadius <= 400.0f);

    glUseProgram(shaderProgram);
    glUniform2f(glGetUniformLocation(shaderProgram, "uCircleCenter"), circleX, windowHeight - circleY);
    glUniform1f(glGetUniformLocation(shaderProgram, "uCircleRadius"), circleRadius);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIntersecting"), intersecting);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


void drawHorizontalLine() {
    glUseProgram(0);
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(200, lineY);
    glVertex2f(400, lineY);
    glEnd();
}

void display() {
    processInput(glfwGetCurrentContext());
    glClear(GL_COLOR_BUFFER_BIT);

    drawHorizontalLine(); 
    drawCircle();
    updateCircle();

    glfwSwapBuffers(glfwGetCurrentContext());
}

void initOpenGL() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f); 
}

string readShaderFile(const char* filePath) {
    string content;
    ifstream file(filePath, ios::in);
    if (!file.is_open()) {
        cerr << "Nem sikerült megnyitni a shader fájlt: " << filePath << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    content = buffer.str();
    return content;
}

void setupShader() {

    string vertexShaderStr = readShaderFile("vertexShader.vert");
    string fragmentShaderStr = readShaderFile("fragmentShader.frag");
    
    if (vertexShaderStr.empty() || fragmentShaderStr.empty()) {
        cerr << "Hiba a shader fájlok betöltésekor!" << endl;
        return;
    }

    const char* vertexShaderSource = vertexShaderStr.c_str();
    const char* fragmentShaderSource = fragmentShaderStr.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

  
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cerr << "Vertex Shader fordítási hiba: " << infoLog << endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cerr << "Fragment Shader fordítási hiba: " << infoLog << endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "Shader Program linkelési hiba: " << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
void setupCircle() {
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Shader Bouncing Circle", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();
    initOpenGL();
    setupShader();
    setupCircle();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapInterval(1);
        display();
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

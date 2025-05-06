#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

#define numVAOs 2
#define numVBOs 2
constexpr float SENSITIVITY = 0.03f;


GLchar windowTitle[] = "Beadando 2 - Fazekas Gréta";
GLuint windowWidth = 800;
GLuint windowHeight = 800;
GLFWwindow* window = nullptr;
//ÁTMÉRŐ
GLfloat d = 7.0f;

//PONT MOZGATÁSA
GLint dragged = -1;

//PONTOK RAJZOLÁSA
std::vector<glm::vec3> bezier;
std::vector<glm::vec3> myControlPoints;

GLuint VAO[numVAOs];
GLuint VBO[numVBOs];

GLuint renderingProgram;

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

GLuint createShaderProgram(const char* vertexShader, const char* fragmentShader) {
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string vertShaderStr = readShaderSource(vertexShader);
	string fragShaderStr = readShaderSource(fragmentShader);

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

/* Kiszámoljuk két pont távolságát. */
GLfloat dist2(glm::vec3 P1, glm::vec3 P2)
{
	GLfloat t1 = P1.x - P2.x;
	GLfloat t2 = P1.y - P2.y;

	return t1 * t1 + t2 * t2;
}

/* Megnézzük melyik az éppen aktuális pont amire kattintunk. */
GLint getActivePoint(const std::vector<glm::vec3>& points, int size, float sens, float x, float y) {
	float xNorm = x / (windowWidth / 2) - 1.0f;
	float yNorm = y / (windowHeight / 2) - 1.0f;
	glm::vec3 clickPos(xNorm, yNorm, 0.0f);
	float sens2 = sens * sens;

	for (int i = 0; i < size; i++) {
		if (dist2(points[i], clickPos) < sens2) {
			return i;
		}
	}
	return -1;
}

/* Binomiális együttható. */
int NCR(int n, int r) {
	if (r > n - r) r = n - r;  // Kihasználjuk a szimmetriát: C(n, r) = C(n, n-r)
	int result = 1;

	for (int i = 0; i < r; ++i) {
		result *= (n - i);
		result /= (i + 1);
	}

	return result;
}

/* Bezier görbe egyenlete. */
double blending(int i, float t, int n) {
	return NCR(n, i) * pow(1 - t, n - i) * pow(t, i);
}

/* A görbe pontok koordinátái */
void drawBezierCurve(const std::vector<glm::vec3>& controlPoints)
{
	bezier.clear();
	glm::vec3 nextPoint;
	GLfloat t = 0.0f;
	GLfloat increment = 1.0f / 100.0f;
	int n = controlPoints.size() - 1;

	while (t <= 1.0f)
	{
		nextPoint = glm::vec3(0.0f);
		for (int i = 0; i <= n; ++i)
		{
			nextPoint += (float)blending(i, t, n) * controlPoints[i];
		}

		bezier.push_back(nextPoint);

		t += increment;
	}
}

void draw() {
	if (myControlPoints.size() >= 3)
	{
		drawBezierCurve(myControlPoints);
	}
	else {
		bezier.clear();
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, bezier.size() * sizeof(glm::vec3), bezier.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, myControlPoints.size() * sizeof(glm::vec3), myControlPoints.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* Ha a drag drop technikával megfogjuk a pontok, és arrébb visszük, akkor újra kell számolni a görbe pontjait. */
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	GLint i;

	GLfloat xNorm = xPos / (windowWidth / 2) - 1.0f;
	GLfloat yNorm = (windowHeight - yPos) / (windowHeight / 2) - 1.0f;

	if (dragged >= 0)
	{
		myControlPoints.at(dragged).x = xNorm;
		myControlPoints.at(dragged).y = yNorm;

		draw();
	}
}

void cleanUpScene() {
	/** Töröljük a vertex array és a vertex buffer objektumokat. **/
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);
	/** Töröljük a shader programot. **/
	glDeleteProgram(renderingProgram);
	/** Töröljük a GLFW ablakot. **/
	glfwDestroyWindow(window);
	/** Leállítjuk a GLFW-t. **/
	glfwTerminate();
	/** Kilépés EXIT_SUCCESS kóddal. **/
	exit(EXIT_SUCCESS);
}

/* Bal kattintás: pont letétel, bal kattintás és mozgatás: pont mozgatás, jobb kattintás ponton: pont levétele */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GLint i;
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if ((i = getActivePoint(myControlPoints, myControlPoints.size(), SENSITIVITY, x, windowHeight - y)) != -1)
		{
			dragged = i;
		}
		else
		{
			myControlPoints.push_back(glm::vec3(x / (windowWidth / 2) - 1.0f, (windowWidth - y) / (windowHeight / 2) - 1.0f, 0.0f));
			draw();
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		int idx = getActivePoint(myControlPoints, myControlPoints.size(), SENSITIVITY, x, windowHeight - y);
		if (idx != -1) {
			// Töröljük a pontot
			myControlPoints.erase(myControlPoints.begin() + idx);
		}
		draw();
	}


	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		dragged = -1;
	}
}

// Billentyűk használata
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		// ESC-el bezárom az ablakot
		if (key == GLFW_KEY_ESCAPE)
		{
			cleanUpScene();
		}
		//NUMPAD +
		if (key == GLFW_KEY_KP_ADD && d < 9.0f) {
			d += 1.0f;
		}
		//NUMPAD -
		if (key == GLFW_KEY_KP_SUBTRACT && d > 3.0f) {
			d -= 1.0f;
		}

	}
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram("vertexShader.glsl", "fragmentShader.glsl");

	// Antialiasing bekapcsolása
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);


	glGenBuffers(2, VBO);
	glGenVertexArrays(2, VAO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, bezier.size() * sizeof(glm::vec3), bezier.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// === VAO[1]: Kontrollpontok és kontrollpoligon ===
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, myControlPoints.size() * sizeof(glm::vec3), myControlPoints.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);  // Itt hibás volt: nem kell "3 * sizeof(glm::vec3)"
	glEnableVertexAttribArray(0);

	// Tisztítás
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glClearColor(GLclampf(0.0f / 255.0f), GLclampf(0.0f / 255.0f), GLclampf(139.0f / 255.0f), 1.0);
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT);

	GLint drawModeLoc = glGetUniformLocation(renderingProgram, "drawMode");

	glUseProgram(renderingProgram);

	// Bezier
	glUniform1i(drawModeLoc, 0);
	glBindVertexArray(VAO[0]);
	glLineWidth(d);
	glDrawArrays(GL_LINE_STRIP, 0, bezier.size());

	// Kontrollpoligon
	glUniform1i(drawModeLoc, 1);
	glBindVertexArray(VAO[1]);
	glLineWidth(d);
	glDrawArrays(GL_LINE_STRIP, 0, myControlPoints.size());

	// Kontrollpontok
	glUniform1i(drawModeLoc, 2);
	glPointSize(d);
	glDrawArrays(GL_POINTS, 0, myControlPoints.size());
}

int main(void) {

	if (!glfwInit()) { exit(EXIT_FAILURE); }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

	glfwMakeContextCurrent(window);
	glViewport(0, 0, windowWidth, windowHeight);

	/** Billentyûzethez, kurzorhoz és egérhez köthetõ események kezelése. */
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);



	// Írkálás
	cout << "Beadando 02." << endl;
	cout << "Keszitette: Fazekas Greta" << endl;
	cout << "Gombok, amelyeket hasznalhatsz:" << endl;
	cout << "\t-Bal egergomb kattintas: egy pont lerakasa" << endl;
	cout << "\t-Bal egergomb lenyomva tartva a ponton: pont mashova mozgatasa" << endl;
	cout << "\t-Jobb egergomb kattintas egy ponton: egy pont levetele" << endl;
	cout << "\t-NUM PAD PLUSZ: pont nagyitasa" << endl;
	cout << "\t-NUM PAD MINUSZ: pont kicsinyitese" << endl;
	cout << "\t-ESC: kilepes a programbol" << endl;
	init(window);

	while (!glfwWindowShouldClose(window)) {

		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene();
}
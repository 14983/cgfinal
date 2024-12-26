#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "character.h"
#include "shader.h"
#include "utils.h"
#include "texture.h"
#include "bunny.h"
#include "common.h"
#include "screenshot.h"

//Window settings
unsigned int scr_width = 800;
unsigned int scr_height = 600;
float SCR_RATIO = (float)scr_width / (float)scr_height;
float deltaTime, lastTime, baseTime, currentTime;
bool isFullScreen = false;
int windowedWidth = 800;
int windowedHeight = 600;
int windowedX, windowedY;
GLFWmonitor* monitor;
const GLFWvidmode* video_mode;

//Camera settings
Camera camera(campos, bunnypos - campos);	//point to origin
Charactor bunny(bunnypos, -bunnypos);

unsigned int VBO, VAO, bunnyVAO, bunnyVBO, bunnyEBO, cubeVAO, cubeVBO;
unsigned int cubemapTexture;
glm::mat4 view, proj;

std::vector<float> vertices_bunny_UV;

void ExpandVertices();
unsigned int loadCubemap(std::vector<std::string> faces);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void InitVAO();
void updateTime();

void Draw(Shader& shader, Shader& cubeShader)
{
	shader.use();
	shader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
	shader.setVec3("lightColor", 1.0f, 0.98f, 0.8f);
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);
	shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", camera.Position);
	glm::mat4 model;

	//DrawTable
	Texture textable("res/texture/table.jpg");
	glBindVertexArray(VAO);
	for (int i = 0; i < 5; ++i) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, table_trans[i]);
		if (i == 0)
			model = glm::scale(model, glm::vec3(5.0f, 0.5f, 4.0f));
		else
			model = glm::scale(model, glm::vec3(0.6f, 3.0f, 0.6f));
		shader.setMat4("model", model);
		textable.Bind();
		shader.setInt("u_Texture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	textable.Unbind();

	//DrawCharacter
	Texture texbunny("res/texture/bunny.jpg");

	model = glm::mat4(1.0f);
	model = glm::translate(model, bunny.Position); // 将模型平移到 bunny 的位置

	// 计算 bunny 的旋转
	glm::vec3 direction = glm::normalize(bunny.Front);
	float yaw = glm::degrees(atan2(direction.z, direction.x)) - 90.0f;
	model = glm::rotate(model, glm::radians(-yaw + 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // 绕 Y 轴旋转 (Yaw)

	shader.setMat4("model", model);
	texbunny.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	texbunny.Unbind();

	//DrawCube
	glDepthFunc(GL_LEQUAL);
	cubeShader.use();
	cubeShader.setInt("skybox", 0);
	cubeShader.setMat4("view", glm::mat4(glm::mat3(view)));
	cubeShader.setMat4("projection", proj);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
}

int main()
{
	GLFWwindow* window = InitGL(3, 3, scr_width, scr_height, "CG Lab5_2");

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	monitor = glfwGetPrimaryMonitor();
	video_mode = glfwGetVideoMode(monitor);

	ExpandVertices();
	InitVAO();

	std::vector<std::string> faces
	{
		"res/texture/RT.jpg",
		"res/texture/LF.jpg",
		"res/texture/UP.jpg",
		"res/texture/DN.jpg",
		"res/texture/FR.jpg",
		"res/texture/BK.jpg"
	};
	cubemapTexture = loadCubemap(faces);

	Shader shader("res/shader/tex.vs", "res/shader/tex.fs");
	Shader cube_shader("res/shader/cube.vs", "res/shader/cube.fs");

	view = camera.GetViewMatrix();
	proj = glm::perspective(glm::radians(camera.Zoom), SCR_RATIO, 0.1f, 200.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwGetWindowPos(window, &windowedX, &windowedY);
		processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		updateTime();

		view = camera.GetViewMatrix();
		proj = glm::perspective(glm::radians(camera.Zoom), SCR_RATIO, 0.1f, 200.0f);

		camera.target(bunny);
		Draw(shader, cube_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void updateTime() {
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, width, height);
	SCR_RATIO = (float)width / (float)height;
	scr_height = height;
	scr_width = width;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		bunny.move(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		bunny.move(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		bunny.move(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		bunny.move(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		bunny.move(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		if (bunny.Position.y > ground)
			bunny.move(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        captureScreen(scr_width, scr_height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static float lastX = 400.0f;
	static float lastY = 300.0f;

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	bunny.ProcessMouseMovement(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;
	switch (key) {
	case GLFW_KEY_F:
		if (!isFullScreen) {
			glfwSetWindowMonitor(window, monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
			isFullScreen = true;
		}
		else {
			glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, GLFW_DONT_CARE);
			isFullScreen = false;
		}
		break;
	default:
		break;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.zoom(yoffset);
}

void InitVAO()
{
	//bind VAO, VBO, EBO for lighting
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	//pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));		//normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));		//texcoord
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//bind VAO, VBO, EBO for bunny
	glGenVertexArrays(1, &bunnyVAO);
	glGenBuffers(1, &bunnyVBO);
	glGenBuffers(1, &bunnyEBO);
	glBindVertexArray(bunnyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bunnyVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_bunny_UV.size() * sizeof(float), &vertices_bunny_UV[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	//pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));		//normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_bunny.size() * sizeof(int), &indices_bunny[0], GL_STATIC_DRAW);

	//bind VAO, VBO for cube
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), &cubeVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void ExpandVertices()
{
	for (size_t i = 0; i < vertices_bunny.size(); i += 6) {
		float x = vertices_bunny[i];
		float y = vertices_bunny[i + 1];
		float z = vertices_bunny[i + 2];

		float nx = vertices_bunny[i + 3];
		float ny = vertices_bunny[i + 4];
		float nz = vertices_bunny[i + 5];

		float u = 0.5f + atan2(z, x) / (2 * glm::pi<float>());
		float v = 0.5f - asin(y) / glm::pi<float>();
		vertices_bunny_UV.push_back(x);
		vertices_bunny_UV.push_back(y);
		vertices_bunny_UV.push_back(z);
		vertices_bunny_UV.push_back(nx);
		vertices_bunny_UV.push_back(ny);
		vertices_bunny_UV.push_back(nz);
		vertices_bunny_UV.push_back(u);
		vertices_bunny_UV.push_back(v);
	}
}
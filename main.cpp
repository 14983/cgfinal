#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "character.h"
#include "shader.h"
#include "utils.h"
#include "texture.h"
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

//camera settings
Camera player_camera(campos, playerpos - campos);	//point to origin
Camera free_camera(campos, playerpos - campos);	//point to origin
Charactor player(playerpos, -playerpos);

std::vector<glm::vec3> Platfrom_pos;

unsigned int VBO, VAO, cubeVAO, cubeVBO, sphereVAO, sphereVBO, sphereEBO;
unsigned int cubemapTexture;
glm::mat4 view, proj;

unsigned int loadCubemap(std::vector<std::string> faces);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void InitVAO();
void InitSphere();
void updateTime();

void Draw(Shader& shader, Shader& cubeShader, Shader& sphereShader)
{
	shader.use();

	shader.setVec3("lightColor", 1.0f, 0.98f, 0.8f);
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);
	shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", player_camera.Position);
	shader.setFloat("ambientStrength", 0.5f + 0.03f * player.Position.y);
	glm::mat4 model;

	//DrawPlatform
	glBindVertexArray(VAO);
	shader.setInt("u_Texture", 0);
	Texture ptex1 = Texture("res/texture/p1.jpg");
	ptex1.Bind();
	for (int layer = 0; layer < layer_num; layer++) {
		for (int i = 0; i < platform_edge * platform_edge; i++) {
			std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr) + i));
			std::uniform_real_distribution<float> dist(0.3f, 1.0f);
			glm::vec3 color(dist(rng), dist(rng), dist(rng));
			shader.setVec3("objectColor", color);
			model = glm::mat4(1.0f);
			model = glm::translate(model, platform_trans - (float)layer * layer_interval);
			int idx = i % platform_edge;
			int idy = i / platform_edge;
			model = glm::translate(model, glm::vec3(idx * (platform_scale + platform_interval) - platform_interval, 0.0f,
				idy * (platform_scale + platform_interval) - platform_interval));
			Platfrom_pos.push_back(model[3]);
			model = glm::scale(model, glm::vec3(platform_scale, 0.3f, platform_scale));
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	sphereShader.use();
	glBindVertexArray(sphereVAO);
	Texture ptex2 = Texture("res/texture/sphere.jpg");
	ptex2.Bind();
	for (int i = 0; i < layer_num; i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, Platfrom_pos[i * platform_edge * platform_edge + i % (platform_edge * platform_edge)]);
		model = glm::scale(model, glm::vec3(sphere_scale, sphere_scale, sphere_scale));
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 mvp = proj * view * model;
		sphereShader.setMat4("mvp", mvp);
		sphereShader.setInt("u_Texture", 0);
		glDrawElements(GL_TRIANGLES, sphere_indices.size(), GL_UNSIGNED_INT, 0);
	}

	shader.use();
	//DrawCharacter
	Texture texplayer("res/texture/player.jpg");

	model = glm::mat4(1.0f);
	model = glm::translate(model, player.Position);

	//Calculate rotation
	glm::vec3 direction = glm::normalize(player.Front);
	float yaw = glm::degrees(atan2(direction.z, direction.x)) - 90.0f;
	model = glm::rotate(model, glm::radians(-yaw + 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	shader.setMat4("model", model);
	texplayer.Bind();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	texplayer.Unbind();

	//DrawCube
	glDepthFunc(GL_LEQUAL);
	cubeShader.use();
	cubeShader.setInt("skybox", 0);
	cubeShader.setMat4("view", glm::mat4(glm::mat3(view)));
	cubeShader.setMat4("projection", proj);
	cubeShader.setFloat("ambient", 0.8f + 0.03f * player.Position.y);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
}

int main()
{
	GLFWwindow* window = InitGL(3, 3, scr_width, scr_height, "CG Project");

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

	InitSphere();
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
	Shader sphere_shader("res/shader/sphere.vs", "res/shader/sphere.fs");

	if (!freecam) {
		view = player_camera.GetViewMatrix();
		proj = glm::perspective(glm::radians(player_camera.Zoom), SCR_RATIO, 0.1f, 200.0f);
	}
	else {
		view = free_camera.GetViewMatrix();
		proj = glm::perspective(glm::radians(free_camera.Zoom), SCR_RATIO, 0.1f, 200.0f);
	}

	while (!glfwWindowShouldClose(window)) {
		glfwGetWindowPos(window, &windowedX, &windowedY);
		processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		updateTime();

		if (!freecam) {
			view = player_camera.GetViewMatrix();
			proj = glm::perspective(glm::radians(player_camera.Zoom), SCR_RATIO, 0.1f, 200.0f);
		}
		else {
			view = free_camera.GetViewMatrix();
			proj = glm::perspective(glm::radians(free_camera.Zoom), SCR_RATIO, 0.1f, 200.0f);
		}

		player_camera.target(player);
		Draw(shader, cube_shader, sphere_shader);

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
	if (!freecam) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			player.move(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			player.move(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			player.move(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			player.move(RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			player.move(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			//if (player.Position.y > ground)
			player.move(DOWN, deltaTime);
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			free_camera.move(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			free_camera.move(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			free_camera.move(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			free_camera.move(RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			free_camera.move(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			free_camera.move(DOWN, deltaTime);
	}
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
	if (!freecam) {
		player_camera.updatePitch(yoffset);
		player.ProcessMouseMovement(xoffset, yoffset);
	}
	else {
		free_camera.ProcessMouseMovement(xoffset, yoffset);
	}
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
	case GLFW_KEY_R:
	{
		if (!freecam)
			free_camera = std::move(player_camera);
		freecam = !freecam;
	}
	default:
		break;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	player_camera.zoom(yoffset);
}


void InitSphere() {
	for (int i = 0; i <= PREC; i++) {
		for (int j = 0; j <= PREC; j++) {
			float xSegment = (float)j / (float)PREC;
			float ySegment = (float)i / (float)PREC;
			// vertex coordinates
			sphere_vertices.push_back(std::cos(2.0f * glm::pi<float>() * xSegment) * std::sin(glm::pi<float>() * ySegment));
			sphere_vertices.push_back(std::cos(glm::pi<float>() * ySegment));
			sphere_vertices.push_back(std::sin(2.0f * glm::pi<float>() * xSegment) * std::sin(glm::pi<float>() * ySegment));
			// texture coordinates
			sphere_vertices.push_back(xSegment);
			sphere_vertices.push_back(ySegment);
		}
	}
	for (int i = 0; i < PREC; i++) {
		for (int j = 0; j < PREC; j++) {
			sphere_indices.push_back(i * (PREC + 1) + j);
			sphere_indices.push_back((i + 1) * (PREC + 1) + j);
			sphere_indices.push_back((i + 1) * (PREC + 1) + j + 1);

			sphere_indices.push_back(i * (PREC + 1) + j);
			sphere_indices.push_back((i + 1) * (PREC + 1) + j + 1);
			sphere_indices.push_back(i * (PREC + 1) + j + 1);
		}
	}

}

void InitVAO()
{
	//bind VAO, VBO for lighting
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

	//bind VAO, VBO for cube
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float), &cubeVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//bind VAO, VBO, EBO for sphere
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &sphereEBO);
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(float), &sphere_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // texture
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.size() * sizeof(unsigned int), &sphere_indices[0], GL_STATIC_DRAW);
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
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
  #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SIZE 1
GLuint CubemapTexture;

GLuint programColor;
GLuint programTexture;
GLuint programTextureProc;
GLuint programSkybox;

GLuint g_Texture;
GLuint moon_Texture;
GLuint sun_Texture;
GLuint mercury_Texture;
GLuint venus_Texture;
GLuint mars_Texture;
GLuint jupiter_Texture;


GLuint stars_bk_Texture;
GLuint stars_lf_Texture;
GLuint stars_fr_Texture;
GLuint stars_dn_Texture;
GLuint stars_up_Texture;
GLuint stars_rt_Texture;

Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;

float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-20, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;
glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));


float cubemapVertices[] = {
	// positions
	-SIZE,  SIZE, -SIZE,
	-SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE, -SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,

	-SIZE, -SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE, -SIZE,  SIZE,
	-SIZE, -SIZE,  SIZE,

	-SIZE,  SIZE, -SIZE,
	SIZE,  SIZE, -SIZE,
	SIZE,  SIZE,  SIZE,
	SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE,  SIZE,
	-SIZE,  SIZE, -SIZE,

	-SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE,  SIZE,
	SIZE, -SIZE,  SIZE
};

GLuint SkyboxVertexBuffer, SkyboxVertexAttributes;


glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}


//render skybox
void renderSkybox()
{
	glUseProgram(programSkybox);
	glm::mat4 view = glm::mat4(glm::mat3(cameraMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "view"), 1, GL_FALSE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "perspective"), 1, GL_FALSE, (float*)&perspectiveMatrix);

	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(SkyboxVertexAttributes);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	//glDepthMask(GL_TRUE);

	glUseProgram(0);

}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint texture)
{
	GLuint program = programTexture;

	glUseProgram(program);

	Core::SetActiveTexture(texture, "texture", program, 0);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectProceduralTexture(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color) {
	GLuint program = programTextureProc;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.4f;
	switch(key)
	{
		case 'z': cameraAngle -= angleSpeed; break;
		case 'x': cameraAngle += angleSpeed; break;
		case 'w': cameraPos += cameraDir * moveSpeed; break;
		case 's': cameraPos -= cameraDir * moveSpeed; break;
		case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
		case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
		case 'q': cameraPos += glm::cross(cameraDir, glm::vec3(0,0,1)) * moveSpeed; break;
		case 'e': cameraPos -= glm::cross(cameraDir, glm::vec3(0,0,1)) * moveSpeed; break;

	}

}


glm::mat4 createOrbit(float m_f,float sunDistance){
	glm::mat4 translate;
	glm::mat4 rotate;

	rotate[0][0] = cos(m_f);
	rotate[2][2] = cos(m_f);
	rotate[0][2] = sin(m_f);
	rotate[2][0] = -sin(m_f);

	translate[3][0] = 0;
	translate[3][1] = 0;
	translate[3][2] = -sunDistance;

	return rotate * translate;
}

void drawMercury(float m_f){
	glm::mat4 modelMatrix = glm::scale(glm::vec3(0.235f))  * createOrbit(m_f * 4.19,24);
	drawObjectTexture(&sphereModel, modelMatrix , mercury_Texture);
}

void drawVenus(float m_f){
	glm::mat4 modelMatrix =  glm::scale(glm::vec3(0.35f))  * createOrbit(m_f * 1.629,23);
	drawObjectTexture(&sphereModel, modelMatrix , venus_Texture);
}

void drawJupiter(float m_f){
	glm::mat4 modelMatrix =  glm::scale(glm::vec3(1.25f))  * createOrbit(m_f * 0.08,30);
	drawObjectTexture(&sphereModel, modelMatrix , jupiter_Texture);
}

void drawEarthAndMoon(float m_f){
	glm::mat4 modelMatrix = createOrbit(m_f,26);
	drawObjectTexture(&sphereModel, modelMatrix , g_Texture);

	glm::mat4 moon = modelMatrix *  glm::scale(glm::vec3(0.35f))  *  createOrbit(m_f * 2,4);
	drawObjectTexture(&sphereModel, moon , moon_Texture);
}

void drawMars(float m_f){

	glm::mat4 modelMatrix = createOrbit(m_f * 0.532,30);
	drawObjectTexture(&sphereModel, modelMatrix , mars_Texture);

	glm::mat4 moon = modelMatrix * glm::scale(glm::vec3(0.25f)) *  createOrbit(m_f,4);
	drawObjectTexture(&sphereModel, moon , moon_Texture);
}

void drawSun(float m_f){
	drawObjectTexture(&sphereModel, glm::scale(glm::vec3(3.25f)) *  createOrbit(m_f,0), sun_Texture);
}


void renderScene()
{

    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float m_f = time;

	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(0.3f, 0.5f, 0.3f, 1.0f);
	glClearColor(0.129f, 0.191f, 0.199f, 1.0f);

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.25f + glm::vec3(0.5,-0.25f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
	drawObjectColor(&shipModel,shipModelMatrix , glm::vec3(0.6f));

		drawSun(time);
		drawVenus(time);
		drawMercury(time);
		drawEarthAndMoon(time);
		drawMars(time);
		drawJupiter(time);
renderSkybox();


	//proceduralTex
	//drawObjectProceduralTexture(&sphereModel, glm::translate(glm::vec3(0,0,0)), glm::vec3(0.0,0.0,0.0));
	glutSwapBuffers();
}


//unsigned int LoadCubemap(std::vector<std::string> faces)
unsigned int LoadCubemap(std::vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, numComponents;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++)
    {
        image = stbi_load(faces[i], &width, &height, &numComponents, 3);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}


unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
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
void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programTextureProc = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/skybox.vert", "shaders/skybox.frag");

	g_Texture = Core::LoadTexture("textures/earth.png");
	moon_Texture = Core::LoadTexture("textures/moonmap.png");
	sun_Texture = Core::LoadTexture("textures/sunmap.png");
	mercury_Texture = Core::LoadTexture("textures/mercurymap.png");
	venus_Texture = Core::LoadTexture("textures/venusmap.png");
	mars_Texture = Core::LoadTexture("textures/marsmap.png");
	jupiter_Texture = Core::LoadTexture("textures/jupitermap.png");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");

	stars_bk_Texture = Core::LoadTexture("textures/stars_bk.png");
	stars_lf_Texture = Core::LoadTexture("textures/stars_lf.png");
	stars_fr_Texture = Core::LoadTexture("textures/stars_rf.png");
	stars_dn_Texture = Core::LoadTexture("textures/stars_dn.png");
	stars_up_Texture = Core::LoadTexture("textures/stars_up.png");
	stars_rt_Texture = Core::LoadTexture("textures/stars_rt.png");


	//load Cubemap texture
	std::vector<std::string> faces ;
	faces.push_back("textures/skybox/stars_fr.jpg");
	faces.push_back("textures/skybox/stars_lf.jpg");
	faces.push_back("textures/skybox/stars_rt.jpg");
	faces.push_back("textures/skybox/stars_up.jpg");
	faces.push_back("textures/skybox/stars_dn.jpg");
	faces.push_back("textures/skybox/stars_bk.jpg");

	CubemapTexture = loadCubemap(faces);

	//Skybox settings
	glGenBuffers(1, &SkyboxVertexBuffer);
	glGenVertexArrays(1, &SkyboxVertexAttributes);
	glBindVertexArray(SkyboxVertexAttributes);
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), cubemapVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Symulacja lotu kosmicznego");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}

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



GLuint programColor;
GLuint programTexture;
GLuint programTextureProc;
GLuint  programSkybox;

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

glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
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



	glPushMatrix();

	    // Reset and transform the matrix.
	    glLoadIdentity();

	    // Enable/Disable features
	    glPushAttrib(GL_ENABLE_BIT);
	    glEnable(GL_TEXTURE_2D);
	    glDisable(GL_DEPTH_TEST);
	    glDisable(GL_LIGHTING);
	    glDisable(GL_BLEND);

	    // Just in case we set all vertices t white.
	    glColor4f(1,1,1,1);

	    // Render the front quad
	    glBindTexture(GL_TEXTURE_2D, sun_Texture);
	    glBegin(GL_QUADS);
	        glTexCoord2f(0, 0); glVertex3f(  1, -1, -1 );
	        glTexCoord2f(1, 0); glVertex3f( -1, -1, -1 );
	        glTexCoord2f(1, 1); glVertex3f( -1,  1, -1 );
	        glTexCoord2f(0, 1); glVertex3f(  1,  1, -1 );
	    glEnd();

	    // Render the left quad
	    glBindTexture(GL_TEXTURE_2D, stars_lf_Texture);
	    glBegin(GL_QUADS);
	        glTexCoord2f(0, 0); glVertex3f(  1, -1,  1 );
	        glTexCoord2f(1, 0); glVertex3f(  1, -1, -1 );
	        glTexCoord2f(1, 1); glVertex3f(  1,  1, -1 );
	        glTexCoord2f(0, 1); glVertex3f(  1,  1,  1 );
	    glEnd();

	    // Render the back quad
	    glBindTexture(GL_TEXTURE_2D, stars_bk_Texture);
	    glBegin(GL_QUADS);
	        glTexCoord2f(0, 0); glVertex3f( -1, -1,  1 );
	        glTexCoord2f(1, 0); glVertex3f(  1, -1,  1 );
	        glTexCoord2f(1, 1); glVertex3f(  1,  1,  1 );
	        glTexCoord2f(0, 1); glVertex3f( -1,  1,  1 );

	    glEnd();

	    // Render the right quad
	    glBindTexture(GL_TEXTURE_2D, stars_rt_Texture);
	    glBegin(GL_QUADS);
	        glTexCoord2f(0, 0); glVertex3f( -1, -1, -1 );
	        glTexCoord2f(1, 0); glVertex3f( -1, -1,  1 );
	        glTexCoord2f(1, 1); glVertex3f( -1,  1,  1 );
	        glTexCoord2f(0, 1); glVertex3f( -1,  1, -1 );
	    glEnd();

	    // Render the top quad
	    glBindTexture(GL_TEXTURE_2D, stars_up_Texture);
	    glBegin(GL_QUADS);
	        glTexCoord2f(0, 1); glVertex3f( -1,  1, -1 );
	        glTexCoord2f(0, 0); glVertex3f( -1,  1,  1 );
	        glTexCoord2f(1, 0); glVertex3f(  1,  1,  1 );
	        glTexCoord2f(1, 1); glVertex3f(  1,  1, -1 );
	    glEnd();

	    // Render the bottom quad
	    glBindTexture(GL_TEXTURE_2D, stars_dn_Texture);
	    glBegin(GL_QUADS);
	        glTexCoord2f(0, 0); glVertex3f( -1, -1, -1 );
	        glTexCoord2f(0, 1); glVertex3f( -1, -1,  1 );
	        glTexCoord2f(1, 1); glVertex3f(  1, -1,  1 );
	        glTexCoord2f(1, 0); glVertex3f(  1, -1, -1 );
	    glEnd();

	    // Restore enable bits and matrix
	    glPopAttrib();
	    glPopMatrix();

		drawSun(time);
		drawVenus(time);
		drawMercury(time);
		drawEarthAndMoon(time);
		drawMars(time);
		drawJupiter(time);
		glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.25f + glm::vec3(0.5,-0.25f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
		drawObjectColor(&shipModel,shipModelMatrix , glm::vec3(0.6f));


	//proceduralTex
	//drawObjectProceduralTexture(&sphereModel, glm::translate(glm::vec3(0,0,0)), glm::vec3(0.0,0.0,0.0));
	glutSwapBuffers();
}


void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programTextureProc = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

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

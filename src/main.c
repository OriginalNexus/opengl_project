#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../include/utility.h"
#include "../include/lobjder.h"
#include "../include/motion.h"
#include "../include/bass.h"

// C does not support boolean
#define true 1
#define false 0

// Ligth parameters
GLfloat lightPos[] = {10, 4.5, 10, 1};
GLfloat lightAmbient[] = {0.5, 0.5, 0.5, 1};
GLfloat lightDiffuse[] = {0.8, 0.8, 0.8, 1};
GLfloat lightSpecular[] = {0.4, 0.4, 0.4, 1};

GLdouble a1 = 0, a2 = 0;

GLfloat fps = 0;
int fpsCurrentTime = 0, fpsPreviousTime = 0, fpsFrameCount = 0;

// Terrain textures
lbj_Arraym terrainMats;

// Models
lbj_Model planeModel, nokiaModel, cubeModel, carModel, nexusModel, iphoneModel;


void drawWallsAndFloor(void)
{
	GLdouble i, j;
	// The walls and floor will be a (20 / nr) * (20 / nr) grid
	GLdouble nr = 0.4;

	// Enables textures
	glEnable(GL_TEXTURE_2D);

	// Draws floor
	lbj_LoadMaterial(terrainMats.array[0]);
	glBegin(GL_QUADS);
		for (i = 0; i < 19.99; i += nr) {
			for (j = 0; j < 19.99; j += nr) {
				glNormal3f(0, 1, 0);
				glTexCoord2f(0.5 * i, 0.5 * j);
				glVertex3f(i, 0, j);
				glTexCoord2f(0.5 * i, 0.5 * (j + nr));
				glVertex3f(i, 0, j + nr);
				glTexCoord2f(0.5 * (i + nr), 0.5 * (j + nr));
				glVertex3f(i + nr, 0, j + nr);
				glTexCoord2f(0.5 * (i + nr), 0.5 * j);
				glVertex3f(i + nr, 0, j);
			}
		}
	glEnd();

	// Draws walls
	lbj_LoadMaterial(terrainMats.array[1]);
	glBegin(GL_QUADS);
		for (i = 0; i < 19.99; i += nr) {
			for (j = 0; j < 19.99; j += nr) {
				// Wall 1
				glNormal3f(0, 0, 1);
				glTexCoord2f(0.5 * i, 0.125 * j);
				glVertex3f(i, 0.25 * j, 0);
				glTexCoord2f(0.5 * i, 0.125 * (j + nr));
				glVertex3f(i, 0.25 * (j + nr), 0);
				glTexCoord2f(0.5 * (i + nr), 0.125 * (j + nr));
				glVertex3f(i + nr, 0.25 * (j + nr), 0);
				glTexCoord2f(0.5 * (i + nr), 0.125 * j);
				glVertex3f(i + nr, 0.25 * j, 0);

				// Wall 2
				glNormal3f(0, 0, -1);
				glTexCoord2f(0.5 * i, 0.125 * j);
				glVertex3f(i, 0.25 * j, 20);
				glTexCoord2f(0.5 * i, 0.125 * (j + nr));
				glVertex3f(i, 0.25 * (j + nr), 20);
				glTexCoord2f(0.5 * (i + nr), 0.125 * (j + nr));
				glVertex3f(i + nr, 0.25 * (j + nr), 20);
				glTexCoord2f(0.5 * (i + nr), 0.125 * j);
				glVertex3f(i + nr, 0.25 * j, 20);

				// Wall 3
				glNormal3f(1, 0, 0);
				glTexCoord2f(0.5 * i, 0.125 * j);
				glVertex3f(0, 0.25 * j, i);
				glTexCoord2f(0.5 * i, 0.125 * (j + nr));
				glVertex3f(0, 0.25 * (j + nr), i);
				glTexCoord2f(0.5 * (i + nr), 0.125 * (j + nr));
				glVertex3f(0, 0.25 * (j + nr), i + nr);
				glTexCoord2f(0.5 * (i + nr), 0.125 * j);
				glVertex3f(0, 0.25 * j, i + nr);

				// Wall 4
				glNormal3f(-1, 0, 0);
				glTexCoord2f(0.5 * i, 0.125 * j);
				glVertex3f(20, 0.25 * j, i);
				glTexCoord2f(0.5 * i, 0.125 * (j + nr));
				glVertex3f(20, 0.25 * (j + nr), i);
				glTexCoord2f(0.5 * (i + nr), 0.125 * (j + nr));
				glVertex3f(20, 0.25 * (j + nr), i + nr);
				glTexCoord2f(0.5 * (i + nr), 0.125 * j);
				glVertex3f(20, 0.25 * j, i + nr);
			}
		}
	glEnd();

	// Draws ceiling
	lbj_LoadMaterial(terrainMats.array[1]);
	glBegin(GL_QUADS);
		for (i = 0; i < 19.99; i += nr) {
			for (j = 0; j < 19.99; j += nr) {
				glNormal3f(0, -1, 0);
				glTexCoord2f(i, j);
				glVertex3f(i, 5, j);
				glTexCoord2f(i, j + nr);
				glVertex3f(i, 5, j + nr);
				glTexCoord2f(i + nr, j + nr);
				glVertex3f(i + nr, 5, j + nr);
				glTexCoord2f(i + nr, j);
				glVertex3f(i + nr, 5, j);
			}
		}
	glEnd();

	// Disables textures
	glDisable(GL_TEXTURE_2D);
}

// This will draw a 100 by 100 plane, the camera being always in the middle of it. Gives the impression of an infinite world
void drawGround() {
	int centerX = (int) motGetEyePos().x, centerY = (int) motGetEyePos().z;
	int i, j;
	glEnable(GL_TEXTURE_2D);
	lbj_LoadMaterial(terrainMats.array[2]);
	glPushMatrix();
		glTranslatef(0, -0.01, 0);
		glBegin(GL_QUADS);
			glNormal3f(0, 1, 0);
			for (i = 0; i < 100; i++) {
				for (j = 0; j < 100; j++) {
					glTexCoord2f(1, 0);
					glVertex3f(centerX + i - 50, 0, centerY + j - 50);
					glTexCoord2f(1, 1);
					glVertex3f(centerX + i - 50 + 1, 0, centerY + j - 50);
					glTexCoord2f(0, 1);
					glVertex3f(centerX + i - 50 + 1, 0, centerY + j - 50 + 1);
					glTexCoord2f(0, 0);
					glVertex3f(centerX + i - 50, 0, centerY + j - 50 + 1);
				}
			}
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void display(void)
{
	// Clear the color buffer, restore the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Load the identity matrix, clear all the previous transformations
	glLoadIdentity();
	// Set up the camera
	motMoveCamera();
	// Set light position
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Loads the default material
	lbj_LoadDefaultMaterial();

	// Draws and rotates a teapot
	glPushMatrix();
		glTranslatef(3, 0.6, 15);
		glRotatef(a2, 0, 1, 0);
		glutSolidTeapot(1);
	glPopMatrix();

	// Draws and animates a cube
	glPushMatrix();
		GLdouble k = (a2 - ((int) a2 / 90) * 90) * 2 * DEG_TO_RAD;
		glTranslatef(5 + 4 * sin(a1 * DEG_TO_RAD), 0.5 + sin(k) * (sqrt(2) / 2 - 0.5), 5 + 4 * cos(a1 * DEG_TO_RAD));
		glRotatef(a2, -sin(a1 * DEG_TO_RAD), 0, -cos(a1 * DEG_TO_RAD));
		glRotatef(a1, 0, 1, 0);
		glutSolidCube(1);
	glPopMatrix();

	// Draws the room
	drawWallsAndFloor();

	// Draws ground
	drawGround();

	// Draws Plane on ground
	glPushMatrix();
		glTranslatef(17, 1, 3);
		glRotatef(a1, 0, 1, 0);
		glScalef(0.4, 0.4, 0.4);
		lbj_DrawModelVBO(planeModel);
	glPopMatrix();

	// Draws Plane that flies
	glPushMatrix();
		glTranslatef(10 + 7 * sin(a1 * DEG_TO_RAD), 4 + 0.5 * sin(a1 * DEG_TO_RAD), 10 + 7 * cos(a1 * DEG_TO_RAD));
		glRotatef(a1 - 90, 0, 1, 0);
		glScalef(0.4, 0.4, 0.4);
		lbj_DrawModelVBO(planeModel);
	glPopMatrix();

	// Draws car
	glPushMatrix();
		glTranslatef(3, 0, 7);
		glRotatef(a1, 0 , 1, 0);
		glRotatef(-90, 1, 0, 0);
		glScalef(0.03, 0.03, 0.03);
		lbj_DrawModelVBO(carModel);
	glPopMatrix();

	// Draws Nexus
	glPushMatrix();
		glTranslatef(17, 1.5, 10);
		glRotatef(-90, 0, 1, 0);
		glScalef(0.01, 0.01, 0.01);
		lbj_DrawModelVBO(nexusModel);
	glPopMatrix();

	// Draws Nokia
	glPushMatrix();
		glTranslatef(3, 1.5, 3);
		glRotatef(a1, 0, 1, 0);
		glScalef(0.005, 0.005, 0.005);
		glRotatef(90, 1, 0, 0);
		lbj_DrawModelVBO(nokiaModel);
	glPopMatrix();

	// Draws Cube
	glPushMatrix();
		glTranslatef(7, 0.5, 3);
		glRotatef(a1, 0, 1, 0);
		glScalef(0.5, 0.5, 0.5);
		lbj_DrawModelVBO(cubeModel);
	glPopMatrix();

	// Draws IPhone
	glPushMatrix();
		glTranslatef(5, 1.5, 3);
		glScalef(0.1, 0.1, 0.1);
		lbj_DrawModelVBO(iphoneModel);
	glPopMatrix();

	// Swap buffers in GPU
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	// Set the viewport to the full window size
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	// Load the projection matrix
	glMatrixMode(GL_PROJECTION);
	// Clear all the transformations on the projection matrix
	glLoadIdentity();
	// Set the perspective according to the window size
	gluPerspective(70, (GLdouble) width / (GLdouble) height, 0.1, 60000);
	// Load back the modelview matrix
	glMatrixMode(GL_MODELVIEW);
}

void initialize(void)
{
	// Set the background to light gray
	glClearColor(0.8, 0.8, 0.8, 1);
	// Enables depth test
	glEnable(GL_DEPTH_TEST);
	// Disable color material
	glDisable(GL_COLOR_MATERIAL);
	// Enable normalize 
	glEnable(GL_NORMALIZE);
	// Enable Blend and transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Sets the light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	// Enables lighting
	glEnable(GL_LIGHTING);
	// Enables the light
	glEnable(GL_LIGHT0);
	// Loads the default material
	lbj_LoadDefaultMaterial();

	glShadeModel(GL_SMOOTH);

	// Loads terrain materials
	lbj_SetPaths("./data/models/", "./data/materials/", "./data/textures/");
	lbj_SetFlipping(0, 1, 0, 0, 0);

	lbj_LoadMTLToMaterials("terrain.mtl", &terrainMats, 1);

	// Load models
	lbj_SetPaths("./data/models/", "./data/models/materials/", "./data/models/textures/");

	// Plane
	lbj_LoadOBJToModel("SimplePlane.obj", &planeModel);
	lbj_CreateVBO(&planeModel, 1);

	// Cube
	lbj_LoadOBJToModel("cube2.obj", &cubeModel);
	lbj_CreateVBO(&cubeModel, 1);

	// Nexus
	lbj_LoadOBJToModel("Nexus.obj", &nexusModel);
	lbj_CreateVBO(&nexusModel, 1);

	// Nokia
	lbj_LoadOBJToModel("sonyericsson-w9600-midres.obj", &nokiaModel);
	lbj_CreateVBO(&nokiaModel, 0);

	// Car
	lbj_LoadOBJToModel("alfa147.obj", &carModel);
	lbj_CreateVBO(&carModel, 0);

	// IPhone 4S
	lbj_LoadOBJToModel("iphone_4s_home_screen.obj", &iphoneModel);
	lbj_CreateVBO(&iphoneModel, 1);

	// Loads the shaders
	loadShaders("./src/vshader.vsh", GL_VERTEX_SHADER, "./src/fshader.fsh", GL_FRAGMENT_SHADER);
}

void tick(int value)
{
	a1+= 5 * 0.1;
	if (a1 >= 360)
		a1 -= 360;

	a2 += 5 * 0.62831;
	if (a2 >= 360)
		a2 -= 360;
	
	// Calls the display() function
	glutPostRedisplay();

	// Waits 10 ms
	glutTimerFunc(10, tick, value + 1);

	// FPS calculus

	// Increase frame count
	fpsFrameCount++;
	// Get the number of milliseconds since glutInit called (or first call to glutGet(GLUT ELAPSED TIME))
	fpsCurrentTime = glutGet(GLUT_ELAPSED_TIME);
	// Calculate time passed
	int timeInterval = fpsCurrentTime - fpsPreviousTime;

	if (timeInterval > 1000)
	{
		// Calculate the number of frames per second
		fps = fpsFrameCount / (float) timeInterval * 1000;
		// Set time
		fpsPreviousTime = fpsCurrentTime;
		// Reset frame count
		fpsFrameCount = 0;
		// Change the window's title
		char title[30];
		sprintf(title, "Epic Game | FPS: %f", fps);
		glutSetWindowTitle(title);
	}
}

// Called when the song ends
void replayMusic(HSYNC handle, DWORD channel, DWORD data, void *user) {
	// Replays the song
	BASS_ChannelPlay(channel, TRUE);
}

void bass(void) {
	// Initializes the library
	BASS_Init(-1, 44100, 0, 0, NULL);
	// Creates a stream from a file (the stream is some sort of ID)
	int stream = BASS_StreamCreateFile(FALSE, "data/sound/Tritonal  Paris Blohm ft Sterling Fox - Colors Culture Code Remix.mp3", 0, 0, 0);
	// Adds an "Event Listener" (it is called sync) to detect when the song ends
	BASS_ChannelSetSync(stream, BASS_SYNC_MIXTIME | BASS_SYNC_END, 0, replayMusic, 0);
	// Plays the stream
	BASS_ChannelPlay(stream, TRUE);
}


int main(int argc, char *argv[])
{
	printf("===================================================\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Epic Game");
	glewInit();
	motionInit();

	// Event listeners
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	initialize();

	bass();

	// Starts main timer
	glutTimerFunc(10, tick, 0);

	glutMainLoop();
	return 0;
}

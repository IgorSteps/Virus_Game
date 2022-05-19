/******************************************************************************************
Project:	Virus Game
Date:		15 February 2022
Name:		Igor Stepanenko
Details:	User controls a 2D object around a game environment which is restricted by
			boundaries. The view scrolls both horizontally and vertically. Cells move
			collide with each other, creating and destroying objects dynamically.
******************************************************************************************/

//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\freeglut.h"
#pragma comment(lib, "glew32.lib")
//------- GLM -------
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
//------ Game Objects -------
#include "shaders\Shader.h"
#include "Sprite.h"
#include "GameObject.h"
#include "Virus.h"
#include "Enemy.h"
// ----- Other -----
#include <algorithm>
#include <iostream>
#include <random>
#include <ctime>
#include "FreeType.h";
using namespace std;

// Coordinate System Data
glm::mat4 ViewMatrix;		// matrix for the modelling and viewing
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::vec2 Direction;
glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

// Game Data
Shader myShader;
Sprite Background;
Sprite PetriDish;
std::shared_ptr<Sprite> SpriteRenderer;
std::shared_ptr<Virus> VirusObj;
std::vector<Enemy*> enemies;

// ----------------- GLOABAL VARIABLES ----------------------
// Start/End Screens
bool start = false;
bool endScreen = false;

// Font
freetype::Font our_font;   

// Variables for timing
float dt = 0.0f;
float currentTicks = 0.0f;
float PreviousTicks = 0.0f;

// window dimensions 
int screenWidth = 800, screenHeight = 800;
// mouse position
int	mouse_x = 0, mouse_y = 0;

//----------------- OPENGL FUNCTION PROTOTYPES -----------------
void display();				          //used as callback in glut for display.
void reshape(int width, int height);  //used as callback for reshape function in glut
void init();				          //called in main when the program starts.
//----------------- OTHER FUNCTION PROTOTYPES -----------------
void keyfunction(unsigned char key, int x, int y); //key board function
void mouseMotion(int x, int y);
void resolveCollisionVirus(Virus& one, GameObject& two);
void resolveCollisionEnemy(Enemy& one, Enemy& two);
float random(float range_from, float range_to);

/*************    START OF OPENGL FUNCTIONS   ****************/

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth = width;
	screenHeight = height;

	glViewport(0, 0, width, height);

	ProjectionMatrix = glm::ortho(
		(static_cast<float>(-width) / 2.0f),
		(static_cast<float>(width) / 2.0f),
		static_cast<float>(-height) / 2.0f,
		static_cast<float>(height) / 2.0f,
		1.0f,
		-1.0f);
}


void display()									
{
	/// ************************ DELTA TIME ************************
	//obtain the ticks from the clock and find difference with previous time.
	currentTicks = std::clock();
	float deltaTicks = (float)(currentTicks - PreviousTicks);
	PreviousTicks = currentTicks;
	//now find the actual time passed between frames
	dt = deltaTicks / (float)CLOCKS_PER_SEC;
	/// ************************ END DELTA TIME ************************

	//clear the colour buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// set view for virus position
	ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-VirusObj->Position.x, - VirusObj->Position.y, 0.0f));

	/// ************************ RENDER SPRITE OBJECTS ************************
	glEnable(GL_BLEND); // enable blend

	// background
	ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, 0.0f));
	Background.Render(myShader, ViewMatrix, ProjectionMatrix);
	
	// petri dish
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	PetriDish.Render(myShader, ModelViewMatrix, ProjectionMatrix);
	
	// virus
	VirusObj->Draw(myShader, ViewMatrix, ProjectionMatrix, Scale);
	if (start) {
		VirusObj->Move(Direction, dt);
	}

	// enemy
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	for (auto& en : enemies) {
		
		en->Draw(myShader, ViewMatrix, ProjectionMatrix, en->Scale);
		if (start) {
			en->Move(dt);
		}
		
		resolveCollisionVirus(*VirusObj, *en);

		for (auto& an : enemies) {
			if (an != en) {
				resolveCollisionEnemy(*an, *en);
			}
		}
		

	}
	
	// Delete enemy if it's dead
	enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy* e) {return e->Dead; }), enemies.end());

	/// ************************ STOP RENDER SPRITE OBJECTS ************************

	// screen menus
	if (!start) {
		freetype::print(ProjectionMatrix, our_font, (static_cast<float>(-screenWidth) /6.0f),
			(static_cast<float>(screenHeight) / 16.0f),
			"VIRUS GAME BY IGOR STEPANENKO\nMOVE YOUR CURSOR TO MOVE\nTO PLAY,PRESS S\nTO EXIT, PRESS ESC");
	}
	if (VirusObj->Dead) {
		endScreen = true;
		VirusObj->Speed = 0.0f;
		freetype::print(ProjectionMatrix, our_font, (static_cast<float>(-screenWidth) / 14.0f),
			(static_cast<float>(screenHeight) / 14.0f), "YOU LOST!\nPRESS ESC TO EXIT");
	}
	if (enemies.size() == 0)
		freetype::print(ProjectionMatrix, our_font, (static_cast<float>(-screenWidth) / 14.0f),
			(static_cast<float>(screenHeight) / 14.0f), "YOU WON!\nPRESS ESC TO EXIT");


	glDisable(GL_BLEND); // disable blend
	glutSwapBuffers();

}


void init()
{
	//Load the GLSL program 
	if (!myShader.load("Basic", "./glslfiles/basicTexture.vert", "./glslfiles/basicTexture.frag"))
	{
		std::cout << "failed to load shader" << std::endl;
	}
	//sets the clear colour to black
	glClearColor(0.0, 0.0, 1.0, 0.0);
	/// ************************ Initialise sprites ************************
	float red[3]	= {1.0f, 0.0f, 0.0f};
	float green[3]	= {0.0f, 1.0f, 0.0f};
	float redish[3] = { 3.0f, 14.0f, 18.0f };

	// background
	Background.SetWidth(2000.0f);
	Background.SetHeight(2000.0f);
	Background.Init(myShader, red, "textures/background.png", 3.0f, 3.0f);

	// petri-dish
	PetriDish.SetWidth(800.0f);
	PetriDish.SetHeight(800.0f);
	PetriDish.Init(myShader, red, "textures/petridish.png", 1.0f, 1.0f);

	// seed rand for random point in enemy/radii
	srand(time(NULL));


	// virus
	VirusObj = std::unique_ptr<Virus>(new Virus(glm::vec2(0.0f), glm::vec2(0.0f, 0.0f), 20.0f));
	VirusObj->Init(myShader, red, "textures/Red_Virus.png", 1.0f, 1.0f);
	
	// enemy
	for (int i = 0; i < 10; ++i) {
		enemies.push_back(new Enemy(glm::vec2(0.0f), glm::vec2(0.0f), random(15.0f, 30.0f), enemies));
		enemies.back()->Init(myShader, red, "textures/Green_Virus.png", 1.0f, 1.0f);
	}

	// font
	our_font.init("arialbd.ttf", 22);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}


void idle()
{
	glutPostRedisplay();
}

/**************** END OPENGL FUNCTIONS *************************/

void keyfunction(unsigned char key, int x, int y)
{
	switch (key )
	{
	case 27: //esc
		glutLeaveMainLoop();
	case 115: //s
		start = true;
	
	}
}



void mouseMotion(int x, int y)
{
	float angle;
	glm::vec2 center = glm::vec2(screenWidth / 2.0f, screenHeight / 2.0f);

	angle = atan2(y - center.y, x - center.x);

	glm::vec4 ForwardVec = glm::vec4(1.0f,0.0f,0.0f,1.0f);
	auto mat = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0.0f, 0.0f, 1.0f));

	Direction = mat * ForwardVec;
}

void resolveCollisionVirus(Virus& one, GameObject& two) {

	if (VirusObj->checkCollision(one, two)) {	
		// if virus is bigger then enemy
		if (two.Radius <= one.Radius) {
			// scale on collision
			Scale += 0.1f;
			
			// change radius of virus
			VirusObj->Radius += Scale.x;
			
			// set the enemy state to Dead = true
			two.Dead = true;
		} 
		else {
			one.Dead = true;
			}


	}
}

void resolveCollisionEnemy(Enemy& one, Enemy& two) {
	if (VirusObj->checkCollision(one, two)) {
	
		if (one.Radius > two.Radius) {
			one.Scale += 0.1f;
			one.Radius += one.Scale.x;
			two.Dead = true;
		}
		else {
			two.Scale += 0.1f;
			two.Radius += two.Scale.x;
			one.Dead = true;
		}
	}
}

float random(float range_from, float range_to) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(range_from, range_to);
	return distr(gen);
}


// FREEGLUT WINDOW SET UP
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(1, 1);
	glutCreateWindow("Virus Game");
	
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(mouseMotion);
	glutKeyboardFunc(keyfunction);

	glutIdleFunc(idle);

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	/// Uncomment to check the OpenGL version being used
	/*int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;*/

	//initialise the objects for rendering
	init();

	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}


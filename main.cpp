// OpenGLFun.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h> 
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Sprite.h"
#include "Input.h"
#include <stdio.h>
#include <time.h>
#include "fmod.hpp"

#define MAX_LOADSTRING 100

enum GameState
{
	GS_Splash,
	GS_Loading,
	GS_Menu,
	GS_Credits,
	GS_Running,
	GS_NextLevel,
	GS_Paused,
	GS_GameOver,
	GS_Level2,
	GS_Highscore,
};

int lvl = 0;

float ballSpeed = 50;
float platform2Pos = 50;
float platformWidth = 150;
float platformHeight = 10;

int previousTime;
int refreshMills = 15; // refresh interval in milliseconds

float p_deltaTime = 0.0;

int rows = 4;
int columns = 10;




GLfloat screen_height;
GLfloat screen_width;

//Game variables
//audio
FMOD::System* audiomgr;
FMOD::Sound* sfxButton;
FMOD::Sound* musBackground;
FMOD::Sound* sfxHitPaddle;
FMOD::Sound* sfxHitBrick;
FMOD::Sound* sfxDeath;
//game objects
Sprite* player;
Sprite* platform;
Sprite* platform2;
Sprite* BlueBrick[100][100];
Sprite* RedBrick[100][100];

//new
char* c;
//special variables 
Sprite* widthBrick;

bool widthPowerUp = false;
bool xtraBall = false;

Sprite* lifeBrick;

Sprite* extraBallBrick;

Sprite* extraBall;

Sprite* highscoreButton;

std::string highscore;

//UI elements
Sprite* pauseButton;
Sprite* resumeButton;
Sprite* splashScreen;
Sprite* menuScreen;
Sprite* creditsScreen;
Sprite* nextLevelScreen;
Sprite* continueButton;
Sprite* gameOverScreen; 
Sprite* replayButton;
Sprite* playButton;
Sprite* creditsButton;
Sprite* exitButton;
Sprite* menuButton;

Input* inputManager;

GameState m_gameState;

float uiTimer;
const float UI_THRESHOLD = 0.01f;

float pickupSpawnThreshold;
float pickupSpawnTimer;

float enemySpawnThreshold;
float enemySpawnTimer;

float splashDisplayTimer;
float splashDisplayThreshold;

int scr;
int lives;

const float velocityX = 50;
const float velocityY = -50;



bool InitFmod()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&audiomgr);
	if (result != FMOD_OK)
	{
		return false;
	}
	result = audiomgr->init(50, FMOD_INIT_NORMAL, NULL);
	if (result != FMOD_OK)
	{
		return false;
	}
	return true;
}

const bool LoadAudio()
{
	FMOD_RESULT result; 
	result = audiomgr->createSound("resources/paddleBounce.wav", FMOD_DEFAULT, 0, &sfxHitPaddle);
	result = audiomgr->createSound("resources/brickBounce.wav", FMOD_DEFAULT, 0, &sfxHitBrick);
	result = audiomgr->createSound("resources/death.wav", FMOD_DEFAULT, 0, &sfxDeath);

	result = audiomgr->createSound("resources/button.wav", FMOD_DEFAULT, 0, &sfxButton);

	result = audiomgr->createSound("resources/background.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &musBackground);
	FMOD::Channel* channel;
	result = audiomgr->playSound(musBackground, 0, false, &channel);
	return true;
}

void DrawStrokeText(char* string, int x, int y, const float r, const float g, const float b) // Stroke Font
{
	char* c;

	glColor3f(r, g, b);
	glPushMatrix();
	glTranslatef(x, y + 8, 0.0);
	glLineWidth(2.0);
	glScalef(0.14f, -0.12f, 0.0);

	for (c = string; *c != '\0'; c++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	}
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
}

void DrawText(char* string, float x, float y, const float r, const float g, const float b)  // BItmap Font
{
	glColor3f(r, g, b);
	glLineWidth(4.0);
	char* c;
	glRasterPos3f(x, y, 0.0);
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
}
void drawChars(char s, float x, float y, const float r, const float g, const float b) {
	glColor3f(r, g, b);
	glLineWidth(2.0);
	glRasterPos3f(x, y, 0.0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s);
	glColor3f(1.0f, 1.0f, 1.0f);
}


//funkcija za upis u file
void writeToFile() {
	char ime[100];

	std::ofstream ofile;
	ofile.open("resources/highscore.txt", std::fstream::app);

	std::cout << "Unesite vase ime : " << std::endl;
	std::cin.getline(ime, 100);

	ofile << ime << "'s Score : " << scr << " Lives left : " << lives << "-" << std::endl;
	ofile.close();
}
//funkcija za ispis iz file-a
void readFromFile() {
	std::ifstream ifs("resources/highscore.txt");
	highscore.assign((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	std::vector<char> chars(highscore.begin(), highscore.end());
	chars.push_back('\0');

	char* c = &chars[0];

	DrawText("Highscore",screen_width/2-50,20,0,0,1);

	float startX = 0.8;
	float startY = 100.0f;

	float spaceY = 20.0f;
	float spaceX = 10.0f;
	int l = highscore.length();
	for (int i = 0; i <= l;i++) {
		if (c[i] == '-') {
			startX = 0;
			startY +=  spaceY;
		}else if(c[i] != '-'){
			startX++;
			drawChars(c[i], startX + spaceX * startX, startY, 0, 0, 1);
		}
	}
}




void DrawCredits()
{
	float startX = 325.0f;
	float startY = 250.0f;
	float spaceY = 30.0f;
	DrawText("Robert Madsen", startX, startY, 0.0f, 0.0f, 1.0f);
	DrawText("Author", startX, startY + spaceY, 0.0f, 0.0f, 1.0f);
}
void DrawStats()
{
	char life[50];
	char score[50];
	sprintf_s(life, 50, "Lives: %i", lives);
	sprintf_s(score, 50, "Score: %i", scr);
	DrawText(life, 350.0f, 320.0f, 0.0f, 0.0f, 1.0f);
	DrawText(score, 350.0f, 370.0f, 0.0f, 0.0f, 1.0f);
}
void DrawScore()
{
	char score[50];
	sprintf_s(score, 50, "Score: %i Lives: %i", scr , lives);
	DrawText(score, 350.0f, 25.0f, 0.0f, 0.0f, 1.0f);
}



void LoadSplash()
{
	m_gameState = GameState::GS_Splash;
	splashScreen = new Sprite(1);
	splashScreen->SetFrameSize(800.0f, 600.0f);
	splashScreen->SetNumberOfFrames(1);
	splashScreen->AddTexture("resources/splash.png", false);
	splashScreen->IsActive(true);
	splashScreen->IsVisible(true);
}

const bool LoadTextures()
{	


	platform = new Sprite(1);
	platform->SetFrameSize(platformWidth, platformHeight);
	platform->SetNumberOfFrames(1);
	platform->AddTexture("resources/platform.png", true);
	platform->IsActive(true);
	platform->IsVisible(true);
	platform->IsCollideable(true);
	platform->SetPosition(screen_width / 2 - platform->GetSize().width/2, 550);

	platform2 = new Sprite(1);
	platform2->SetFrameSize(platformWidth, platformHeight);
	platform2->SetNumberOfFrames(1);
	platform2->AddTexture("resources/platform.png", true);
	platform2->IsActive(true);
	platform2->IsVisible(true);
	platform2->IsCollideable(true);
	platform2->SetPosition(screen_width / 2 - platform2->GetSize().width / 2, platform2Pos);


	extraBall = new Sprite(1);
	extraBall->SetFrameSize(20, 20);
	extraBall->SetNumberOfFrames(1);
	extraBall->AddTexture("resources/ball2.png", true);
	extraBall->IsActive(false);
	extraBall->IsVisible(false);


	player = new Sprite(1);
	player->SetFrameSize(20,20);
	player->SetNumberOfFrames(1);
	player->AddTexture("resources/ball.png", true);
	player->IsActive(true);
	player->IsVisible(true);
	player->SetPosition(screen_width/2,400);
	player->SetVelocityY(-ballSpeed);
	player->SetVelocityX(ballSpeed);

	widthBrick = new Sprite(1);
	widthBrick->SetFrameSize(60.0f, 30.0f);
	widthBrick->SetNumberOfFrames(1);
	widthBrick->IsVisible(false); 
	widthBrick->IsActive(false);
	widthBrick->IsCollideable(true); 
	widthBrick->AddTexture("resources/widthBrick.png");

	lifeBrick = new Sprite(1);
	lifeBrick->SetFrameSize(60.0f, 30.0f);
	lifeBrick->SetNumberOfFrames(1);
	lifeBrick->IsVisible(false);
	lifeBrick->IsActive(false);
	lifeBrick->IsCollideable(true);
	lifeBrick->AddTexture("resources/heartBrick.png");

	extraBallBrick = new Sprite(1);
	extraBallBrick->SetFrameSize(60.0f, 30.0f);
	extraBallBrick->SetNumberOfFrames(1);
	extraBallBrick->IsVisible(false);
	extraBallBrick->IsActive(false);
	extraBallBrick->IsCollideable(true);
	extraBallBrick->AddTexture("resources/extraBallBrick.png");

	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			BlueBrick[i][j] = new Sprite(1);
			BlueBrick[i][j]->SetFrameSize(60.0f, 30.0f);
			BlueBrick[i][j]->SetNumberOfFrames(1);
			BlueBrick[i][j]->IsVisible(false);
			BlueBrick[i][j]->IsActive(false);
			BlueBrick[i][j]->IsCollideable(true);


			if (i == 0 && j == 0) BlueBrick[i][j]->AddTexture("resources/blueBrick.png");
			else BlueBrick[i][j]->SetTextureID(BlueBrick[0][0]);

		}
	}

	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
				RedBrick[i][j] = new Sprite(1);
				RedBrick[i][j]->SetFrameSize(60.0f, 30.0f);
				RedBrick[i][j]->SetNumberOfFrames(1);
				RedBrick[i][j]->IsVisible(false);
				RedBrick[i][j]->IsActive(false);
				RedBrick[i][j]->IsCollideable(true);


				if (i == 0 && j == 0) RedBrick[i][j]->AddTexture("resources/redBrick.png");
				else RedBrick[i][j]->SetTextureID(RedBrick[0][0]);

			}
		}

	pauseButton = new Sprite(1);
	pauseButton->SetFrameSize(75.0f, 38.0f);
	pauseButton->SetNumberOfFrames(1);
	pauseButton->SetPosition(5.0f, 5.0f);
	pauseButton->AddTexture("resources/pauseButton.png");
	pauseButton->IsVisible(true);
	pauseButton->IsActive(true);
	inputManager->AddUiElement(pauseButton);

	resumeButton = new Sprite(1);
	resumeButton->SetFrameSize(75.0f, 38.0f);
	resumeButton->SetNumberOfFrames(1);
	resumeButton->SetPosition(80.0f, 5.0f);
	resumeButton->AddTexture("resources/resumeButton.png");
	inputManager->AddUiElement(resumeButton);

	Sprite::Rect collision;
	collision.left = 0.0f;
	collision.right = 0.0f;
	collision.top = 0.0f;
	collision.bottom = 0.0f;
	

	Sprite::Rect colider2;
	colider2.left = platform2->GetSize().width / 2;
	colider2.right = platform2->GetSize().width / 2;
	colider2.top = platform2->GetSize().height / 2;
	colider2.bottom = platform2->GetSize().height / 2;

	Sprite::Rect colider;
	colider.left = platform->GetSize().width / 2;
	colider.right = platform->GetSize().width / 2;
	colider.top = platform->GetSize().height / 2;
	colider.bottom = platform->GetSize().height / 2;

	Sprite::Rect coll;
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {

			coll.left = BlueBrick[i][j]->GetSize().width;
			coll.right = BlueBrick[i][j]->GetSize().width;
			coll.top = BlueBrick[i][j]->GetSize().height;
			coll.bottom = BlueBrick[i][j]->GetSize().height;

		}
	}
	Sprite::Rect coll1;
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {

			coll1.left = RedBrick[i][j]->GetSize().width;
			coll1.right = RedBrick[i][j]->GetSize().width;
			coll1.top = RedBrick[i][j]->GetSize().height;
			coll1.bottom = RedBrick[i][j]->GetSize().height;

		}
	}
	menuScreen = new Sprite(1);
	menuScreen->SetFrameSize(800.0f, 600.0f);
	menuScreen->SetNumberOfFrames(1);
	menuScreen->AddTexture("resources/mainmenu.png", false);
	menuScreen->IsActive(true);
	menuScreen->IsVisible(true);

	playButton = new Sprite(1);
	playButton->SetFrameSize(75.0f, 38.0f);
	playButton->SetNumberOfFrames(1);
	playButton->SetPosition(390.0f, 300.0f);
	playButton->AddTexture("resources/playButton.png");
	playButton->IsVisible(true);
	playButton->IsActive(false);
	//inputManager->AddUiElement(playButton);

	highscoreButton = new Sprite(1);
	highscoreButton->SetFrameSize(75.0f, 38.0f);
	highscoreButton->SetNumberOfFrames(1);
	highscoreButton->SetPosition(390.0f, 400.0f);
	highscoreButton->AddTexture("resources/highscoreButton.png");
	highscoreButton->IsVisible(true);
	highscoreButton->IsActive(false);
	inputManager->AddUiElement(highscoreButton);


	creditsButton = new Sprite(1);
	creditsButton->SetFrameSize(75.0f, 38.0f);
	creditsButton->SetNumberOfFrames(1);
	creditsButton->SetPosition(390.0f, 350.0f);
	creditsButton->AddTexture("resources/creditsButton.png");
	creditsButton->IsVisible(true);
	creditsButton->IsActive(false);
	inputManager->AddUiElement(creditsButton);

	exitButton = new Sprite(1);
	exitButton->SetFrameSize(75.0f, 38.0f);
	exitButton->SetNumberOfFrames(1);
	exitButton->SetPosition(390.0f, 500.0f);
	exitButton->AddTexture("resources/exitButton.png");
	exitButton->IsVisible(true);
	exitButton->IsActive(false);
	inputManager->AddUiElement(exitButton);

	creditsScreen = new Sprite(1);
	creditsScreen->SetFrameSize(800.0f, 600.0f);
	creditsScreen->SetNumberOfFrames(1);
	creditsScreen->AddTexture("resources/credits.png", false);
	creditsScreen->IsActive(false);
	creditsScreen->IsVisible(true);
	
	menuButton = new Sprite(1);
	menuButton->SetFrameSize(75.0f, 38.0f);
	menuButton->SetNumberOfFrames(1);
	menuButton->SetPosition(160.0f, 5.0f);
	menuButton->AddTexture("resources/menuButton.png");
	menuButton->IsVisible(false);
	menuButton->IsActive(false);
	inputManager->AddUiElement(menuButton);

	nextLevelScreen = new Sprite(1);
	nextLevelScreen->SetFrameSize(800.0f, 600.0f);
	nextLevelScreen->SetNumberOfFrames(1);
	nextLevelScreen->AddTexture("resources/level.png", false);
	nextLevelScreen->IsActive(true);
	nextLevelScreen->IsVisible(true);

	continueButton = new Sprite(1);
	continueButton->SetFrameSize(75.0f, 38.0f);
	continueButton->SetNumberOfFrames(1);
	continueButton->SetPosition(390.0f, 400.0f);
	continueButton->AddTexture("resources/continueButton.png");
	continueButton->IsVisible(true);
	continueButton->IsActive(false);
	inputManager->AddUiElement(continueButton);

	gameOverScreen = new Sprite(1);
	gameOverScreen->SetFrameSize(800.0f, 600.0f);
	gameOverScreen->SetNumberOfFrames(1);
	gameOverScreen->AddTexture("resources/gameover.png", false);
	gameOverScreen->IsActive(true);
	gameOverScreen->IsVisible(true);

	replayButton = new Sprite(1);
	replayButton->SetFrameSize(75.0f, 38.0f);
	replayButton->SetNumberOfFrames(1);
	replayButton->SetPosition(390.0f, 400.0f);
	replayButton->AddTexture("resources/replayButton.png");
	replayButton->IsVisible(true);
	replayButton->IsActive(false);
	inputManager->AddUiElement(replayButton);

	return true;
}

void CheckCollisionBlue(Sprite* p_sprite)
{
	Sprite::Rect collider;
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			collider = BlueBrick[i][j]->GetCollisionRect();
			//std::cout << collider.left << std::endl;
			
			if (p_sprite->GetCollisionRect().top < collider.bottom) {
				if (p_sprite->GetCollisionRect().bottom > collider.top) {
					if (p_sprite->GetCollisionRect().left < collider.right) {
						if (p_sprite->GetCollisionRect().right > collider.left) {
							FMOD::Channel* channel;
							audiomgr->playSound(sfxHitBrick, 0, false, &channel);
							BlueBrick[i][j]->IsVisible(false);
							BlueBrick[i][j]->IsActive(false);
							BlueBrick[i][j]->IsCollideable(false);
							BlueBrick[i][j]->SetPosition(3000,0);
							scr++;
							p_sprite->SetVelocityY(p_sprite->GetVelocityY() * -1);
						}
					}
				}
				

			}
			
			
		}
	}
	
}
void CheckSpecialColider(Sprite* p_sprite) {
	Sprite::Rect xtraBallCol = extraBallBrick->GetCollisionRect();
	if (p_sprite->GetCollisionRect().top < xtraBallCol.bottom) {
		if (p_sprite->GetCollisionRect().bottom > xtraBallCol.top) {
			if (p_sprite->GetCollisionRect().left < xtraBallCol.right) {
				if (p_sprite->GetCollisionRect().right > xtraBallCol.left) {
					FMOD::Channel* channel;
					audiomgr->playSound(sfxHitBrick, 0, false, &channel);
					scr++;
					extraBallBrick->IsVisible(false);
					extraBallBrick->IsActive(false);
					extraBallBrick->IsCollideable(false);
					extraBallBrick->SetPosition(1000, 1);
					xtraBall = true;

					std::cout << xtraBall << std::endl;

					//ENTER LOGIC
					if (xtraBall == true) {
						extraBall->IsVisible(true);
						extraBall->IsActive(true);
						extraBall->IsCollideable(true);
						extraBall->SetPosition(screen_width / 2, 200);
						extraBall->SetVelocityY(-ballSpeed);
						extraBall->SetVelocityX(ballSpeed);
					}
					p_sprite->SetVelocityY(p_sprite->GetVelocityY() * -1);
				}
			}
		}
	}

	Sprite::Rect heartBrickCol = lifeBrick->GetCollisionRect();
	if (p_sprite->GetCollisionRect().top < heartBrickCol.bottom) {
		if (p_sprite->GetCollisionRect().bottom > heartBrickCol.top) {
			if (p_sprite->GetCollisionRect().left < heartBrickCol.right) {
				if (p_sprite->GetCollisionRect().right > heartBrickCol.left) {
					FMOD::Channel* channel;
					audiomgr->playSound(sfxHitBrick, 0, false, &channel);
					scr++;
					lifeBrick->IsVisible(false);
					lifeBrick->IsActive(false);
					lifeBrick->IsCollideable(false);
					lifeBrick->SetPosition(1000, 1);
					lives++;
					p_sprite->SetVelocityY(p_sprite->GetVelocityY() * -1);
				}
			}
		}
	}


	Sprite::Rect widthBrickCol = widthBrick->GetCollisionRect();
	if (p_sprite->GetCollisionRect().top < widthBrickCol.bottom) {
		if (p_sprite->GetCollisionRect().bottom > widthBrickCol.top) {
			if (p_sprite->GetCollisionRect().left < widthBrickCol.right) {
				if (p_sprite->GetCollisionRect().right > widthBrickCol.left) {
					FMOD::Channel* channel;
					audiomgr->playSound(sfxHitBrick, 0, false, &channel);
					scr++;
					widthBrick->IsVisible(false);
					widthBrick->IsActive(false);
					widthBrick->IsCollideable(false);
					widthBrick->SetPosition(1000, 1);
					platformWidth = 200;
					p_sprite->SetVelocityY(p_sprite->GetVelocityY() * -1);
				}
			}
		}
	}

}
void CheckCollisionRed(Sprite* p_sprite)
{
	
	Sprite::Rect collider;
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			collider = RedBrick[i][j]->GetCollisionRect();

			if (p_sprite->GetCollisionRect().top < collider.bottom) {
				if (p_sprite->GetCollisionRect().bottom > collider.top) {
					if (p_sprite->GetCollisionRect().left < collider.right) {
						if (p_sprite->GetCollisionRect().right > collider.left) {
							FMOD::Channel* channel;
							scr++;
							audiomgr->playSound(sfxHitBrick, 0, false, &channel);
							RedBrick[i][j]->IsVisible(false);
							RedBrick[i][j]->IsActive(false);
							RedBrick[i][j]->IsCollideable(false);
							RedBrick[i][j]->SetPosition(1000, 0);
							p_sprite->SetVelocityY(p_sprite->GetVelocityY() * -1);
						}
					}
				}
			}


		}
	}

}

void spawnBlueBricks(GLfloat x, GLfloat y) {
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			BlueBrick[i][j]->SetPosition(x*i*0.7+60,y*j*0.5+180);
			BlueBrick[i][j]->IsVisible(true);
			BlueBrick[i][j]->IsActive(true);
		}
	}
}
void spawnRedBricks(GLfloat x, GLfloat y) {



	int iLife = rand() % columns;
	int jLife = rand() % rows;

	int iWidth = rand() % columns;
	int jWidth = rand() % rows;

	int iExtraBall = rand() % columns;
	int jExtraBall = rand() % rows;



	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			//std::cout << iWidth << "-" << jWidth << " " << iLife << "-" << jLife << std::endl;
			if (iWidth == iLife || jWidth == jLife) {
				iWidth = rand() % columns;
				jWidth = rand() % rows;
			}

			if (iExtraBall == iLife || jExtraBall == jLife && iExtraBall == iWidth || jExtraBall == jWidth) {
				iExtraBall = rand() % columns;
				jExtraBall = rand() % rows;
			}


			if (i == iWidth && j == jWidth) {
				widthBrick->SetPosition(x * i * 0.7 + 60, y * j * 0.5 + 180);
				widthBrick->IsVisible(true);
				widthBrick->IsActive(true);
			}
			else if (i == iLife && j == jLife) {
				lifeBrick->SetPosition(x * i * 0.7 + 60, y * j * 0.5 + 180);
				lifeBrick->IsVisible(true);
				lifeBrick->IsActive(true);
			}
			else if (i == iExtraBall && j == jExtraBall) {
				extraBallBrick->SetPosition(x * i * 0.7 + 60, y * j * 0.5 + 180);
				extraBallBrick->IsVisible(true);
				extraBallBrick->IsActive(true);
				xtraBall = false;
			}
			else {
				RedBrick[i][j]->SetPosition(x * i * 0.7 + 60, y * j * 0.5 + 180);
				RedBrick[i][j]->IsVisible(true);
				RedBrick[i][j]->IsActive(true);
			}

		}
	}

}
void bounceBall(Sprite* p_sprite) {
	Sprite::Rect collider;
	collider = platform->GetCollisionRect();
	if (p_sprite->GetCollisionRect().top < collider.bottom) {
		if (p_sprite->GetCollisionRect().bottom > collider.top) {
			if (p_sprite->GetCollisionRect().left < collider.right) {
				if (p_sprite->GetCollisionRect().right > collider.left) {

					FMOD::Channel* channel;
					audiomgr->playSound(sfxHitPaddle, 0, false, &channel);
					p_sprite->SetVelocityY(p_sprite->GetVelocityY() * -1);
				}
			}
		}


	}
	Sprite::Rect collider2;
	collider2 = platform2->GetCollisionRect();
	if (p_sprite->GetCollisionRect().top < collider2.bottom) {
		if (p_sprite->GetCollisionRect().bottom > collider2.top) {
			if (p_sprite->GetCollisionRect().left < collider2.right) {
				if (p_sprite->GetCollisionRect().right > collider2.left) {

					FMOD::Channel* channel;
					audiomgr->playSound(sfxHitPaddle, 0, false, &channel);
					p_sprite->SetVelocityY(p_sprite->GetVelocityY() * -1);
				}
			}
		}


	}
}

void CheckBoundaries(Sprite* p_sprite)
{
	Sprite::Rect check = p_sprite->GetCollisionRect();

	if (p_sprite == platform || p_sprite == platform2) {
		if (check.left < 0.0f)
		{
			p_sprite->SetVelocityX(0);
			p_sprite->SetPosition(p_sprite->GetPosition().x + 1 , p_sprite->GetPosition().y);
		}
		else if (check.right > screen_width)
		{
			p_sprite->SetVelocityX(0);
			p_sprite->SetPosition(p_sprite->GetPosition().x - 1, p_sprite->GetPosition().y);
		}
		
	}
	else {
		if (check.left < 0.0f)
		{
			FMOD::Channel* channel;
			audiomgr->playSound(sfxHitPaddle, 0, false, &channel);

			p_sprite->SetVelocityX(50);
		}
		if (check.right > screen_width)
		{
			FMOD::Channel* channel;
			audiomgr->playSound(sfxHitPaddle, 0, false, &channel);

			p_sprite->SetVelocityX(-50);
		}
		if (check.top < 0.0f)
		{
			FMOD::Channel* channel;
			audiomgr->playSound(sfxHitPaddle, 0, false, &channel);
			if (lvl == 2) {
				
				xtraBall = false;
				player->SetVelocityY(-50.0f);
				player->SetVelocityY(-50);
				player->SetPosition(screen_width / 2, 400);
				platformWidth = 150;
			}
			else {
				p_sprite->SetVelocityY(50);
			}
			

			if (xtraBall == false) {
				extraBall->IsActive(false);
				extraBall->IsVisible(false);
				extraBall->SetVelocityY(0.0f);
				extraBall->SetVelocityX(0.0f);
				extraBall->SetPosition(20,5);
			}
		}
	
		if (check.bottom >= screen_height)
		{
			xtraBall = false;
			FMOD::Channel* channel;
			audiomgr->playSound(sfxDeath, 0, false, &channel);
			platformWidth = 150;
			lives--;
			player->SetVelocityY(-50.0f);
			player->SetVelocityY(-50);
			player->SetPosition(screen_width / 2, 400);
			
			if (xtraBall == false) {
					extraBall->IsActive(false);
					extraBall->IsVisible(false);
					extraBall->SetVelocityY(0.0f);
					extraBall->SetVelocityX(0.0f);
					extraBall->SetPosition(20, 5);
				}
		}
	}
} 

void RestartGame()
{
	lvl = 1;
	player->SetValue(0);
	splashDisplayTimer = 0.0f;
	splashDisplayThreshold = 5.0f;
	scr = 0;
	lives = 3;
	player->SetVelocityY(-ballSpeed);
	player->SetVelocityX(ballSpeed);
	player->SetPosition(screen_width / 2, 400);
	platform->SetPosition(screen_width / 2 - platform->GetSize().width / 2, 550);
	platform2->SetPosition(screen_width / 2 - platform2->GetSize().width / 2, platform2Pos);
}



void NextLevel()
{
	if (lives <= 0) {
		m_gameState = GameState::GS_GameOver;
		writeToFile();
	}
	if (lvl == 1) {
		if (scr == rows * columns) {
			m_gameState = GameState::GS_NextLevel;
		}
	}
	else if (lvl == 2) {
		if (scr == rows * columns * 2) {
			m_gameState = GameState::GS_NextLevel;
			writeToFile();
		}
	}

	
}

void ProcessInput()
{
	Input::Command command = inputManager->GetCommand();
	switch (m_gameState)
	{
	case GameState::GS_Splash:
	case GameState::GS_Loading:
	{
		return;
	}
	break;
	case GameState::GS_Menu:
	case GameState::GS_Credits:
	case GameState::GS_Level2:
	{
	
	}
	break;
	case GameState::GS_Paused:
	case GameState::GS_Highscore:
	case GameState::GS_NextLevel:
	case GameState::GS_GameOver:
	{
		command = Input::Command::CM_UI;
	}
	break;
	case GameState::GS_Running:
	{

	}
	break;
	}

	uiTimer += p_deltaTime;
	if (uiTimer > UI_THRESHOLD)
	{
		uiTimer = 0.0f;
		switch (command)
		{
		case Input::Command::CM_STOP:
		{
			player->SetVelocityX(0.0f);
			player->SetVelocityY(0.0f);
		}
		break;

		case Input::Command::CM_LEFT:
		{
			platform->SetVelocityX(-60);
		}
		break;

		case Input::Command::CM_RIGHT:
		{
			platform->SetVelocityX(60);
		}
		break;
		case Input::Command::CM_Left2:
		{
			platform2->SetVelocityX(-60);
		}
		break;

		case Input::Command::CM_Right2:
		{
			platform2->SetVelocityX(60);
		}
		break;
		case Input::Command::CM_UP:
		{
		}
		break;

		case Input::Command::CM_DOWN:
		{
		}
		break;

		case Input::Command::CM_QUIT:
		{
			PostQuitMessage(0);
		}
		break;

		case Input::Command::CM_UI:
		{
			FMOD::Channel* channel;
			if (pauseButton->IsClicked())
			{
				audiomgr->playSound(sfxButton, 0, false, &channel);
				pauseButton->IsClicked(false);
				pauseButton->IsVisible(false);
				pauseButton->IsActive(false);

				menuButton->IsVisible(true);
				menuButton->IsActive(true);
				menuButton->SetPosition(160.0f, 5.0f);

				resumeButton->IsVisible(true);
				resumeButton->IsActive(true);
				m_gameState = GS_Paused;
			}

			if (resumeButton->IsClicked())
			{
				audiomgr->playSound(sfxButton, 0, false, &channel);
				resumeButton->IsClicked(false);
				resumeButton->IsVisible(false);
				resumeButton->IsActive(false);

				menuButton->IsVisible(false);
				menuButton->IsActive(false);

				pauseButton->IsVisible(true);
				pauseButton->IsActive(true);
				m_gameState = GS_Running;
			}
			if (playButton->IsClicked())
			{
				platformWidth = 150;
				ballSpeed = 50;
				for (int i = 0; i < columns; i++) {
					for (int j = 0; j < rows; j++) {
						RedBrick[i][j]->SetPosition(1000, 0);
						RedBrick[i][j]->IsVisible(false);
					}
				}
				spawnBlueBricks(100,100);
				audiomgr->playSound(sfxButton, 0, false, &channel);
				playButton->IsClicked(false);
				exitButton->IsActive(false);
				playButton->IsActive(false);
				creditsButton->IsActive(false);
				
				pauseButton->IsVisible(true);
				pauseButton->IsActive(true);
				RestartGame();
				lvl = 1;
				resumeButton->IsVisible(false);
				resumeButton->IsActive(false);

				menuButton->IsVisible(false);
				menuButton->IsActive(false);

				platform->SetVelocityX(0);
				platform2->SetVelocityX(0);
				platform2->SetPosition(2000,2000);

				m_gameState = GameState::GS_Running;
			}	
			if (highscoreButton->IsClicked())
			{
				audiomgr->playSound(sfxButton, 0, false, &channel);
				highscoreButton->IsClicked(false);
				highscoreButton->IsActive(false);
				exitButton->IsActive(false);
				playButton->IsActive(false);
				menuButton->SetPosition(10, 550);
				menuButton->IsVisible(true);
				menuButton->IsActive(true);
				m_gameState = GameState::GS_Highscore;
			}

			if (creditsButton->IsClicked())
			{
				audiomgr->playSound(sfxButton, 0, false, &channel);
				creditsButton->IsClicked(false);
				exitButton->IsActive(false);
				playButton->IsActive(false);
				menuButton->SetPosition(400,350);
				menuButton->IsVisible(true);
				menuButton->IsActive(true);
				creditsButton->IsActive(false);
				m_gameState = GameState::GS_Credits;
			}
			if (exitButton->IsClicked())
			{
				audiomgr->playSound(sfxButton, 0, false, &channel);
				playButton->IsClicked(false);
				exitButton->IsActive(false);
				playButton->IsActive(false);
				creditsButton->IsActive(false);
				PostQuitMessage(0);
			}
			if (menuButton->IsClicked())
			{
				scr = 0;
				audiomgr->playSound(sfxButton, 0, false, &channel);
				menuButton->IsClicked(false);
				menuButton->IsActive(false);
				m_gameState = GameState::GS_Menu;
			}
			if (continueButton->IsClicked())
			{
				lvl = 2;
				for (int i = 0; i < columns; i++) {
					for (int j = 0; j < rows; j++) {
						BlueBrick[i][j]->SetPosition(1000,0);
						BlueBrick[i][j]->IsVisible(false);
					}
				}
				ballSpeed *= 1.5;
				platform->SetVelocityX(0);
				platform2->SetVelocityX(0);
				lives += 1;
				player->SetVelocityY(-ballSpeed);
				player->SetVelocityX(ballSpeed);
				player->SetPosition(screen_width / 2, 400);
				platform->SetPosition(screen_width / 2 - platform->GetSize().width / 2, 550);
				platform2->SetPosition(screen_width / 2 - platform->GetSize().width / 2, platform2Pos);
				audiomgr->playSound(sfxButton, 0, false, &channel);
				continueButton->IsClicked(false);
				continueButton->IsActive(false);
				spawnRedBricks(100, 100);
				m_gameState = GameState::GS_Level2;
			}
			if (replayButton->IsClicked())
			{
				audiomgr->playSound(sfxButton, 0, false, &channel);
				replayButton->IsClicked(false);
				replayButton->IsActive(false);
				exitButton->IsActive(false);
				RestartGame();
				m_gameState = GameState::GS_Menu;
			}
		
		}
		break;
		}
	}
}

void Update()
{
	menuButton->Render();
	switch (m_gameState)
	{
	case GameState::GS_Splash:
	case GameState::GS_Loading:
	{
		spawnBlueBricks(100, 100);
		splashScreen->Update(p_deltaTime);
		splashDisplayTimer += p_deltaTime;
		if (splashDisplayTimer > splashDisplayThreshold)
		{
			m_gameState = GameState::GS_Menu;
		}
	}
	break;
	case GameState::GS_Menu:
	{
		menuScreen->Update(p_deltaTime);
		playButton->IsActive(true);
		creditsButton->IsActive(true);
		exitButton->IsActive(true);
		playButton->Update(p_deltaTime);
		highscoreButton->IsActive(true);
		highscoreButton->Update(p_deltaTime);
		creditsButton->Update(p_deltaTime);
		exitButton->Update(p_deltaTime);
		ProcessInput();
	}
	break;

	case GameState::GS_Credits:
	{
		creditsScreen->Update(p_deltaTime);
		menuButton->IsActive(true);
		menuButton->Update(p_deltaTime);
		ProcessInput();
	}
	break;
	case GameState::GS_Running:
	{

		CheckCollisionBlue(player);
		CheckBoundaries(player);
		bounceBall(player);
		CheckBoundaries(platform);

		ProcessInput();


		player->Update(p_deltaTime);
		extraBall->Update(p_deltaTime);
		pauseButton->Update(p_deltaTime);
		resumeButton->Update(p_deltaTime);
		menuButton->Update(p_deltaTime);
		platform->Update(p_deltaTime);
		platform2->Update(p_deltaTime);
		

		for (int i = 0; i < columns; i++) {
			for (int j = 0; j < rows; j++) {
				BlueBrick[i][j]->Update(p_deltaTime);
			}
		}
	
		NextLevel();

	}
	break;
	case GameState::GS_Level2:
	{	
		ProcessInput();

		CheckBoundaries(player); 
		bounceBall(player);
		CheckCollisionRed(player);
		CheckSpecialColider(player);

		CheckBoundaries(platform);
		CheckBoundaries(platform2);

		CheckBoundaries(extraBall);
		bounceBall(extraBall);
		CheckCollisionRed(extraBall);
		CheckSpecialColider(extraBall);


		//scr = 0;
		widthBrick->Update(p_deltaTime);
		lifeBrick->Update(p_deltaTime);
		extraBallBrick->Update(p_deltaTime);
		player->Update(p_deltaTime);
		extraBall->Update(p_deltaTime);
		pauseButton->Update(p_deltaTime);
		resumeButton->Update(p_deltaTime);
		menuButton->Update(p_deltaTime);
		platform->Update(p_deltaTime);
		platform2->Update(p_deltaTime);

		for (int i = 0; i < columns; i++) {
			for (int j = 0; j < rows; j++) {
				RedBrick[i][j]->Update(p_deltaTime);
			}
		}
		NextLevel();
	}
	break;
	case GameState::GS_Highscore:
	{
		menuButton->IsActive(true);
		menuButton->Update(p_deltaTime);
		ProcessInput();
	}
	break;
	case GameState::GS_Paused:
	{
		ProcessInput();
	}
	break;
	case GameState::GS_NextLevel:
	{
		highscoreButton->IsActive(false);
		nextLevelScreen->Update(p_deltaTime);
		continueButton->Update(p_deltaTime);
		menuButton->Update(p_deltaTime);
		if (lvl == 1) {
			continueButton->IsActive(true);
			continueButton->IsVisible(true);
			continueButton->SetPosition(400, 400);
			menuButton->SetPosition(4000, 3050);
			continueButton->IsActive(true);
			ProcessInput();
		}else if(lvl == 2){
			continueButton->IsActive(false);
			continueButton->IsVisible(false);
			menuButton->SetPosition(400, 400);
			menuButton->IsVisible(true);
			menuButton->IsActive(true);
			continueButton->SetPosition(4000, 3050);
			nextLevelScreen->Update(p_deltaTime);
			ProcessInput();
		}
		

		
	}
	break;
	case GameState::GS_GameOver:
	{
		gameOverScreen->Update(p_deltaTime);
		replayButton->IsActive(true);
		replayButton->Update(p_deltaTime);
		exitButton->IsActive(true);
		exitButton->Update(p_deltaTime);
		ProcessInput();
	}
	break;
	}

	inputManager->SetCommand(Input::Command::CM_INVALID);
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	platform->SetFrameSize(platformWidth, platformHeight);
	platform2->SetFrameSize(platformWidth, platformHeight);

	switch (m_gameState)
	{
	case GameState::GS_Splash:
	case GameState::GS_Loading:
	{
		splashScreen->Render();
	}
	break;
	case GameState::GS_Menu:
	{
		menuScreen->Render();
		playButton->Render();
		highscoreButton->Render();
		creditsButton->Render();
		exitButton->Render();
	}
	break;
	case GameState::GS_Credits:
	{
		creditsScreen->Render();
		menuButton->Render();
		DrawCredits();
	}
	break;
	case GameState::GS_Running:
	{
	}
	case GameState::GS_Level2:
	{

	}
	
	case GameState::GS_Paused:
	{

		for (int i = 0; i < columns; i++) {
			for (int j = 0; j < rows; j++) {
				BlueBrick[i][j]->Render();
			}
		}	
		for (int i = 0; i < columns; i++) {
			for (int j = 0; j < rows; j++) {
				RedBrick[i][j]->Render();
			}
		}
		pauseButton->Render();
		resumeButton->Render();
		menuButton->Render();
		player->Render();
		platform->Render();
		if (lvl == 2) {
			extraBall->Render();
			platform2->Render();
			widthBrick->Render();
			lifeBrick->Render();
			extraBallBrick->Render();
		}
		DrawScore();
	}
	break;
	case GameState::GS_NextLevel:
	{
		nextLevelScreen->Render();
		DrawStats();
		continueButton->Render();
	}
	break;
	case GameState::GS_GameOver:
	{
		gameOverScreen->Render();
		DrawStats();
		replayButton->Render();
	}
	break;
	case GameState::GS_Highscore:
	{
		readFromFile();
		menuButton->Render();
	}
	break;
	}

	glutSwapBuffers();
}

void StartGame()
{
	LoadSplash();

	inputManager = new Input();

	uiTimer = 0.0f;
	srand(time(NULL));

	pickupSpawnThreshold = 3.0f;
	pickupSpawnTimer = 0.0f;

	enemySpawnThreshold = 7.0f;
	enemySpawnTimer = 0.0f;

	splashDisplayTimer = 0.0f;
	splashDisplayThreshold = 5.0f;

}

void GameLoop()
{
	
	if (m_gameState == GameState::GS_Splash)
	{
		InitFmod();
		LoadAudio();
		LoadTextures();
		m_gameState = GameState::GS_Loading;
	}

	Update();
	Render();
}

void EndGame()
{
	delete pauseButton;
	delete resumeButton;
	delete splashScreen;
	delete menuScreen;
	delete creditsScreen;
	delete playButton;
	delete creditsButton;
	delete exitButton;
	delete menuButton;
	delete nextLevelScreen;
	delete continueButton;
	delete gameOverScreen;
	delete replayButton;

	delete inputManager;

	sfxDeath->release();
	sfxHitBrick->release();
	sfxHitPaddle->release();
	sfxButton->release();
	musBackground->release();
	audiomgr->release();
}

void reshape(const GLsizei p_width, const GLsizei p_height)
{
	screen_height = (GLfloat)p_height;
	screen_width = (GLfloat)p_width;
	GLsizei h = p_height;
	GLsizei w = p_width;
	glutReshapeWindow(800, 600);
	if (h == 0) h = 1;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void initGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
	glEnable(GL_TEXTURE_2D);
	//glShadeModel(GL_SMOOTH);
	glShadeModel(GL_FLAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_DEPTH_TEST);

	previousTime = glutGet(GLUT_ELAPSED_TIME);
}

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(refreshMills, timer, 0);
}

void display()
{
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	p_deltaTime = (float)(currentTime - previousTime) / 1000;
	previousTime = currentTime;

	GameLoop();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:     // ESC key
		exit(0);
		break;
	default:
		inputManager->keyboardKey(key, x, y);
	}
}

void specialKeys(int key, int x, int y)
{
	inputManager->keyboardSpec(key, x, y);
}

void mouse(int button, int state, int x, int y)
{
	inputManager->mouse(button, state, x, y);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);  // Enable double buffered mode

	screen_height = (GLfloat)600;
	screen_width = (GLfloat)800;


	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("RoboRacer 2D");
	// Callback Functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, timer, 0);

	// Keyboard and Mouse
	glutSpecialFunc(specialKeys);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	

	// Initialise GL and Game
	initGL();
	StartGame();

	// Game Loop
	glutMainLoop();

	EndGame();

	return 0;
}
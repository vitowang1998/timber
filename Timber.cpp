// HelloSFML.cpp : Defines the entry point for the console application.

// TODO: OOP Bee
// TODO: OOP Cloud Array

#include "stdafx.h"
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

enum class side {
	LEFT,
	RIGHT,
	NONE
};

const short NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];
side branchPositions[NUM_BRANCHES];

void updateBranches(int seed) {
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}
	// Spawn a new branch at the top of the tree
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}

int main() {
	VideoMode vm(1920, 1080);

	RenderWindow window(vm, "Timber", Style::Fullscreen);

	// texture used to hold graphics on GPU
	Texture textureBackground;
	textureBackground.loadFromFile("graphics/background.png");

	// create a sprite
	Sprite spriteBackground;

	// attach the background texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// set the background sprite to (0,0) to cover the entire screen
	spriteBackground.setPosition(0, 0);

	// tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	/**** bee sprite ****/
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	// boolean determining whether the bee is currently flying
	bool beeActive = false;
	// float controls the speed of the bee
	float beeSpeed = 0.0f;

	/**** clouds sprite ****/
	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");
	// generate 3 clouds
	Sprite spriteClouds[3];
	for (int i = 0; i < 3; i++) {
		static int height = 0;
		spriteClouds[i].setPosition(0, height);
		spriteClouds[i].setTexture(textureCloud);
		height += 250;
	}
	// booleans control whether the clouds are moving or not
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	
	// floats control the speed of the clouds
	float cloud1Speed = 0.0f;
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	// timer for the game
	Clock clock;

	// time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	//
	bool paused = true;

	/***** Generate Texts ****/
	int score = 0;
	Text messageText;
	Text scoreText;

	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");
	messageText.setFont(font);
	scoreText.setFont(font);

	messageText.setString("Press Enter to Start");
	scoreText.setString("Score = 0");

	// set the size of the texts
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	
	// set the color of the texts
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	// set the positions of the texts
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	/**** Generate the Branches ****/
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	for (int i = 0; i < NUM_BRANCHES; ++i) {
		branches[i].setTexture(textureBranch);
		branches[1].setPosition(-2000, -2000);

		branches[i].setOrigin(220, 20);
	}

	/**** Generate Player ****/
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	// The player starts from the left side
	side playerSide = side::LEFT;

	/**** Generate the Grave Stone ****/
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	/**** Generate the axe ****/
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	// align the axe along with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	/**** Generate Flying Log ****/
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	// variables about the log
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// 
	bool acceptInput = false;

	/**** Provide the sound effects ****/
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);


	while (window.isOpen()) {
		/**** Keyboard Listener ****/
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				//
				acceptInput = true;

				// hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			} 
		}

		// close the game once the user presses Esc
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			window.close();
		}

		// start the game once the user presses enter
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// reset the time and score
			score = 0;
			timeRemaining = 5;

			// hide all branches at the beginning of the game
			for (int i = 0; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			// hide the grave stone
			spriteRIP.setPosition(675, 2000);

			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		if (acceptInput) {
			/**** When the user presses RIGHT key ****/
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				playerSide = side::RIGHT;
				score++;

				// reward some time to the player
				timeRemaining += (2 / score) + 0.15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);

				// update the branches
				updateBranches(score);

				// set the log
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				chop.play();
			}
			/**** When the user presses LEFT key ****/
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				playerSide = side::LEFT;
				score++;

				// reward some time to the player
				timeRemaining += (2 / score) + 0.15;

				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);

				// update the branches
				updateBranches(score);

				// set the log
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				chop.play();
			}
		}

		if (!paused) {
			// 
			Time dt = clock.restart();

			/**** Adjust the size of the time bar ****/
			timeRemaining -= dt.asSeconds();
			timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {
				paused = true;
				messageText.setString("Time has run out!");
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				//

				outOfTime.play();
			}

			/**** Moving the Bee ****/
			if (!beeActive) {
				// generate the speed of the bee
				srand((int)time(0) * 10);
				beeSpeed = (rand() % 200) + 200;
				// generate the height of the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			else {
				spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);
				// if the bee has reached the right end of the screen
				if (spriteBee.getPosition().x < -100) {
					beeActive = false;
				}
			}

			/**** Move the cloud ****/
			if (!cloud1Active) {
				// speed
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);
				// height
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteClouds[0].setPosition(-200, height);
				cloud1Active = true;
			}
			else {
				spriteClouds[0].setPosition(spriteClouds[0].getPosition().x + (cloud1Speed * dt.asSeconds()), spriteClouds[0].getPosition().y);
				if (spriteClouds[0].getPosition().x > 1920) {
					cloud1Active = false;
				}
			}
			if (!cloud2Active) {
				// speed
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);
				// height
				srand((int)time(0) * 20);
				float height = (rand() % 150);
				spriteClouds[1].setPosition(-200, height);
				cloud2Active = true;
			}
			else {
				spriteClouds[1].setPosition(spriteClouds[1].getPosition().x + (cloud2Speed * dt.asSeconds()), spriteClouds[1].getPosition().y);
				if (spriteClouds[1].getPosition().x > 1920) {
					cloud2Active = false;
				}
			}
			if (!cloud3Active) {
				// speed
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);
				// height
				srand((int)time(0) * 30);
				float height = (rand() % 150);
				spriteClouds[2].setPosition(-200, height);
				cloud3Active = true;
			}
			else {
				spriteClouds[2].setPosition(spriteClouds[2].getPosition().x + (cloud3Speed * dt.asSeconds()), spriteClouds[2].getPosition().y);
				if (spriteClouds[2].getPosition().x > 1920) {
					cloud3Active = false;
				}
			}

			// update the score
			// TODO: Use plain strings
			stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPositions[i] == side::LEFT) {
					branches[i].setPosition(610, height);
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT) {
					branches[i].setPosition(1330, height);
					branches[i].setRotation(0);
				}
				else {
					// hide the branch
					branches[i].setPosition(3000, height);
				}
			}
			
			// update the position of the log			
			if (logActive) {
				spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()), 
									  spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));
				// if the log has gone out of the screen
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
					// generate the next log
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			/**** Game Over Case Handling****/
			if (playerSide == branchPositions[5]) {
				paused = true;
				acceptInput = false;

				spriteRIP.setPosition(525, 760);

				// hide the player
				spritePlayer.setPosition(2000, 660);

				messageText.setString("Player is Squished!");
				
				
				
				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);

				death.play();

			}

		} // END OF IF PAUSED

		/**** Drawing the scenes ****/

		// clear all the scenes in the last frame
		window.clear();

		// draw the background
		window.draw(spriteBackground);

		// draw the clouds
		for (int i = 0; i < 3; i++) {
			window.draw(spriteClouds[i]);
		}

		// draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		// draw the tree
		window.draw(spriteTree);

		// draw the player
		window.draw(spritePlayer);

		// draw the axe
		window.draw(spriteAxe);

		// draw the flying log
		window.draw(spriteLog);

		// draw the grave stone
		window.draw(spriteRIP);

		// draw the bee
		window.draw(spriteBee);

		// draw the score
		window.draw(scoreText);

		// draw the time bar
		window.draw(timeBar);

		// draw message text
		if (paused) {
			window.draw(messageText);
		}

		/**** Show the content drawn ****/
		window.display();
	}

    return 0;
}


#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Collision.h"
#include "DrawQ.cpp"
#include <vector>
#include "VectorMath.cpp"
#include <iostream> //for sprintf

int main(int argc, char *argv[]){
	bool DEBUG_ENABLED = false;
	if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'd'){
		DEBUG_ENABLED = true;
	}
	
	//open the game window
	const int SCREEN_X = 300;
	const int SCREEN_Y = 400;
	sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y),\
			"Every Pinball is Water");
	
	window.setFramerateLimit(120);
	Collision::DEBUG_ENABLED = DEBUG_ENABLED;
	Collision::DEBUG_WINDOW = &window;
	
	DrawQ drawQ;
	std::vector<sf::ConvexShape> terrainList;
	
	sf::CircleShape bgCircle;
	bgCircle.setRadius(100.0f);
	bgCircle.setFillColor(sf::Color::Green);
	bgCircle.setOrigin(100,100);
	bgCircle.setPosition(SCREEN_X/2.0f,SCREEN_Y/2.0f);
	drawQ.add(bgCircle, DrawLayers::background);
	
	sf::CircleShape bg2Circle;
	bg2Circle.setRadius(75.0f);
	bg2Circle.setFillColor(sf::Color::Blue);
	bg2Circle.setOrigin(75,75);
	bg2Circle.setPosition(SCREEN_X/2.0f,SCREEN_Y/2.0f);
	drawQ.add(bg2Circle, DrawLayers::background, 1);
	
	sf::CircleShape ball;
	ball.setRadius(10);
	ball.setOrigin(10, 10);
	ball.setFillColor(sf::Color::Red);
	ball.setPosition(SCREEN_X*0.25f, 0);
	drawQ.add(ball, DrawLayers::stage, 10);
	
	sf::ConvexShape blGuard;
	blGuard.setPointCount(5);
	blGuard.setPoint(0, sf::Vector2f(0,0));
	blGuard.setPoint(1, sf::Vector2f(0,85));
	//blGuard.setPoint(2, sf::Vector2f(130,85));
	blGuard.setPoint(2, sf::Vector2f(160,85));
	blGuard.setPoint(3, sf::Vector2f(115,65));
	blGuard.setPoint(4, sf::Vector2f(20, 5));
	blGuard.setOrigin(5,80);
	//blGuard.setPosition(5,SCREEN_Y-5);
	blGuard.setPosition(5,SCREEN_Y-100);
	blGuard.setFillColor(sf::Color::Green);
	blGuard.setOutlineColor(sf::Color::Blue);
	blGuard.setOutlineThickness(2);
	drawQ.add(blGuard, DrawLayers::stage);
	terrainList.push_back(blGuard);
	
	sf::ConvexShape ramp;
	ramp.setPointCount(4);
	ramp.setPoint(0, sf::Vector2f(160, 85));
	ramp.setPoint(1, sf::Vector2f(260, -50));
	ramp.setPoint(2, sf::Vector2f(130, 65));
	ramp.setPoint(3, sf::Vector2f(115, 65));
	ramp.setFillColor(sf::Color::Red);
	ramp.setOutlineColor(sf::Color::Yellow);
	ramp.setOutlineThickness(2);
	ramp.setPosition(5, SCREEN_Y-180);
	drawQ.add(ramp, DrawLayers::stage);
	terrainList.push_back(ramp);
	
	
	sf::ConvexShape brGuard;
	brGuard.setPointCount(5);
	brGuard.setPoint(0, sf::Vector2f(0,85));
	brGuard.setPoint(1, sf::Vector2f(130,85));
	brGuard.setPoint(2, sf::Vector2f(130,0));
	brGuard.setPoint(3, sf::Vector2f(110,5));
	brGuard.setPoint(4, sf::Vector2f(15, 5)); //15, 65
	brGuard.setOrigin(125,80);
	brGuard.setPosition(SCREEN_X-5,SCREEN_Y-5);
	brGuard.setFillColor(sf::Color::Green);
	brGuard.setOutlineColor(sf::Color::Blue);
	brGuard.setOutlineThickness(2);
	drawQ.add(brGuard, DrawLayers::stage);
	terrainList.push_back(brGuard);
	
	sf::Text velText;
	sf::Font titillium;
	//initialize debug items
	if (DEBUG_ENABLED){
		
		if (!titillium.loadFromFile("Assets/Titillium/Titillium-Regular.otf")){
			std::cout << "Error: Could not read text file.\n";
			return 1;
		}
		velText.setFont(titillium);
		velText.setString("[Velocity not set]");
		velText.setCharacterSize(15);
		velText.setFillColor(sf::Color::White);
	}
	
	//#############################
	
	sf::Vector2f gravity = sf::Vector2f(0.0f, 0.01f);
	sf::Vector2f ballVelocity;
	sf::Vector2f prevBallPos;

	
	bool freeze = false;//used in debugging; freezes position of ball
	
	while (window.isOpen()){
		//drawing is done FIRST:
		//screen is technically one frame behind stored values. creates perceived input delay.
		//temporary(?) measure, allows other drawers (like debug gizmos) to draw on top of things.
		//need to improve drawing Q to fix this (allow it to hold line/vertex-array entries)
		//could also potentially just force debug gizmos onto another window
		
		window.clear();
		//draw things
		drawQ.drawToWindow(window);
		
		//move ball
		prevBallPos = ball.getPosition();
		if (!freeze)ball.move(ballVelocity);
		
		//apply gravity
		ballVelocity += gravity;
		
		ball.setFillColor(sf::Color::Red);

		//DEBUG: display ball velocity
		if (DEBUG_ENABLED){
			char charBuffer[100];
			snprintf(charBuffer, sizeof(charBuffer), "%4.3f, %4.3f", ballVelocity.x, ballVelocity.y);
			velText.setString(charBuffer);
			velText.setPosition(1, 1);
			window.draw(velText);
		}
		
		
		// check for collisions between terrain and ball
		for (int i=0; i<terrainList.size(); i++){
			Collision::ColData colData;
			if (Collision::isShapeCollidingWithCircle(terrainList[i], ball, colData)){
				//collided with wall:
				//snap ball to a legal position (outside terrain)
				sf::Vector2f ballPos = ball.getPosition();				
				ball.setPosition(colData.POI + (normalize(colData.normal) * (ball.getRadius() + 0.1f)));
				
				//cancel all of ball's momentum normal to surface: inelastic collision
				ballVelocity -= project(ballVelocity, normalize(colData.normal));
			}
		}
		
		
		
		//check input
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma)){
			//left flipper
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period)){
			//right flipper
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
			//hold ball launcher
		}


		//wait for space to be pressed before continuing
		bool passFrame = false;
		do {
			//check for closing
			sf::Event event;
			while (window.pollEvent(event)){
				if (event.type == sf::Event::Closed)
					window.close();
				else if (event.type == sf::Event::KeyPressed){
					if (event.key.code == sf::Keyboard::Right)
						passFrame = true;
					else if (event.key.code == sf::Keyboard::Escape)
						window.close();
				}
			}
		} while(DEBUG_ENABLED && !(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || passFrame) && window.isOpen());
		
		window.display();
	}
	
	return 0;
}

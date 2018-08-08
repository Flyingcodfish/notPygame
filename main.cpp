#include <SFML/Graphics.hpp>
#include <SFML/Keyboard.hpp>
#include <pinball.h>
#include <vector>

int main(){
	//open the game window
	sf::RenderWindow window(sf::VideoMode(200, 250), "Every Pinball is Water");
	std::vector<sf::Drawable> List;	//TODO write code to insert elements using sorting layers
	
	
	while (window.isOpen()){
		
		if (GAMESTATE == 1){
			
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
		}

		//check for closing
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close(); //bye ;-;
		
		//draw things
		window.clear();
		
		
		window.display();
	
	}
	
	return 0;
}

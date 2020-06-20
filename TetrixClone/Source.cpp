/*
Modified from OneLoneCoder's tutorial
https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_Tetris.cpp
*/
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <iostream>
#include <string>


int fieldHeight = 22;
int fieldWidth = 12;

int tileSize = 36;
int screenHeight = 792;
int screenWidth = 432;

int main() {
	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "");
	int *gameBoard = new int[fieldHeight * fieldWidth];
	std::vector<sf::RectangleShape> tilesArray (fieldHeight * fieldWidth);

	sf::RectangleShape blankTile(sf::Vector2f(tileSize, tileSize));
	blankTile.setFillColor(sf::Color::Black);

	sf::RectangleShape borderTile(sf::Vector2f(tileSize, tileSize));
	borderTile.setFillColor(sf::Color::White);

	std::vector<sf::RectangleShape> tilesType = { blankTile, borderTile };

	for (int y = 0; y < fieldHeight; y++) {
		for (int x = 0; x < fieldWidth; x++) {
			//marking wall tiles by index, and all tiles can be loaded onto an array and accessed by the index
			gameBoard[y * fieldWidth + x] = (y == fieldHeight - 1 || x == 0 || x == fieldWidth - 1) ? 1 : 0;
		}
	}

	for (int y = 0; y < fieldHeight; y++) {
		for (int x = 0; x < fieldWidth; x++) {
			//tilesArray.insert(tilesArray.begin, tilesType[gameBoard[i]]);
			float xCoord = tileSize * x;
			float yCoord = tileSize * y;
			
			sf::RectangleShape tile = tilesType[gameBoard[y * fieldWidth + x]];
			tile.setPosition(xCoord, yCoord);
			tilesArray.insert(tilesArray.begin(), tile);
		}
	}

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		for (sf::RectangleShape shape : tilesArray) {
			window.draw(shape);
		}
		
		//window.clear();
		window.display();
	}

}
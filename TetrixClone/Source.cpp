/*
Modified from OneLoneCoder's tutorial
https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_Tetris.cpp
*/
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#define BORDER_COLOR 8
#define BLANK_COLOR 0
#define TETROMINO_VECTOR_WIDTH 4

int fieldHeight = 22;
int fieldWidth = 12;
int *gameBoard = new int[fieldHeight * fieldWidth];

int tileSize = 36;
int screenHeight = 792;
int screenWidth = 432;

enum TETROMINO {
	STRAIGHT, LSHAPE_RIGHT, LSHAPE_LEFT, SQUARE, Z, T, Z_REVERSE
};



std::vector<sf::Color> colorArray = { sf::Color(120, 120, 120), //Grey
										sf::Color(220, 20, 60), //Pink
										sf::Color::Blue,
										sf::Color(255, 127, 80), //Orange
										sf::Color(255, 140, 32), //Yellow
										sf::Color::Green,
										sf::Color(138, 43, 226), //Purple
										sf::Color(220, 0, 0), //Red
										sf::Color::Transparent };

std::vector<int> getTetrominoVector(TETROMINO tetromino) {
	std::vector<int> tetrominoVector(16);
	switch (tetromino) {
	case STRAIGHT:
		tetrominoVector = { 0, 0, 0, 0,
							1, 1, 1, 1,
							0, 0, 0, 0,
							0, 0, 0, 0 };
		break;
	case LSHAPE_RIGHT:
		tetrominoVector = { 0, 2, 0, 0,
							0, 2, 2, 2,
							0, 0, 0, 0,
							0, 0, 0, 0 };
		break;
	case LSHAPE_LEFT:
		tetrominoVector = { 0, 0, 3, 0,
							3, 3, 3, 0,
							0, 0, 0, 0,
							0, 0, 0, 0 };
		break;
	case SQUARE:
		tetrominoVector = { 0, 0, 0, 0,
							0, 4, 4, 0,
							0, 4, 4, 0,
							0, 0, 0, 0 };
		break;
	case Z:
		tetrominoVector = { 5, 5, 0, 0,
							0, 5, 5, 0,
							0, 0, 0, 0,
							0, 0, 0, 0 };
		break;
	case Z_REVERSE:
		tetrominoVector = { 0, 0, 6, 6,
							0, 6, 6, 0,
							0, 0, 0, 0,
							0, 0, 0, 0 };
		break;
	case T:
		tetrominoVector = { 0, 0, 7, 0,
							0, 7, 7, 7,
							0, 0, 0, 0,
							0, 0, 0, 0 };
		break;
	}
	return tetrominoVector;
}


//Translate the index of original tetrominoVector to the index of rotatedVector
//TODO: Figure out better method of translation where you pivot around a central tile (By changing the tetrominoVector size to be 5x5?)
int rotateTetrominoVectorIndex(int x, int y, int rotation) {
	int tetrominoVectorIndex;
	switch (rotation) {
		case 0:
			tetrominoVectorIndex = y * TETROMINO_VECTOR_WIDTH + x;
			break;
		case 1: //90 degrees
			tetrominoVectorIndex = 12 + y - x * TETROMINO_VECTOR_WIDTH;
			break;
		case 2: // 180 degrees
			tetrominoVectorIndex = 15 - y * TETROMINO_VECTOR_WIDTH - x;
			break;
		case 3: // 270 degrees
			tetrominoVectorIndex = 3 - y + (x * 4);
			break;
		default:
			tetrominoVectorIndex = y * TETROMINO_VECTOR_WIDTH + x;
			break;
	}
	return tetrominoVectorIndex;
}

bool collisionCheck(std::vector<int> tetrominoVector, int xBoard, int yBoard, int rotation) {
	for (int xTetrix = 0; xTetrix < TETROMINO_VECTOR_WIDTH; xTetrix++) {
		for (int yTetrix = 0; yTetrix < TETROMINO_VECTOR_WIDTH; yTetrix++) {
			int tetrominoPosOnBoard = (yBoard + yTetrix) * fieldWidth + (xBoard + xTetrix);

			if (xBoard + xTetrix >= 0 && xBoard + xTetrix < fieldWidth) { //check out of bound on x-axis
				if (yBoard + yTetrix >= 0 && yBoard + yTetrix < fieldHeight) { //check out of bound on y-axis
					if (tetrominoVector[rotateTetrominoVectorIndex(xTetrix, yTetrix, rotation)] != 0 && gameBoard[tetrominoPosOnBoard] == BORDER_COLOR) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "");
	std::vector<sf::RectangleShape> tilesArray (fieldHeight * fieldWidth);

	//Array for all variations of tiles
	std::vector<sf::RectangleShape> tilesType;
	for (int i = 0; i < colorArray.size(); i++) {
		sf::RectangleShape tile = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
		tile.setFillColor(colorArray[i]);
		tile.setOutlineThickness(1);
		tile.setOutlineColor(sf::Color::Black);
		tilesType.insert(tilesType.end(), tile);
	}

	int input[4];
	int currentX = 2;
	int currentY = 0;
	int currentRotation = 0;
	int fallingSpeed = 50;
	sf::Event event;

	TETROMINO currentPiece = TETROMINO::STRAIGHT;

	while (window.isOpen()) {
		window.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(fallingSpeed));
			
		//get input - timing is not right
		for (int i = 0; i < 4; i++) {
			input[i] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[i]))) != 0; //R L D Z
		}

		//draw gameboard & get current piece
		for (int y = 0; y < fieldHeight; y++) {
			for (int x = 0; x < fieldWidth; x++) {
				//marking wall tiles by index, and all tiles can be loaded onto an array and accessed by the index
				gameBoard[y * fieldWidth + x] = (y == fieldHeight - 1 || x == 0 || x == fieldWidth - 1) ? BORDER_COLOR : 0;
			}
		}
		std::vector<int> tetrominoVector = getTetrominoVector(currentPiece);

		currentX += (input[0] && collisionCheck(tetrominoVector, currentX + 1, currentY, currentRotation) ) ? 1 : 0;
		currentX -= (input[1] && collisionCheck(tetrominoVector, currentX - 1, currentY, currentRotation)) ? 1 : 0;
		currentY += (input[2] && collisionCheck(tetrominoVector, currentX, currentY + 1, currentRotation)) ? 1 : 0;
		currentRotation += (input[3] && collisionCheck(tetrominoVector, currentX, currentY, currentRotation + 1)) ? 1 : 0;
		currentRotation = currentRotation % 4;

		//draw current tetromino piece
		for (int yTetrix = 0; yTetrix < TETROMINO_VECTOR_WIDTH; yTetrix++) {
			for (int xTetrix = 0; xTetrix < TETROMINO_VECTOR_WIDTH; xTetrix++) {
				int tetrominoPosOnBoard = (currentY + yTetrix) * fieldWidth + (currentX + xTetrix);
				if (gameBoard[tetrominoPosOnBoard] == 0)
					gameBoard[tetrominoPosOnBoard] = tetrominoVector[rotateTetrominoVectorIndex(xTetrix, yTetrix, currentRotation)];
			}
		}

		//drawTetromino(gameBoard, TETROMINO::T, currentX, currentY, 0); 

		tilesArray.clear();
		//Rendering the pieces 
		for (int y = 0; y < fieldHeight; y++) {
			for (int x = 0; x < fieldWidth; x++) {
				float xCoord = tileSize * x;
				float yCoord = tileSize * y;

				sf::RectangleShape tile = tilesType[gameBoard[y * fieldWidth + x]];
				tile.setPosition(xCoord, yCoord);
				tilesArray.insert(tilesArray.end(), tile);
			}
		}

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
		}

		for (sf::RectangleShape shape : tilesArray) {
			window.draw(shape);
		}
		
		window.display();
	}
}
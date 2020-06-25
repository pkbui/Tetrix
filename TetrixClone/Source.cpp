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
#include <time.h>

#define BORDER_TILE 8
#define BLANK_TILE 0
#define TETROMINO_MATRIX_WIDTH 5

//TODO: Check for game ending, by reorganize logic for checking next piece

int fieldHeight = 22;
int fieldWidth = 12;
int *gameBoard = new int[fieldHeight * fieldWidth];


int tileSize = 36;
int screenHeight = 792;
int screenWidth = 432;

enum TETROMINO {
	STRAIGHT, LSHAPE_RIGHT, LSHAPE_LEFT, SQUARE, Z, T, Z_REVERSE, END
};

TETROMINO currentPiece;

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
	std::vector<int> tetrominoVector(TETROMINO_MATRIX_WIDTH * TETROMINO_MATRIX_WIDTH);
	switch (tetromino) {
	case STRAIGHT:
		tetrominoVector = { 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0,
							1, 1, 1, 1, 0,
							0, 0, 0, 0, 0,
							0, 0, 0, 0, 0};
		break;
	case LSHAPE_RIGHT:
		tetrominoVector = { 0, 0, 0, 0, 0,
							0, 2, 0, 0, 0,
							0, 2, 2, 2, 0,
							0, 0, 0, 0, 0,
							0, 0, 0, 0, 0};
		break;
	case LSHAPE_LEFT:
		tetrominoVector = { 0, 0, 0, 0, 0,
							0, 0, 0, 3, 0,
							0, 3, 3, 3, 0,
							0, 0, 0, 0, 0,
							0, 0, 0, 0, 0};
		break;
	case SQUARE:
		tetrominoVector = { 0, 0, 0, 0, 0,
							0, 4, 4, 0, 0,
							0, 4, 4, 0, 0,
							0, 0, 0, 0, 0,
							0, 0, 0, 0, 0};
		break;
	case Z:
		tetrominoVector = { 0, 0, 0, 0, 0,
							0, 5, 5, 0, 0,
							0, 0, 5, 5, 0,
							0, 0, 0, 0, 0,
							0, 0, 0, 0, 0};
		break;
	case Z_REVERSE:
		tetrominoVector = { 0, 0, 0, 0, 0,
							0, 0, 6, 6, 0,
							0, 6, 6, 0, 0,
							0, 0, 0, 0, 0,
							0, 0, 0, 0, 0};
		break;
	case T:
		tetrominoVector = { 0, 0, 0, 0, 0,
							0, 0, 7, 0, 0,
							0, 7, 7, 7, 0, 
							0, 0, 0, 0, 0,
							0, 0, 0, 0, 0};
		break;
	}
	return tetrominoVector;
}


//Translate the index of original tetrominoVector to the index of rotatedVector
int rotateTetrominoVectorIndex(int x, int y, int rotation) {
	if (currentPiece == TETROMINO::SQUARE) rotation = 0;
	int tetrominoVectorIndex;
	switch (rotation) {
		case 0:
			tetrominoVectorIndex = y * TETROMINO_MATRIX_WIDTH + x;
			break;
		case 1: //90 degrees
			tetrominoVectorIndex = TETROMINO_MATRIX_WIDTH * (TETROMINO_MATRIX_WIDTH-1) + y - (x * TETROMINO_MATRIX_WIDTH);
			break;
		case 2: // 180 degrees
			tetrominoVectorIndex = (TETROMINO_MATRIX_WIDTH * TETROMINO_MATRIX_WIDTH) - 1 - (y * TETROMINO_MATRIX_WIDTH) - x;
			break;
		case 3: // 270 degrees
			tetrominoVectorIndex = (TETROMINO_MATRIX_WIDTH - 1) - y + (x * TETROMINO_MATRIX_WIDTH);
			break;
		default: //if mistakes happen
			tetrominoVectorIndex = y * TETROMINO_MATRIX_WIDTH + x;
			break;
	}
	return tetrominoVectorIndex;
}

bool collisionPass(std::vector<int> tetrominoVector, int xBoard, int yBoard, int rotation) {
	for (int xTetrix = 0; xTetrix < TETROMINO_MATRIX_WIDTH; xTetrix++) {
		for (int yTetrix = 0; yTetrix < TETROMINO_MATRIX_WIDTH; yTetrix++) {
			int tetrominoPosOnBoard = (yBoard + yTetrix) * fieldWidth + (xBoard + xTetrix);

			if (xBoard + xTetrix >= 0 && xBoard + xTetrix < fieldWidth) { //check out of bound on x-axis
				if (yBoard + yTetrix >= 0 && yBoard + yTetrix < fieldHeight) { //check out of bound on y-axis
					if (tetrominoVector[rotateTetrominoVectorIndex(xTetrix, yTetrix, rotation)] != 0 && gameBoard[tetrominoPosOnBoard] != BLANK_TILE) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

void clearFullLine() {
	for (int y = 0; y < fieldHeight - 1; y++) {
		bool lineCleared = true;
		for (int x = 1; x < fieldWidth - 1; x++) 
			lineCleared = lineCleared && gameBoard[y * fieldWidth + x] != BLANK_TILE;

		if (lineCleared) {
			for (int boardY = y; boardY >= 0; boardY--) {
				for (int boardX = 0; boardX < fieldWidth; boardX++) {
					if (boardY == 0)
						gameBoard[boardY * fieldWidth + boardX] = (boardY == fieldHeight - 1 || boardX == 0 || boardX == fieldWidth - 1) ? BORDER_TILE : BLANK_TILE;
					else
						gameBoard[boardY * fieldWidth + boardX] = gameBoard[(boardY - 1) * fieldWidth + boardX];
				}
			}
		}
	}
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
	srand(time(NULL));
	currentPiece = static_cast<TETROMINO>(rand() % END);
	int currentX = fieldWidth / 2;
	int currentY = 0;
	int currentRotation = 0;
	int sleepMs = 50;
	int fallingMs = 800;
	int hangingMs = 0;
	bool rotatingHold = false;
	bool nextPiece = false;
	bool gameOver = false;

	sf::Event event;

	//draw gameboard 
	for (int y = 0; y < fieldHeight; y++) {
		for (int x = 0; x < fieldWidth; x++) {
			//marking wall tiles by index, and all tiles can be loaded onto an array and accessed by the index
			gameBoard[y * fieldWidth + x] = (y == fieldHeight - 1 || x == 0 || x == fieldWidth - 1) ? BORDER_TILE : BLANK_TILE;
		}
	}

	////test for game end
	//for (int y = 0; y < fieldHeight; y++) {
	//	for (int x = 0; x < fieldWidth; x++) {
	//		//marking wall tiles by index, and all tiles can be loaded onto an array and accessed by the index
	//		gameBoard[y * fieldWidth + x] = (y != fieldHeight - 1 && x > 0 && x < fieldWidth - 2 ) ? 2 : gameBoard[y * fieldWidth + x];
	//	}
	//}

	while (window.isOpen() && !gameOver) {
		window.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
			
		//get input & current pieces
		std::vector<int> tetrominoVector = getTetrominoVector(currentPiece);
		
		for (int i = 0; i < 4; i++) {
			input[i] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[i]))) != 0; //R L D Z
		}

		currentX += (input[0] && collisionPass(tetrominoVector, currentX + 1, currentY, currentRotation) ) ? 1 : 0;
		currentX -= (input[1] && collisionPass(tetrominoVector, currentX - 1, currentY, currentRotation)) ? 1 : 0;
		currentY += (input[2] && collisionPass(tetrominoVector, currentX, currentY + 1, currentRotation)) ? 1 : 0;

		if (input[3]) {
			bool collisionCheckB = collisionPass(tetrominoVector, currentX, currentY, currentRotation + 1);
			currentRotation += (!rotatingHold && collisionCheckB) ? 1 : 0;
			rotatingHold = true;
		}
		else
			rotatingHold = false;
		currentRotation = currentRotation % 4; //reset rotation at 360 degrees

		//push pieces down after falling ms
		hangingMs = (hangingMs + sleepMs) % fallingMs;
		if (hangingMs == 0) {
			int tempY = currentY;
			//TODO: there's a problem that cause collisionPass to return false - it seems that the collisionCheck find collision between the piece of current frame and last frame
			currentY += collisionPass(tetrominoVector, currentX, currentY + 1, currentRotation) ? 1 : 0;

			if (tempY == currentY) { //pieces are at the bottom
				nextPiece = true;
			}
		}

		//draw current tetromino piece into game board
		for (int yTetrix = 0; yTetrix < TETROMINO_MATRIX_WIDTH; yTetrix++) {
			for (int xTetrix = 0; xTetrix < TETROMINO_MATRIX_WIDTH; xTetrix++) {
				int tetrominoPosOnBoard = (currentY + yTetrix) * fieldWidth + (currentX + xTetrix);
				if (gameBoard[tetrominoPosOnBoard] == 0)
					gameBoard[tetrominoPosOnBoard] = tetrominoVector[rotateTetrominoVectorIndex(xTetrix, yTetrix, currentRotation)];
			}
		}

		//Convert gameboard to tiles 
		tilesArray.clear();
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

		//clear this iteration's piece
		if (!nextPiece) {
			for (int yTetrix = 0; yTetrix < TETROMINO_MATRIX_WIDTH; yTetrix++) {
				for (int xTetrix = 0; xTetrix < TETROMINO_MATRIX_WIDTH; xTetrix++) {
					int tetrominoPosOnBoard = (currentY + yTetrix) * fieldWidth + (currentX + xTetrix);
					if (tetrominoVector[rotateTetrominoVectorIndex(xTetrix, yTetrix, currentRotation)] != 0) {
						gameBoard[tetrominoPosOnBoard] = BLANK_TILE;
					}
				}
			}
		}

		else { 
			//check for full line and clear them
			clearFullLine();
			//span new pieces 
			currentPiece = static_cast<TETROMINO>(rand() % END);
			currentX = fieldWidth / 2;
			currentY = 0;
			currentRotation = 0;
			hangingMs = 0;
			//if pieces collide right away, game over
			gameOver = !collisionPass(getTetrominoVector(currentPiece), currentX, currentY, currentRotation);
			nextPiece = false;
		}

	}
}
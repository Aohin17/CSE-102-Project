#include "iGraphics.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_WIDTH 3
#define GRID_HEIGHT 7
#define BLOCK_SIZE 80
#define MAIN_GRID_OFFSET_X ((SCREEN_WIDTH - (BLOCK_SIZE * GRID_WIDTH)) / 2)
#define MAIN_GRID_OFFSET_Y 50
#define NEXT_COLOR_BOX_X 350
#define NEXT_COLOR_BOX_Y 450
#define NEXT_COLOR_BOX_SIZE 60
#define TEXT_OFFSET_X 245
#define TEXT_OFFSET_Y 100


int mainGrid[GRID_WIDTH][GRID_HEIGHT] = {0};
int nextColor = 0;
int fallingBlockX = GRID_WIDTH / 2;
int fallingBlockY = GRID_HEIGHT - 1;
int score = 0;
int highScore = 0;
int gameStarted = 0; 
int gameOver = 0;    
float lastTime = 0;
float gravityTimer = 0;
float gravityInterval = 1.0;

void loadHighScore();
void saveHighScore();
void generateNextColor();
void drawBlock(int x, int y, int color);
void drawNextColorBox();
void drawScore();
void drawHighScore();
void moveBlocksDown();
void updateGame();
void updateHighScore();

void loadHighScore() {
    FILE *file = fopen("highscore.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d", &highScore);
        fclose(file);
    }
}

void saveHighScore() {
    FILE *file = fopen("highscore.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d", highScore);
        fclose(file);
    }
}

void generateNextColor() {
    nextColor = (rand() % 6) + 1;
}

void drawBlock(int x, int y, int color) {
    int red, green, blue;
    
    switch (color) {
        case 1:
            red = 255;
            green = 0;
            blue = 0;
            break;
        case 2:
            red = 0;
            green = 255;
            blue = 0;
            break;
        case 3:
            red = 0;
            green = 0;
            blue = 255;
            break;
        case 4:
            red = 255;
            green = 255;
            blue = 0; 
            break;
        case 5:
            red = 255;
            green = 165; 
            blue = 0;
            break;
        case 6:
            red = 128;
            green = 0;
            blue = 128; 
            break;
        default:
            red = 255;
            green = 255;
            blue = 255;
            break;
    }
    
    iSetColor(red, green, blue);
    iFilledRectangle(x * BLOCK_SIZE + MAIN_GRID_OFFSET_X, y * BLOCK_SIZE + MAIN_GRID_OFFSET_Y, BLOCK_SIZE, BLOCK_SIZE);
    iSetColor(0, 0, 0);
    iRectangle(x * BLOCK_SIZE + MAIN_GRID_OFFSET_X, y * BLOCK_SIZE + MAIN_GRID_OFFSET_Y, BLOCK_SIZE, BLOCK_SIZE);
}

void drawNextColorBox() {
    iSetColor(255, 255, 255);
    drawBlock((NEXT_COLOR_BOX_X + NEXT_COLOR_BOX_SIZE / 4) / BLOCK_SIZE, NEXT_COLOR_BOX_Y / BLOCK_SIZE, nextColor);
    iSetColor(255, 255, 255);
    iText(NEXT_COLOR_BOX_X + TEXT_OFFSET_X, NEXT_COLOR_BOX_Y + TEXT_OFFSET_Y, "New Color:", GLUT_BITMAP_HELVETICA_18);
}

void drawScore() {
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    iSetColor(255, 255, 255);
    iText(SCREEN_WIDTH / 2 - 40, 16, scoreText, GLUT_BITMAP_HELVETICA_18);
}

void drawHighScore() {
    char highScoreText[20];
    sprintf(highScoreText, "Previous High Score: %d", highScore);
    iSetColor(255, 255, 255);
    iText(SCREEN_WIDTH / 2 - 100, 36, highScoreText, GLUT_BITMAP_HELVETICA_18);
}



void iDraw() {
    iClear();
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            iSetColor(255, 255, 255);
            iFilledRectangle(i * BLOCK_SIZE + MAIN_GRID_OFFSET_X, j * BLOCK_SIZE + MAIN_GRID_OFFSET_Y, BLOCK_SIZE, BLOCK_SIZE);
            iSetColor(0, 0, 0);
            iRectangle(i * BLOCK_SIZE + MAIN_GRID_OFFSET_X, j * BLOCK_SIZE + MAIN_GRID_OFFSET_Y, BLOCK_SIZE, BLOCK_SIZE);
            if (mainGrid[i][j] != 0) {
                drawBlock(i, j, mainGrid[i][j]);
            }
        }
    }
    drawBlock(fallingBlockX, fallingBlockY, nextColor);
    drawNextColorBox();
    drawScore();
    drawHighScore();

    if (gameOver == 1) {
        char gameOverText[] = "Game Over!";
        char finalScoreText[20];
        sprintf(finalScoreText, "Final Score: %d", score);
        iSetColor(255, 255, 255);
        iText(80, 340, gameOverText, GLUT_BITMAP_HELVETICA_18);
        iText(75, 300, finalScoreText, GLUT_BITMAP_HELVETICA_18);
    }
}

void moveBlocksDown() {
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 1; j < GRID_HEIGHT; j++) {
            if (mainGrid[i][j] != 0 && mainGrid[i][j - 1] == 0) {
                int currentY = j;
                while (currentY > 0 && mainGrid[i][currentY - 1] == 0) {
                    currentY--;
                }
                mainGrid[i][currentY] = mainGrid[i][j];
                mainGrid[i][j] = 0;
            }
        }
    }
}

void updateGame() {
    if (gameStarted == 0 || gameOver == 1) {
        return;
    }
    
    float currentTime = (float)clock() / CLOCKS_PER_SEC;
    float deltaTime = currentTime - lastTime;
    gravityTimer += deltaTime;
    
    if (gravityTimer >= gravityInterval) {
        lastTime = currentTime;
        gravityTimer = 0;
        
        if (fallingBlockY > 0 && mainGrid[fallingBlockX][fallingBlockY - 1] == 0) {
            fallingBlockY--;
        }
        else {
            mainGrid[fallingBlockX][fallingBlockY] = nextColor;
            int firstRowMiddleColumnFilled = 1;
            for (int j = 0; j < GRID_HEIGHT; j++) {
                if (mainGrid[GRID_WIDTH / 2][j] == 0) {
                    firstRowMiddleColumnFilled = 0;
                    break;
                }
            }
            if (firstRowMiddleColumnFilled == 1) {
                gameOver = 1;
                updateHighScore(); 
                return;
            }
            moveBlocksDown();
            generateNextColor();
            fallingBlockY = GRID_HEIGHT - 1;
            fallingBlockX = GRID_WIDTH / 2;
            for (int j = 0; j < GRID_HEIGHT; j++) {
                for (int i = 0; i < GRID_WIDTH; i++) {
                    if (j > 0 && mainGrid[i][j] != 0 && mainGrid[i][j] == mainGrid[i][j - 1]) {
                        score++;
                        mainGrid[i][j] = 0;
                        mainGrid[i][j - 1] = 0;
                    }
                    if (i > 0 && mainGrid[i][j] != 0 && mainGrid[i][j] == mainGrid[i - 1][j]) {
                        score++;
                        mainGrid[i][j] = 0;
                        mainGrid[i - 1][j] = 0;
                    }
                }
            }
        }
    }
}

void iKeyboard(unsigned char key) {
    if (gameStarted == 0 && gameOver == 0 && key == '\r') {
        gameStarted = 1;
    }
    else if (gameOver == 1 && key == 'r') {
        gameOver = 0;
        score = 0;
        memset(mainGrid, 0, sizeof(mainGrid));
    }
    else if (gameStarted == 1 && gameOver == 0) {
        if (key == 'a' && fallingBlockX > 0 && mainGrid[fallingBlockX - 1][fallingBlockY] == 0) {
            fallingBlockX--;
        }
        else if (key == 'd' && fallingBlockX < GRID_WIDTH - 1 && mainGrid[fallingBlockX + 1][fallingBlockY] == 0) {
            fallingBlockX++;
        }
    }
}

void iSpecialKeyboard(unsigned char key) {
    if (gameStarted == 1 && gameOver == 0) {
        if (key == GLUT_KEY_LEFT && fallingBlockX > 0 && mainGrid[fallingBlockX - 1][fallingBlockY] == 0) {
            fallingBlockX--;
        }
        else if (key == GLUT_KEY_RIGHT && fallingBlockX < GRID_WIDTH - 1 && mainGrid[fallingBlockX + 1][fallingBlockY] == 0) {
            fallingBlockX++;
        }
    }
}

void iMouse(int button, int state, int mx, int my) {
}

void iMouseMove(int mx, int my) {
}

void updateHighScore() {
    if (score > highScore) {
        highScore = score;
        saveHighScore();
    }
}

int main() {
    srand(time(NULL));
    loadHighScore();
    generateNextColor();
    iSetTimer(15, updateGame);
    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Color Matching Game");
    return 0;
}

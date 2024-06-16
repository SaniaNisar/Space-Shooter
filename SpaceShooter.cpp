#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <ctime>
using namespace std;

// Constants for game settings
const int width = 80;
const int height = 20;
const int maxEnemies = 10;
const int maxBullets = 10;

// Function to set text color
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Function to move the cursor to a specific position in the console
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Function to play sound
void playBeep(int freq, int dur) {
    Beep(freq, dur);
}

// Game elements
int playerX, playerY;
int playerLives = 5;
int playerScore = 0;
int enemiesX[maxEnemies], enemiesY[maxEnemies], enemiesActive[maxEnemies];
int bulletsX[maxBullets], bulletsY[maxBullets], bulletsActive[maxBullets];

void drawBorder() {
    setColor(7);
    for (int i = 0; i < width; i++) {
        gotoxy(i, 0); std::cout << "=";
        gotoxy(i, height); std::cout << "=";
    }
    for (int i = 1; i < height; i++) {
        gotoxy(0, i); std::cout << "|";
        gotoxy(width - 1, i); std::cout << "|";
    }
}

void updateInfoPanel() {
    gotoxy(1, height + 1);
    std::cout << "Lives: " << playerLives << " Score: " << playerScore;
}

void drawPlayer() {
    gotoxy(playerX, playerY);
    setColor(14); // Yellow
    std::cout << ">";
}

void erasePlayer() {
    gotoxy(playerX, playerY);
    std::cout << " ";
}

void movePlayer(int dx) {
    erasePlayer();
    playerX += dx;
    if (playerX < 1) playerX = 1;
    if (playerX > width - 2) playerX = width - 2;
    drawPlayer();
}

void drawEnemy(int index) {
    gotoxy(enemiesX[index], enemiesY[index]);
    setColor(12); // Red
    std::cout << "X";
}

void eraseEnemy(int index) {
    gotoxy(enemiesX[index], enemiesY[index]);
    std::cout << " ";
}

void updateEnemies() {
    for (int i = 0; i < maxEnemies; i++) {
        if (enemiesActive[i]) {
            eraseEnemy(i);
            enemiesY[i]++;
            if (enemiesY[i] >= height) {
                enemiesActive[i] = 0;
            }
            else {
                drawEnemy(i);
                if (enemiesX[i] == playerX && enemiesY[i] == playerY) {
                    playerLives--;
                    drawPlayer();
                    playBeep(300, 300);
                    enemiesActive[i] = 0;
                }
            }
        }
    }
}

void drawBullet(int index) {
    gotoxy(bulletsX[index], bulletsY[index]);
    setColor(15); // White
    std::cout << ".";
}

void eraseBullet(int index) {
    gotoxy(bulletsX[index], bulletsY[index]);
    std::cout << " ";
}

void updateBullets() {
    for (int i = 0; i < maxBullets; i++) {
        if (bulletsActive[i]) {
            eraseBullet(i); // Erase bullet from the previous position
            bulletsY[i]--;
            if (bulletsY[i] <= 0) {
                bulletsActive[i] = 0; // Deactivate bullet if it goes off-screen
            }
            else {
                drawBullet(i); // Redraw bullet at new position
                for (int j = 0; j < maxEnemies; j++) {
                    if (enemiesActive[j] && bulletsX[i] == enemiesX[j] && bulletsY[i] == enemiesY[j]) {
                        bulletsActive[i] = 0; // Deactivate the bullet
                        enemiesActive[j] = 0; // Deactivate the enemy hit by the bullet
                        playerScore += 10;
                        playBeep(1000, 50);
                        eraseBullet(i); // Erase the bullet immediately after a hit
                        break; // Break out of the loop once the bullet hits an enemy
                    }
                }
            }
        }
    }
}


void addBullet() {
    for (int i = 0; i < maxBullets; i++) {
        if (!bulletsActive[i]) {
            bulletsX[i] = playerX + 1;
            bulletsY[i] = playerY;
            bulletsActive[i] = 1;
            playBeep(700, 30);
            drawBullet(i);
            break;
        }
    }
}

void addEnemy() {
    for (int i = 0; i < maxEnemies; i++) {
        if (!enemiesActive[i]) {
            enemiesX[i] = rand() % (width - 2) + 1;
            enemiesY[i] = 1;
            enemiesActive[i] = 1;
            drawEnemy(i);
            break;
        }
    }
}

void resetGame() {
    playerX = width / 2; // Center the player horizontally
    playerY = height - 2; // Position player at the bottom
    playerLives = 5; // Reset lives
    playerScore = 0; // Reset score
    memset(enemiesActive, 0, sizeof(enemiesActive)); // Deactivate all enemies
    memset(bulletsActive, 0, sizeof(bulletsActive)); // Deactivate all bullets

    // Optionally, reset the positions of enemies and bullets if needed
    for (int i = 0; i < maxEnemies; i++) {
        enemiesY[i] = 0; // Reset enemy Y positions if needed
    }
    for (int i = 0; i < maxBullets; i++) {
        bulletsY[i] = 0; // Reset bullet Y positions if needed
    }
}

void gameLoop() {
    resetGame();
    playerX = width / 2;
    playerY = height - 2;
    memset(enemiesActive, 0, sizeof(enemiesActive));
    memset(bulletsActive, 0, sizeof(bulletsActive));

    drawBorder();
    drawPlayer();
    updateInfoPanel();

    bool quit = false;
    while (!quit && playerLives > 0) {
        if (_kbhit()) {
            char ch = _getch();
            switch (ch) {
            case 'a': // Move left
                movePlayer(-1);
                break;
            case 'd': // Move right
                movePlayer(1);
                break;
            case ' ': // Fire a bullet
                addBullet();
                break;
            case 'q': // Quit the game
                quit = true;
                break;
            }
        }

        updateBullets();
        updateEnemies();

        if (rand() % 20 == 0) {
            addEnemy();
        }

        updateInfoPanel();
        Sleep(100);
    }

    gotoxy(width / 4, height / 2);
    std::cout << "Game Over! Score: " << playerScore;
    Sleep(3000); // Display the end screen for 3 seconds
}

void displayMenu() {
    setColor(11);
   
    cout << "                                    _____                    _____ _           _           \n"
         << "                                   |   __|___ ___ ___ ___   |   __| |_ ___ ___| |_ ___ ___ \n"
         << "                                   |__   | - |  _|  _| -_|  |__   |  _| - | - |  _| - |  _|\n"
         << "                                   |_____|___|___|___|___|  |_____|_| |___|___|_| |___|_|  \n"
         << "                                         |_|                                            \n";

    cout << "\n\n                                              Welcome to Space Shooter!" << endl;
    cout << "                                              Press '1' to play game\n";
    cout << "                                              Press '2' for instructions\n";
    cout << "                                              Press '0' to exit game\n";
    std::cout << "Select an option: ";
    int choice;
    std::cin >> choice;

    switch (choice) {
    case 1:
        system("cls");
        gameLoop();
        break;
    case 2:
        exit(0);
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
    while (true) {
        system("cls");
        displayMenu();
    }
    return 0;
}

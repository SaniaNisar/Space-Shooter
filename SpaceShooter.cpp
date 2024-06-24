#include <iostream>
#include <fstream>
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

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

void HideCursor();
void gotoxy(int x, int y);

void HideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &cursorInfo);
}

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
int highestScore = 0; // Add highest score variable
int playerLives = 5;
int playerScore = 0;
int enemiesX[maxEnemies], enemiesY[maxEnemies], enemiesActive[maxEnemies];
int bulletsX[maxBullets], bulletsY[maxBullets], bulletsActive[maxBullets];

// Function to load the highest score from a file
int loadHighestScore() {
    FILE* file;
    errno_t err = fopen_s(&file, "highest_score.txt", "r");
    if (err != 0 || file == nullptr) {
        return 0; // Return 0 if the file doesn't exist or can't be opened
    }
    int score;
    fscanf_s(file, "%d", &score);
    fclose(file);
    return score;
}

// Function to save the highest score to a file
void saveHighestScore(int score) {
    FILE* file;
    errno_t err = fopen_s(&file, "highest_score.txt", "w");
    if (err != 0 || file == nullptr) {
        return; // Exit the function if the file can't be opened
    }
    fprintf(file, "%d", score);
    fclose(file);
}

void drawBorder() {
    setColor(7);
    for (int i = 0; i < width; i++) {
        gotoxy(i, 0); cout << "=";
        gotoxy(i, height); cout << "=";
    }
    for (int i = 1; i < height; i++) {
        gotoxy(0, i); cout << "|";
        gotoxy(width - 1, i); cout << "|";
    }
}

void updateInfoPanel() {
    gotoxy(width + 2, 9);
    cout << "   Lives: " << playerLives;
    gotoxy(width + 2, 10);
    cout << "   Score: " << playerScore;
    gotoxy(width + 2, 11);
    cout << "   Highest Score: " << highestScore;
}

void drawPlayer() {
    gotoxy(playerX, playerY);
    setColor(14); // Yellow
    cout << ">";
}

void erasePlayer() {
    gotoxy(playerX, playerY);
    cout << " ";
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
    cout << "X";
}

void eraseEnemy(int index) {
    gotoxy(enemiesX[index], enemiesY[index]);
    cout << " ";
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
    cout << ".";
}

void eraseBullet(int index) {
    gotoxy(bulletsX[index], bulletsY[index]);
    cout << " ";
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

    // Update the highest score if needed
    if (playerScore > highestScore) {
        highestScore = playerScore;
        saveHighestScore(highestScore);
    }

    gotoxy(width / 3, height / 2);
    cout << "Game Over! Score: " << playerScore;
    Sleep(3000); // Display the end screen for 3 seconds
}

void displayMenu() {
    setColor(11);
    gotoxy(13, 6);
    cout << "                       _____                    _____ _           _           \n"
        << "                                   |   __|___ ___ ___ ___   |   __| |_ ___ ___| |_ ___ ___ \n"
        << "                                   |__   | - |  _|  _| -_|  |__   |  _| - | - |  _| - |  _|\n"
        << "                                   |_____|___|___|___|___|  |_____|_| |___|___|_| |___|_|  \n"
        << "                                         |_|                                            \n";

    cout << "\n\n                                              Welcome to Space Shooter!" << endl;
    cout << "                                              Press '1' to play game\n";
    cout << "                                              Press '2' for instructions\n";
    cout << "                                              Press '0' to exit game\n";
    cout << "                                              Select an option: ";
    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        system("cls");
        gameLoop();
        break;
    case 2:
        system("cls");
        setColor(9); // Yellow for instructions
        cout << " \n\n\n\n\n\n\n\n                                                    Instructions:\n"
            << "                                       (1) Use A key to move the spaceship left.\n"
            << "                                       (2) Use D arrow key to move the spaceship right.\n"
            << "                                       (3) Press space to shoot.\n"
            << "                                       (4) Press Q to quit.\n"
            << "                                       Press '1' to play game or anyother key to exit:  ";
        cin >> choice;
        if (choice == 1) {
            system("cls");
            gameLoop();
            break;
        }
        else if (choice == 0) {
            system("cls");
            cout << "\n\n\n\n\n\n\n\n                                You're on your way out Bye Byee Astronaut!!\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
            exit(0);
            break;
        }
        break;
    case 0:
        system("cls");
        setColor(14);
        cout << "\n\n\n\n\n\n\n\n                                You're on your way out Bye Byee Astronaut!!!\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
        exit(0);
        break;
    }

}

int main() {
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
    highestScore = loadHighestScore(); // Load the highest score at the start
    while (true) {
        system("cls");
        HideCursor();
        displayMenu();
    }
    return 0;
}

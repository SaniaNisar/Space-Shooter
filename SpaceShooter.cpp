#include <iostream>
#include <Windows.h>
#include <conio.h>  // For _getch() and _kbhit()
#include <ctime>    // For time()

using namespace std;

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
int shipX = 57, shipY = 22; // Player spaceship position
int enemyX = 0, enemyY = 0; // Enemy position
int bulletX = 0, bulletY = 0; // Bullet position
int score = 0, life = 5; // Game stats
bool fired = false; // Bullet status

// Function declarations
void HideCursor();
void gotoxy(int x, int y);
void setConsoleColor(int color);
void drawBorder();
void displayLivesAndScore();
void WelcomeMessage();
void playGame();
void spaceship();
void eraseSpaceship();
void movement();
void shoot();
void enemy();

int main() {
    HideCursor();
    WelcomeMessage();
    return 0;
}

void HideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &cursorInfo);
}

void gotoxy(int x, int y) {
    COORD pos = {SHORT(x), SHORT(y)};
    SetConsoleCursorPosition(hStdOut, pos);
}

void setConsoleColor(int color) {
    SetConsoleTextAttribute(hStdOut, color);
}

void WelcomeMessage() {
    system("cls");
    gotoxy(13, 6);
    setConsoleColor(11); // Light cyan color for welcome screen
    cout << " _____                    _____ _           _           \n"
         << "|   __|___ ___ ___ ___   |   __| |_ ___ ___| |_ ___ ___ \n"
         << "|__   | -_|  _|  _| -_|  |__   |  _| -_|  _|  _| -_|  _|\n"
         << "|_____|___|___|___|___|  |_____|_| |___|___|_| |___|_|  \n"
         << "      |_|                                            \n";
    cout << "Welcome to Space Shooter!" << endl;
    cout << "Press '1' to play game\n";
    cout << "Press '2' for instructions\n";
    cout << "Press '0' to exit game\n";
    
    
    char ch = _getch();
    switch (ch) {
        case '1':
            playGame();
            break;
        case '2':
            setConsoleColor(14); // Yellow for instructions
            cout << "Instructions:\n"
                 << "(1) Use left arrow key to move the spaceship left.\n"
                 << "(2) Use right arrow key to move the spaceship right.\n"
                 << "(3) Press space to shoot.\n"
                 << "Press '1' to play game or '0' to exit.\n";
            ch = _getch();
            if (ch == '1') {
                playGame();
            } else if (ch == '0') {
                exit(0);
            }
            break;
        case '0':
            exit(0);
            break;
    }
}

void playGame() {
    system("cls");
    drawBorder();
    displayLivesAndScore();
    spaceship();
    while (true) {
        if (_kbhit()) {
            movement();
        }
        if (fired) {
            shoot();
        }
        enemy();
        Sleep(50); // Slow down the game loop
    }
}

void drawBorder() {
    setConsoleColor(10); // Green color for the border
    for (int i = 0; i < 100; i++) { // Top border
        gotoxy(10 + i, 0);
        cout << "-";
    }
    for (int i = 0; i < 25; i++) { // Side borders
        gotoxy(10, i);
        cout << "|";
        gotoxy(109, i);
        cout << "|";
    }
    for (int i = 0; i < 100; i++) { // Bottom border
        gotoxy(10 + i, 25);
        cout << "-";
    }
    setConsoleColor(7); // Reset to white for gameplay
}

void displayLivesAndScore() {
    gotoxy(15, 1);
    cout << "Lives: ";
    for (int i = 0; i < life; i++) {
        cout << char(254) << " "; // Display lives using block characters
    }
    gotoxy(100, 1);
    cout << "Score: " << score;
}

void spaceship() {
    setConsoleColor(15); // Bright white for spaceship
    gotoxy(shipX, shipY);
    cout << " ^ ";
    gotoxy(shipX, shipY + 1);
    cout << "<#>";
    gotoxy(shipX, shipY + 2);
    cout << "H H";
}

void eraseSpaceship() {
    for (int i = 0; i < 3; i++) {
        gotoxy(shipX, shipY + i);
        cout << "   ";
    }
}

void movement() {
    char ch = _getch();
    if (ch == 75 && shipX > 13) { // Left arrow
        eraseSpaceship();
        shipX -= 2;
        spaceship();
    } else if (ch == 77 && shipX < 106) { // Right arrow
        eraseSpaceship();
        shipX += 2;
        spaceship();
    } else if (ch == ' ') { // Spacebar to shoot
        bulletX = shipX + 1; // Bullet starts in the middle of the spaceship
        bulletY = shipY - 1;
        fired = true;
    }
}

void shoot() {
    setConsoleColor(12); // Red for bullet
    if (bulletY > 0) {
        gotoxy(bulletX, bulletY);
        cout << "|";
        gotoxy(bulletX, bulletY + 1);
        cout << " ";
        bulletY--;
    } else {
        fired = false; // Bullet has reached the top
    }
    setConsoleColor(7); // Reset to white for gameplay
}

void enemy() {
    setConsoleColor(13); // Purple for enemy
    if (enemyY == 0) {
        srand(time(NULL));
        enemyX = (rand() % 95) + 11; // Enemy appears within borders
        enemyY = 1; // Start just below the top border
    }
    gotoxy(enemyX, enemyY);
    cout << " ";
    enemyY++;
    if (enemyY < 25) {
        gotoxy(enemyX, enemyY);
        cout << char(15);
    } else {
        enemyY = 0; // Reset enemy position if it reaches the bottom
    }
    setConsoleColor(7); // Reset to white for gameplay
}

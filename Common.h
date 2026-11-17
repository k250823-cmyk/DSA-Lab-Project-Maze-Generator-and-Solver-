#ifndef COMMON_H
#define COMMON_H
//common file for the shared constants and helpers that will be needed
//include this header file for your classes
//new functions have comments that explain their role
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <conio.h>
using namespace std;

//constants
const int MAXN = 20;                //largest maze size (cells per side)
const int MAXT = 2 * MAXN + 1;      //tiles per side (cells plus the walls between them)
const int MAXTILES = MAXT * MAXT;   //total tiles in the biggest maze

//directions: 0 = up (W), 1 = down (S), 2 = left (A), 3 = right (D)
const int dr[4] = {-1, 1, 0, 0};
const int dc[4] = {0, 0, -1, 1};
const char dirKey[4] = {'W', 'S', 'A', 'D'};

//results of a move
const int MOVE_INVALID = 0;
const int MOVE_OK = 1;
const int MOVE_WRONG = 2;
const int MOVE_WIN = 3;
const int MOVE_LOST = 4;

//console handle used for colours and cursor control
HANDLE hOut;

void setColor(int color){
    //sets the colour of everything printed after this. colour number = background * 16 + text colour
    SetConsoleTextAttribute(hOut, color);
}

void gotoxy(int x, int y) {
    //a COORD is a windows type that holds a column (X) and a row (Y) of the console
    COORD pos;
    pos.X = x;
    pos.Y = y;
    //moves the cursor to that column and row, so the maze is redrawn in the same place instead of clearing the screen (no flicker)
    SetConsoleCursorPosition(hOut, pos);
}

void showCursor(bool visible) {
    //a CONSOLE_CURSOR_INFO describes the blinking cursor: dwSize is its thickness (1 to 100) and bVisible shows or hides it
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 10;
    info.bVisible = visible;
    //applies the cursor settings above to the console
    SetConsoleCursorInfo(hOut, &info);
}

string pad(string text, int width){
    //length() is the number of characters in the text, spaces are added until it reaches the width
    while ((int)text.length() < width) {
        text = text + " ";
    }
    return text;
}

//converts a number to text (works on old compilers too)
string toStr(int number) {
    if (number == 0) {
        return "0";
    }
    string text = "";
    bool negative = false;
    if (number < 0) {
        negative = true;
        number = -number;
    }
    while (number > 0) {
        //number % 10 is the last digit, adding '0' turns it into a character, and it is placed in front of the text
        text = char('0' + number % 10) + text;
        number = number / 10;
    }
    if (negative) {
        text = "-" + text;
    }
    return text;
}

void pauseScreen() {
    setColor(7);
    cout<<endl;
    cout<<"  Press any key to continue..."<<endl;
    //waits for one key press (without showing it) so the screen stays until the user is ready
    _getch();
}

#endif

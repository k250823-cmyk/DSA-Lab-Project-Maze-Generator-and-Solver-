
//libraries and what we use from each of them
//iostream: input and output on the console. cout prints to the screen, cin reads what the user types, endl moves to a new line
//cin.clear() removes the error state of cin after a wrong input, cin.ignore(n, ch) throws away leftover characters until ch is found
#include <iostream>
//fstream: reading and writing files. ifstream opens a file for reading, ofstream opens a file for writing
//file>>x reads the next value from the file, file<<x writes a value into the file, file.close() closes it
#include <fstream>
//iomanip: formatting of output. setw(n) reserves n characters for the next printed value so table columns line up
#include <iomanip>
//string: the string type for text, and functions like length() which gives the number of characters
#include <string>
//cstdlib: general functions. rand() gives a random number, srand(seed) sets the starting point of those random numbers
//system("command") runs a windows command from inside the program, for example cls clears the screen
#include <cstdlib>
//ctime: time functions. time(0) gives the current time in seconds, which we use as the seed for srand
#include <ctime>
//windows.h: windows functions to control the console
//a HANDLE is a reference to the console window and GetStdHandle gets it
//the SetConsoleTextAttribute function changes the text and background colour, and a COORD is a column and row position
//the SetConsoleCursorPosition function moves the cursor, CONSOLE_CURSOR_INFO and SetConsoleCursorInfo set the size and visibility of the cursor
//the Sleep(ms) function pauses the program for that many milliseconds
#include <windows.h>
//conio.h: console keyboard input. _getch() reads one key press immediately without showing it and without pressing enter
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
//a HANDLE is a windows type that refers to the console window, it is filled in main using GetStdHandle
HANDLE hOut;

void setColor(int color){
    //sets the colour of everything printed after this. colour number = background * 16 + text colour
    //text colours: 0 black, 1 blue, 2 green, 4 red, 7 white, 8 gray, 10 light green, 11 light aqua, 12 light red, 14 yellow, 15 bright white
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


//stack (array based), used for dfs maze generation and for the path walked by the player
class Stack{
	public:
	    int data[MAXTILES];
	    int top;
	
	    Stack() {
	        top = -1;
	    }
	
	    void push(int value) {
	        if (top < MAXTILES - 1) {
	            top++;
	            data[top] = value;
	        }
	    }
	
	    int pop() {
	        if (top >= 0) {
	            int value = data[top];
	            top--;
	            return value;
	        }
	        return -1;
	    }
	
	    int peek() {
	        if (top >= 0) {
	            return data[top];
	        }
	        return -1;
	    }
	
	    //the element just below the top (the previous tile on the path)
	    int peekBelow() {
	        if (top >= 1) {
	            return data[top - 1];
	        }
	        return -1;
	    }
	
	    bool isEmpty() {
	        return top == -1;
	    }
	
	    int size() {
	        return top + 1;
	    }
	
	    void clear() {
	        top = -1;
	    }
};


//queue (circular array), used for bfs
class Queue{
	public:
	    int data[MAXTILES];
	    int front;
	    int rear;
	    int count;
	
	    Queue() {
	        front = 0;
	        rear = -1;
	        count = 0;
	    }
	
	    void enqueue(int value) {
	        if (count < MAXTILES) {
	            rear = (rear + 1) % MAXTILES;
	            data[rear] = value;
	            count++;
	        }
	    }
	
	    int dequeue() {
	        if (count == 0) {
	            return -1;
	        }
	        int value = data[front];
	        front = (front + 1) % MAXTILES;
	        count--;
	        return value;
	    }
	
	    bool isEmpty() {
	        return count == 0;
	    }
};


//the maze is stored as a grid of tiles: 1 = wall, 0 = open path
//cells sit on the odd rows and columns, the tiles between two cells are the walls that dfs removes
//every open tile is a vertex of a graph and its open neighbours are connected to it
class Maze{
	public:
	    int n;                          //cells per side
	    int size;                       //tiles per side (2n + 1)
	    int tile[MAXT][MAXT];           //1 = wall, 0 = open
	    bool onPath[MAXT][MAXT];        //true if the tile is on the shortest path
	    int parent[MAXTILES];           //bfs parent of every tile
	    int startR, startC;
	    int exitR, exitC;
	    int shortestLen;                //number of steps on the shortest path
	
	    //randomized dfs (recursive backtracker) using our stack
	    void generate(int cells)
	    {
	        n = cells;
	        size = 2 * n + 1;
	
	        for (int r=0; r<size; r++)
	        {
	            for (int c=0; c<size; c++)
	            {
	                tile[r][c] = 1;
	                onPath[r][c] = false;
	            }
	        }
	
	        bool vis[MAXN][MAXN];
	        for (int r=0; r<n; r++)
	        {
	            for (int c=0; c<n; c++)
	            {
	                vis[r][c] = false;
	            }
	        }
	
	        Stack cellStack;
	        //rand() gives a random number, % n keeps it between 0 and n-1, so this is a random starting row
	        int sr = rand() % n;
	        int sc = rand() % n;
	        vis[sr][sc] = true;
	        tile[2 * sr + 1][2 * sc + 1] = 0;
	        cellStack.push(sr * n + sc);
	
	        while (!cellStack.isEmpty())
	        {
	            int cur = cellStack.peek();
	            int r = cur / n;
	            int c = cur % n;
	
	            //collect the unvisited neighbours
	            int options[4];
	            int count = 0;
	            for (int d=0; d<4; d++)
	            {
	                int nr = r + dr[d];
	                int nc = c + dc[d];
	                if (nr >= 0 && nr < n && nc >= 0 && nc < n && !vis[nr][nc])
	                {
	                    options[count] = d;
	                    count++;
	                }
	            }
	
	            if (count > 0)
	            {
	                //carve a passage to a random unvisited neighbour
	                //rand() % count picks one of the available directions at random
	                int d = options[rand() % count];
	                int nr = r + dr[d];
	                int nc = c + dc[d];
	                tile[2 * r + 1 + dr[d]][2 * c + 1 + dc[d]] = 0;
	                tile[2 * nr + 1][2 * nc + 1] = 0;
	                vis[nr][nc] = true;
	                cellStack.push(nr * n + nc);
	            }
	            else
	            {
	                //dead end, go back
	                cellStack.pop();
	            }
	        }
	
	        startR = 1;
	        startC = 1;
	        exitR = size - 2;
	        exitC = size - 2;
	        solve();
	    }
	
	    //bfs using our queue, finds the shortest path from the start to the exit and marks it
	    void solve()
	    {
	        for (int i=0; i<MAXTILES; i++)
	        {
	            parent[i] = -1;
	        }
	        for (int r=0; r<size; r++)
	        {
	            for (int c=0; c<size; c++)
	            {
	                onPath[r][c] = false;
	            }
	        }
	
	        int startCode = startR * MAXT + startC;
	        int exitCode = exitR * MAXT + exitC;
	
	        Queue q;
	        parent[startCode] = startCode;
	        q.enqueue(startCode);
	
	        while (!q.isEmpty())
	        {
	            int cur = q.dequeue();
	            if (cur == exitCode)
	            {
	                break;
	            }
	            int r = cur / MAXT;
	            int c = cur % MAXT;
	            for (int d=0; d<4; d++)
	            {
	                int nr = r + dr[d];
	                int nc = c + dc[d];
	                if (tile[nr][nc] == 0 && parent[nr * MAXT + nc] == -1)
	                {
	                    parent[nr * MAXT + nc] = cur;
	                    q.enqueue(nr * MAXT + nc);
	                }
	            }
	        }
	
	        //walk back from the exit to the start and mark the tiles
	        shortestLen = 0;
	        int cur = exitCode;
	        while (cur != startCode)
	        {
	            onPath[cur / MAXT][cur % MAXT] = true;
	            cur = parent[cur];
	            shortestLen++;
	        }
	        onPath[startR][startC] = true;
	    }
	
	    //fills a stack with the shortest path, the start tile ends up on top
	    void buildPath(Stack &s)
	    {
	        s.clear();
	        int startCode = startR * MAXT + startC;
	        int cur = exitR * MAXT + exitC;
	        while (true)
	        {
	            s.push(cur);
	            if (cur == startCode)
	            {
	                break;
	            }
	            cur = parent[cur];
	        }
	    }
};


//one move of the player (linked list node)
class MoveNode{
	public:
	    char move;
	    MoveNode *next;
	    MoveNode(char m) {
	        move = m;
	        next = NULL;
	    }
};

//linked list of all the moves made in one game
class MoveList{
	public:
	    MoveNode *head;
	    MoveNode *tail;
	    int count;
	
	    MoveList() {
	        head = NULL;
	        tail = NULL;
	        count = 0;
	    }
	
	    ~MoveList() {
	        clear();
	    }
	
	    void add(char m) {
	        MoveNode *node = new MoveNode(m);
	        if (head == NULL) {
	            head = node;
	            tail = node;
	        }
	        else {
	            tail->next = node;
	            tail = node;
	        }
	        count++;
	    }
	    void clear() {
	        while (head != NULL) {
	            MoveNode *temp = head;
	            head = head->next;
	            delete temp;
	        }
	        tail = NULL;
	        count = 0;
	    }
	
	    void print() {
	        MoveNode *cur = head;
	        while (cur != NULL) {
	            cout<<cur->move;
	            cur = cur->next;
	        }
	        cout<<endl;
	    }
};


//one finished game (linked list node)
class GameRecord{
	public:
	    string level;
	    int size;
	    int moves;
	    int livesLeft;
	    int score;
	    bool won;
	    GameRecord *next;
	
	    GameRecord(string l, int s, int m, int lv, int sc, bool w) {
	        level = l;
	        size = s;
	        moves = m;
	        livesLeft = lv;
	        score = sc;
	        won = w;
	        next = NULL;
	    }
};

//linked list of all the games played, saved in history.txt
class HistoryList{
	public:
	    GameRecord *head;
	    GameRecord *tail;
	    int count;
	
	    HistoryList() {
	        head = NULL;
	        tail = NULL;
	        count = 0;
	    }
	
	    ~HistoryList() {
	        while (head != NULL) {
	            GameRecord *temp = head;
	            head = head->next;
	            delete temp;
	        }
	    }
	
	    void add(string level, int size, int moves, int livesLeft, int score, bool won) {
	        GameRecord *node = new GameRecord(level, size, moves, livesLeft, score, won);
	        if (head == NULL) {
	            head = node;
	            tail = node;
	        }
	        else {
	            tail->next = node;
	            tail = node;
	        }
	        count++;
	    }
	
	    void loadFromFile() {
	        //opens history.txt for reading
	        ifstream file("history.txt");
	        //the file could not be opened (for example on the first run), so there is nothing to load
	        if (!file) {
	            return;
	        }
	        string level;
	        int size, moves, livesLeft, score, won;
	        //file>>x reads the next value from the file, the loop ends when there is nothing left to read
	        while (file>>level>>size>>moves>>livesLeft>>score>>won) {
	            add(level, size, moves, livesLeft, score, won == 1);
	        }
	        //closes the file so everything is saved properly
	        file.close();
	    }
	
	    void saveToFile() {
	        //opens history.txt for writing, the old content of the file is replaced
	        ofstream file("history.txt");
	        GameRecord *cur = head;
	        while (cur != NULL) {
	            int won = 0;
	            if (cur->won) {
	                won = 1;
	            }
	            //file<<x writes a value into the file, it works like cout but writes to the file
	            file<<cur->level<<" "<<cur->size<<" "<<cur->moves<<" "<<cur->livesLeft<<" "<<cur->score<<" "<<won<<endl;
	            cur = cur->next;
	        }
	        //closes the file so everything is saved properly
	        file.close();
	    }
	
	    void print() {
	        setColor(14);
	        cout<<endl;
	        cout<<"   GAME HISTORY"<<endl;
	        cout<<endl;
	        if (count == 0) {
	            setColor(7);
	            cout<<"   No games played yet."<<endl;
	            return;
	        }
	
	        setColor(11);
	        cout<<"   No.   Level    Maze     Moves   Lives   Score   Result"<<endl;
	        setColor(7);
	
	        int number = 1;
	        int wins = 0;
	        GameRecord *cur = head;
	        while (cur != NULL) {
	            //left aligns the text to the left, setw(n) reserves n characters for the next value, so the table columns line up
	            cout<<"   "<<left<<setw(6)<<number<<setw(9)<<cur->level<<setw(9)<<cur->size<<setw(8)<<cur->moves<<setw(8)<<cur->livesLeft<<setw(8)<<cur->score;
	            if (cur->won) {
	                setColor(10);
	                cout<<"Won"<<endl;
	                wins++;
	            }
	            else {
	                setColor(12);
	                cout<<"Lost"<<endl;
	            }
	            setColor(7);
	            number++;
	            cur = cur->next;
	        }
	        cout<<endl;
	        cout<<"   Games played: "<<count<<"    Wins: "<<wins<<"    Losses: "<<count - wins<<endl;
	    }
};


//one game of maze quest
class Game{
	public:
	    Maze maze;
	    Stack path;                     //tiles walked by the player (start at the bottom)
	    MoveList moves;                 //every move the player made
	    bool visited[MAXT][MAXT];
	    int pr, pc;                     //player position
	    int lives;
	    int startLives;
	    int moveCount;
	    string levelName;
	    string status;
	    int statusColor;
	    bool showSolution;
	
	    void setup(string name, int cells, int numLives) {
	        levelName = name;
	        maze.generate(cells);
	        startLives = numLives;
	        lives = numLives;
	        moveCount = 0;
	        moves.clear();
	        path.clear();
	
	        for (int r=0; r<MAXT; r++) {
	            for (int c=0; c<MAXT; c++) {
	                visited[r][c] = false;
	            }
	        }
	
	        pr = maze.startR;
	        pc = maze.startC;
	        path.push(pr * MAXT + pc);
	        visited[pr][pc] = true;
	        status = "Find your way to the exit (EX)!";
	        statusColor = 11;
	        showSolution = false;
	    }
	
	    void drawTile(int r, int c) {
	        if (r == pr && c == pc) {
	            setColor(224);
	            cout<<"()";
	        }
	        else if (maze.tile[r][c] == 1) {
	            setColor(31);
	            cout<<"  ";
	        }
	        else if (r == maze.exitR && c == maze.exitC) {
	            setColor(207);
	            cout<<"EX";
	        }
	        else if (showSolution && maze.onPath[r][c]) {
	            setColor(176);
	            cout<<"  ";
	        }
	        else if (r == maze.startR && c == maze.startC) {
	            setColor(10);
	            cout<<"ST";
	        }
	        else if (visited[r][c]) {
	            setColor(2);
	            cout<<"::";
	        }
	        else {
	            setColor(0);
	            cout<<"  ";
	        }
	    }
	
	    void draw() {
	        gotoxy(0, 0);
	        setColor(14);
	        cout<<pad("  MAZE QUEST", 90)<<endl;
	        setColor(15);
	        cout<<pad("  Difficulty: " + levelName, 90)<<endl;
	        setColor(7);
	        cout<<"  Lives: ";
	        for (int i=0; i<startLives; i++) {
	            if (i < lives) {
	                setColor(12);
	                cout<<"O ";
	            }
	            else {
	                setColor(8);
	                cout<<". ";
	            }
	        }
	        setColor(7);
	        cout<<pad("", 60)<<endl;
	        setColor(7);
	        cout<<pad("  Moves: " + toStr(moveCount), 90)<<endl;
	        setColor(8);
	        cout<<pad("  W A S D or arrow keys = move     Q = quit to menu", 90)<<endl;
	        setColor(statusColor);
	        cout<<pad("  " + status, 90)<<endl;
	        setColor(7);
	        cout<<endl;
	
	        for (int r=0; r<maze.size; r++) {
	            cout<<"  ";
	            for (int c=0; c<maze.size; c++) {
	                drawTile(r, c);
	            }
	            setColor(7);
	            cout<<endl;
	        }
	    }
	
	    //walks the player back to the start by popping the stack
	    void backtrack() {
	        while (path.size() > 1) {
	            path.pop();
	            int code = path.peek();
	            pr = code / MAXT;
	            pc = code % MAXT;
	            draw();
	            //the Sleep() call pauses the program for 60 milliseconds so the walk back to the start is visible as an animation
	            Sleep(60);
	        }
	    }
	
	    //tries to move the player one step in direction dir
	    int tryMove(int dir) {
	        int nr = pr + dr[dir];
	        int nc = pc + dc[dir];
	
	        if (maze.tile[nr][nc] == 1) {
	            status = "That is a wall.";
	            statusColor = 8;
	            return MOVE_INVALID;
	        }
	
	        moveCount++;
	        moves.add(dirKey[dir]);
	
	        //stepping back onto the previous tile just undoes the last step
	        int code = nr * MAXT + nc;
	        if (code == path.peekBelow()) {
	            path.pop();
	        }
	        else {
	            path.push(code);
	        }
	        pr = nr;
	        pc = nc;
	        visited[nr][nc] = true;
	
	        if (nr == maze.exitR && nc == maze.exitC) {
	            status = "You reached the exit!";
	            statusColor = 10;
	            return MOVE_WIN;
	        }
	
	        //a tile that is not on the shortest path is a wrong turn
	        if (!maze.onPath[nr][nc]) {
	            lives--;
	            statusColor = 12;
	            if (lives == 0) {
	                status = "Wrong turn! You have no lives left.";
	                return MOVE_LOST;
	            }
	            status = "Wrong turn! You lost a life. Going back to the start...";
	            draw();
	            //pauses for 700 milliseconds so the player can read the wrong turn message
	            Sleep(700);
	            backtrack();
	            status = "Back at the start. Try another way!";
	            statusColor = 14;
	            return MOVE_WRONG;
	        }
	
	        status = "Keep going...";
	        statusColor = 11;
	        return MOVE_OK;
	    }
	
	    int calcScore() {
	        int bonus = maze.shortestLen * 3 - moveCount;
	        if (bonus < 0) {
	            bonus = 0;
	        }
	        return lives * 100 + bonus;
	    }
	
	    //returns 1 if the player won, 0 if the player lost, -1 if the player quit
	    int play() {
	        //system() runs a windows command, cls clears the console screen
	        system("cls");
	        showCursor(false);
	        draw();
	
	        while (true) {
	            //reads one key press immediately, no need to press enter
	            int ch = _getch();
	            if (ch == 0 || ch == 224) {
	                //arrow keys send two codes
	                //the second code tells which arrow key it was: 72 up, 80 down, 75 left, 77 right
	                ch = _getch();
	                if (ch == 72) ch = 'w';
	                else if (ch == 80) ch = 's';
	                else if (ch == 75) ch = 'a';
	                else if (ch == 77) ch = 'd';
	                else ch = 0;
	            }
	            if (ch >= 'A' && ch <= 'Z') {
	                ch = ch + 32;
	            }
	
	            if (ch == 'q') {
	                showCursor(true);
	                return -1;
	            }
	
	            int dir = -1;
	            if (ch == 'w') dir = 0;
	            else if (ch == 's') dir = 1;
	            else if (ch == 'a') dir = 2;
	            else if (ch == 'd') dir = 3;
	            if (dir == -1) {
	                continue;
	            }
	
	            int result = tryMove(dir);
	            draw();
	            if (result == MOVE_WIN) {
	                showCursor(true);
	                return 1;
	            }
	            if (result == MOVE_LOST) {
	                showCursor(true);
	                return 0;
	            }
	        }
	    }
	
	    //shows the bfs shortest path being walked from the start to the exit
	    void autoSolve() {
	        system("cls");
	        showCursor(false);
	        Stack s;
	        maze.buildPath(s);
	        status = "Auto-solving the maze...";
	        statusColor = 11;
	        draw();
	
	        while (!s.isEmpty()) {
	            int code = s.pop();
	            pr = code / MAXT;
	            pc = code % MAXT;
	            visited[pr][pc] = true;
	            moveCount++;
	            draw();
	            //pauses for 40 milliseconds between steps so the solution is shown as an animation
	            Sleep(40);
	        }
	
	        moveCount--;
	        showSolution = true;
	        status = "Solved! The shortest path has " + toStr(maze.shortestLen) + " steps.";
	        statusColor = 10;
	        draw();
	        showCursor(true);
	    }
};

int main(){
    //time(0) is the current time in seconds, srand() uses it as the starting point so rand() gives a different maze every run
    srand((unsigned int)time(0));
    //the GetStdHandle call gives us the console output window, so setColor, gotoxy and showCursor can control it
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    //mode con sets the console window to 100 columns and 56 rows so the hard maze fits on the screen
    system("mode con cols=100 lines=56");
    //sets the text shown in the title bar of the console window
    system("title Maze Quest");

    HistoryList history;
    history.loadFromFile();

    //difficulty settings: easy, medium, hard
    const string levelName[3] = {"Easy", "Medium", "Hard"};
    const int levelCells[3] = {10, 15, 20};
    const int levelLives[3] = {5, 3, 2};

    int choice = 0;
    while (choice != 4) {
        system("cls");
        showCursor(true);
        setColor(14);
        cout<<endl;
        cout<<"   M A Z E   Q U E S T"<<endl;
        setColor(8);
        cout<<"   random maze generator and solver"<<endl;
        cout<<endl;
        setColor(15);
        cout<<"   1. New Game"<<endl;
        cout<<"   2. Auto-Solve"<<endl;
        cout<<"   3. View History"<<endl;
        cout<<"   4. Exit"<<endl;
        cout<<endl;
        setColor(11);
        cout<<"   Enter your choice: ";
        setColor(7);

        //cin>>choice reads a typed number into choice, it fails if the user types something that is not a number
        if (!(cin>>choice)) {
            //removes the error state of cin, otherwise it would refuse all further input
            cin.clear();
            //throws away up to 1000 leftover characters until the end of the line
            cin.ignore(1000, '\n');
            choice = 0;
        }

        if (choice == 1 || choice == 2) {
            //difficulty menu
            int level = 0;
            while (level < 1 || level > 4) {
                system("cls");
                setColor(14);
                cout<<endl;
                cout<<"   SELECT DIFFICULTY"<<endl;
                cout<<endl;
                setColor(10);
                cout<<"   1. Easy     ("<<levelCells[0]<<" x "<<levelCells[0]<<" maze, "<<levelLives[0]<<" lives)"<<endl;
                setColor(14);
                cout<<"   2. Medium   ("<<levelCells[1]<<" x "<<levelCells[1]<<" maze, "<<levelLives[1]<<" lives)"<<endl;
                setColor(12);
                cout<<"   3. Hard     ("<<levelCells[2]<<" x "<<levelCells[2]<<" maze, "<<levelLives[2]<<" lives)"<<endl;
                setColor(15);
                cout<<"   4. Back"<<endl;
                cout<<endl;
                setColor(11);
                cout<<"   Enter your choice: ";
                setColor(7);
                if (!(cin>>level)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    level = 0;
                }
            }

            if (level == 4) {
                continue;
            }

            int i = level - 1;
            Game game;
            game.setup(levelName[i], levelCells[i], levelLives[i]);

            if (choice == 1) {
                int result = game.play();
                if (result != -1) {
                    bool won = (result == 1);
                    int score = 0;
                    if (won) {
                        score = game.calcScore();
                    }
                    history.add(levelName[i], levelCells[i], game.moveCount, game.lives, score, won);
                    history.saveToFile();

                    system("cls");
                    cout<<endl;
                    if (won) {
                        setColor(10);
                        cout<<"   YOU ESCAPED THE MAZE!"<<endl;
                    }
                    else {
                        setColor(12);
                        cout<<"   GAME OVER - you ran out of lives."<<endl;
                    }
                    setColor(7);
                    cout<<endl;
                    cout<<"   Difficulty  : "<<levelName[i]<<endl;
                    cout<<"   Maze size   : "<<levelCells[i]<<" x "<<levelCells[i]<<endl;
                    cout<<"   Moves       : "<<game.moveCount<<endl;
                    cout<<"   Lives left  : "<<game.lives<<endl;
                    cout<<"   Score       : "<<score<<endl;
                    cout<<endl;
                    setColor(11);
                    cout<<"   Your moves:"<<endl;
                    setColor(7);
                    cout<<"   ";
                    game.moves.print();
                    pauseScreen();
                }
            }
            else {
                game.autoSolve();
                pauseScreen();
            }
        }
        else if (choice == 3) {
            system("cls");
            history.print();
            pauseScreen();
        }
        else if (choice != 4) {
            setColor(12);
            cout<<endl;
            cout<<"   Invalid choice, please enter 1 to 4."<<endl;
            pauseScreen();
        }
    }

    system("cls");
    setColor(14);
    cout<<endl;
    cout<<"   Thanks for playing Maze Quest!"<<endl;
    setColor(7);
    cout<<endl;
    return 0;
}
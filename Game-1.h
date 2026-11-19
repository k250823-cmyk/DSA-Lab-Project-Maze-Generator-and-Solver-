#ifndef GAME_H
#define GAME_H
#include "Common.h"
#include "Stack.h"
#include "Queue.h"
#include "Maze.h"
#include "MoveList.h"

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

#endif

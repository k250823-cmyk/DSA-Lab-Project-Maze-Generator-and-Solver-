#ifndef GAME_H
#define GAME_H
#include "Common.h"
#include "Stack.h"
#include "Queue.h"
#include "Maze.h"

//one game of maze quest
class Game{
	public:
	    Maze maze;
	    Stack path;                     //tiles walked by the player (start at the bottom)
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
};

#endif

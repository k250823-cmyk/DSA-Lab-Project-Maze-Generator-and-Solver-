#ifndef MAZE_H
#define MAZE_H
#include "Common.h"
#include "Stack.h"
#include "Queue.h"

//the maze is stored as a grid of tiles: 1 = wall, 0 = open path
//cells sit on the odd rows and columns, the tiles between two cells are the walls that dfs removes
//every open tile is a vertex of a graph and its open neighbours are connected to it

//has function to generate the maze, to solve will be added later
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
	    void generate(int cells) {
	        n = cells;
	        size = 2 * n + 1;
	
	        for (int r=0; r<size; r++) {
	            for (int c=0; c<size; c++) {
	                tile[r][c] = 1;
	                onPath[r][c] = false;
	            }
	        }
	        bool vis[MAXN][MAXN];
	        for (int r=0; r<n; r++) {
	            for (int c=0; c<n; c++) {
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
	
	        while (!cellStack.isEmpty()) {
	            int cur = cellStack.peek();
	            int r = cur / n;
	            int c = cur % n;
	
	            //collect the unvisited neighbours
	            int options[4];
	            int count = 0;
	            for (int d=0; d<4; d++) {
	                int nr = r + dr[d];
	                int nc = c + dc[d];
	                if (nr >= 0 && nr < n && nc >= 0 && nc < n && !vis[nr][nc]){
	                    options[count] = d;
	                    count++;
	                }
	            }
	
	            if (count > 0){
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
	            else{
	                //dead end, go back
	                cellStack.pop();
	            }
	        }
	
	        startR = 1;
	        startC = 1;
	        exitR = size - 2;
	        exitC = size - 2;
	        //the bfs solve() step is added in the next part
	    }
};

#endif

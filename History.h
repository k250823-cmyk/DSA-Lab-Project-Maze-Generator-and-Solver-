#ifndef HISTORY_H
#define HISTORY_H
#include "Common.h"
//savetofile and loadtofile functions not added yet

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
};

#endif

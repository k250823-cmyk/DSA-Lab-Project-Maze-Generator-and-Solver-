#ifndef HISTORY_H
#define HISTORY_H
#include "Common.h"

//one finished game (linked list node)
//added the filing functions, now this file is compelete
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

#endif

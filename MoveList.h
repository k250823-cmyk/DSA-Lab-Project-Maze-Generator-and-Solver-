#ifndef MOVELIST_H
#define MOVELIST_H
#include "Common.h"

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

#endif

#ifndef STACK_H
#define STACK_H
#include "Common.h"

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

#endif

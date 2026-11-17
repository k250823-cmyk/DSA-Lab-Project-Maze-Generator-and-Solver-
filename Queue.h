#ifndef QUEUE_H
#define QUEUE_H
#include "Common.h"

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

#endif

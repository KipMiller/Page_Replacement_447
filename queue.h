//Author: Chris Miller
// CSCI 447 Homework 4

#ifndef Queue
#define Queue

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The queue structure
struct Frame{
	int pageNumber;// All frames have a page number
	int timeStamp;// LRU with time stamp
	int counter;// LRU with counter 
	int chanceBit;// Second chance 
	int nextRef;// Next reference for Optimal replacement
	struct Frame *next, *prev;// pointer to the next and previous entry in the queue
};

// The actual queue which frames are stored in
// FIFO style
struct queue{
	int size;// the total size of the queue 
	int full;// how full the queue is
	struct Frame *front, *end;//front and back of the FIFO queue
};

struct Frame* createFrame();

struct queue* createQueue();

int pageSearch();

int optimalSearch();

int optimal();

int secondChance();

int LRUcounter();

void counterReset();

int timeStampAdd();

void addToQueue();

void addToEnd();

void removeQueue();

#endif
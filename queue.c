//Author: Chris Miller
// CSCI 447 Homework 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int lastExamined = 0;// used to keep track of the very last frame examined during second chance

// The frame structure will hold everything we need to accomplish all four algorithsm 
// for page replacement
struct Frame* createFrame(int pageNum){
	struct Frame* temp = (struct Frame*)malloc(sizeof(struct Frame));
	temp->timeStamp = 0;
	temp->counter = 0;
	temp->chanceBit = 0;
	temp->nextRef = 0;
	temp->pageNumber = pageNum;
	temp-> prev = NULL;
	temp->next = NULL;
	return temp;
}

// Creates a queue of SIZE, setting each page to -1 at first to indicate that each 
// spot is empty to begin with
struct queue* createQueue(int size){
	struct queue* q = (struct queue*)malloc(sizeof(struct queue));
	
	q->size = size;
	q->front = NULL;
	q->end = NULL;
	
	int index = 0; 
	while(index < size){
		addToQueue(q, -1);//fill the queue with empty slots
		index++;
	}
	return q;
}

// A method that will iterate through the table looking for any frame
// with the same given page number, if there is one it is a hit, else a miss
int pageSearch(struct queue* q, int pageNumber){
	struct Frame* temp = q->front;
	for(int i = 0; i < q->size; i++){
		if(temp->pageNumber == pageNumber){// HIT 
			temp->counter++;// Increment the counter for LRU counter
			temp->chanceBit = 1;
			return 1; 
		}
		temp = temp->next;
	}
	return -1;// MISS
}

// Search through the ahead queue (the next 500 lines of input) 
// to find when the page will be used again (if at all)
int optimalSearch(struct queue* q, int pageNum){
	if(q->end == NULL){
		return 0;
	}
	int count = 1;// how far away this number will be used 
	// check the next 500 lines of input for a match on this address
	struct Frame* temp = q->front;
	while(temp != NULL){
		if(temp->pageNumber == pageNum){
			return count;
		}	
		count++;
		temp = temp->next;
	}
	return 0;// if not within 500 lines, return 0
}

// The optimal algorithm will look ahead 500 lines of input and determine which pages should be replaced
// based upon when they will be used in the near future, preferring to replace items that are not used 
// at all over ones that will be used later (I.E. 0 = not in 500 lines)
int optimal(struct queue* qAhead, struct queue* q, int pageNum, int pageFault){
	struct Frame* temp = q->front;
	if(pageSearch(q, pageNum) == 1){// if the current page number is on the table
		temp->nextRef = optimalSearch(qAhead, pageNum);
		return pageFault;// HIT - return the page fault number un-incremented
	}
	pageFault++;// ELSE - Miss
	
	for(int i = 0; i < q->size; i++){// look for an empty slot 
		if(temp->pageNumber == -1){// if we have found an empty spot 
			temp->pageNumber = pageNum;
			temp->nextRef = optimalSearch(qAhead, pageNum);// this address will be addressed again in X number of lines 
			return pageFault;
		}
		temp = temp->next;
	}
	int maxRef = 0;// keep track of the largest possible next reference found
	temp = q->front;// reset the iterator variable
	for(int i = 0; i < q->size; i++){// iterate through the table to find the maximum next reference variable
		temp->nextRef = optimalSearch(qAhead, temp->pageNumber);
		if(temp->nextRef > maxRef){// look for the max distance for the next reference 
			maxRef = temp->nextRef;
		}
		temp = temp->next;
	}
	temp = q->front;// if there are no empty slots, we need to replace the page not used for the longest amount of time (or 0) 
	for(int i = 0; i < q->size; i++){// Iterate through the table, looking for any 0's to replace
		if( temp->nextRef == 0){ //|| (temp->nextRef == maxRef) ){// look for an empty slot 
			temp->pageNumber = pageNum;
			temp->nextRef = optimalSearch(qAhead, pageNum);// this address will be addressed again in X number of lines
			return pageFault;
		}
		temp = temp->next;
	}
	temp = q->front;// reset the iterator variable
	for(int i = 0; i < q->size; i++){// there were no 0's to replace, now look for the max next reference variable
		if( temp->nextRef == maxRef ){// if we have found the pre-determined maximum reference (not 0), replace said page	
			temp->pageNumber = pageNum;
			temp->nextRef = optimalSearch(qAhead, pageNum);// this address will be addressed again in X number of lines
			return pageFault;
		}
		temp = temp->next;
	}
	return -1;
}

// Second chance will only replace a page from the table if its chance bit is 0, chance bits are set to 1
// when a page is added to the table and set to 1 if they are referenced again on the page table. 
// If there are no empty spots and the page is not on the table, then we will loop through the list
// setting each bit to 0 until we encounter a pre-existing 0 to replace
int secondChance(struct queue* q, int pageNumber, int pageFault){
	if(pageSearch(q, pageNumber) == 1){// if the current page number is on the table
		return pageFault;// HIT - return the page fault number un-incremented
	}
	pageFault++;// ELSE- it is a page fault for not being on the table
	
	struct Frame* temp = q->front;
	for(int j = 0; j < q->size; j++){// look through for any empty spots before moving onto the second chance part 
		if(temp->pageNumber == -1){// if the current slot on the page table is empty, fill it 
			temp->pageNumber = pageNumber;
			temp->chanceBit = 1;// the new page gets a second chance bit 
			lastExamined = j;//set the last frame examined int for next time there is a page fault
			return pageFault;
		}
		temp = temp->next;
	}
	temp = q->front;
	int index = 0;
	while(index < lastExamined){// iterate to where we last left off
		temp = temp->next;
		index++;
	}
	for(int i = lastExamined; i < q->size; i++){// start from the last page examined after the fault 
		if(temp->chanceBit == 0){// if it is 0 then it is ready to be evicted
			temp->pageNumber = pageNumber;
			temp->chanceBit = 1;// the new page gets a second chance bit 
			lastExamined = i;//set the last frame examined int for next time there is a page fault
			return pageFault;
		} else {// else change the bit to 0 until we find a pre-existing 0
			temp->chanceBit = 0;
		}
		temp = temp->next;
		if(i == q->size - 1){// Loop back to the start if we didn't find any spot to put the page in 
			i = 0;// reset the index
			temp = q->front;// reset the temp to iterate through the table
		}	
	}
	return -1;
}

// LRU Counter uses a counter variable to determine which page to replace, 
// every time a page is referenced its counter is increased, every so often (period value)
// we will reset every entry on the table to 0 to prevent stagnation
int LRUcounter(struct queue* q, int pageNumber, int pageFault){
	if(pageSearch(q, pageNumber) == 1){// if the current page number is on the table
		return pageFault;// HIT - return the page fault number un-incremented
	}
	pageFault++;// ELSE- it is a page fault for not being on the table
	
	struct Frame* temp = q->front;
	int minCounter = 10000000;
	for(int i = 0; i < q->size; i++){// Now look for an empty spot to put the page number
		if(temp->pageNumber == -1){// add if the current slot is empty (-1)
			temp->pageNumber = pageNumber;// set the page number from -1
			return pageFault; 
		}
		if(temp->counter < minCounter){// Look for min counter while we iterate
			minCounter = temp->counter;//store the lowest valued counter in case we need to replace
		}
		temp = temp->next;
	}
	temp = q->front;//reset to the start of the queue so we can loop back for the min time stamp
	for(int j = 0; j < q->size; j++){// now look for the lowest valued time stamp (as there are no empty slots)
		if(temp->counter == minCounter){
			temp->pageNumber = pageNumber; 
			temp->counter = 0;
			return pageFault;
		}
		temp = temp->next;
	}
	return -1;
}

// Reset all of the counter in the queue to zero, at period intervals
void counterReset(struct queue* q){
	struct Frame* temp = q->front;
	for(int i = 0; i < q->size; i++){
		temp->counter = 0;
		temp = temp->next;
	}
}

// Will add to the queue only if a slot is empty OR if it has the min
// value time stamp in the queue 
int timeStampAdd(struct queue* q, int pageNumber, int time, int pageFault){
	if(pageSearch(q, pageNumber) == 1){// if the current page number is on the table
		return pageFault;// HIT - return the page fault number un-incremented
	}
	pageFault++;// ELSE- it is a page fault for not being on the table
	
	struct Frame* temp = q->front;
	int minTime = 10000000;
	for(int i = 0; i < q->size; i++){// Now look for an empty spot to put the page number
		if(temp->pageNumber == -1){// add if the current slot is empty (-1)
			temp->pageNumber = pageNumber;// set the page number from -1
			temp->timeStamp = time;// mark the time stamp
			return pageFault; 
		}
		if(temp->timeStamp < minTime && temp->timeStamp != 0){// Look for min timestamp while we iterate
			minTime = temp->timeStamp;//store the lowest valued time stamp in case we need to replace
		}
		temp = temp->next;
	}
	temp = q->front;//reset to the start of the queue so we can loop back for the min time stamp
	for(int j = 0; j < q->size; j++){// now look for the lowest valued time stamp (as there are no empty slots)
		if(temp->timeStamp == minTime){//if we have found our lowest time stamp, replace it 
			temp->pageNumber = pageNumber; 
			temp->timeStamp = time;
			return pageFault;
		}
		temp = temp->next;
	}
	return -1;
}

// Adds a new page to the given queue
void addToQueue(struct queue* q, int pageNumber){
	struct Frame* temp = createFrame(pageNumber);
	temp->next = q->front;// put the new frame at the start of the given queue

	if(q->end == NULL){// if the queue is empty
		q->front = temp;
		q->end = temp;
	} else {// re-arrange the order of the last entries in the queue
		q->front->prev = temp;
		q->front = temp;
	}
	
	q->full++;// increment how full the queue is 
}

// Add an entry to the very end a queue
void addToEnd(struct queue* q, int pageNumber){
	struct Frame* temp = createFrame(pageNumber);
	temp->prev = q->end;
	
	q->end->next = temp;
	q->end = temp;
}

// Remove an entry from the front of a given queue
void removeQueue(struct queue* q){
	if(q->front == NULL || q->end == NULL){
		q->end = NULL;
		return;
	}
	q->front = q->front->next; 
	
	if(q->front == NULL){
		q->end = NULL;
	}
	
}

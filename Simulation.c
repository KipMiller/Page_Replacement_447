/*
 *  File: Simulation.c
 *  Author: Chris Miller 
* 	Homework 4
 * 	11/23/19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "queue.h"

// Global Variables:
int framesNumber; // number of frames to be used in this simultion
int period;// period value for LRU with Counter
int memoryAccess = 0;// the number of memory references performed (shold be the number of lines in the trace file)

// Variables to keep track of how many page faults have occurred in each algorithm
int timeStampFault = 0;
int counterFault = 0;
int secondChanceFault = 0;
int optimalFault = 0;

// Used to print out all the page numbers and relevant data from a given queue
void printQueue(struct queue* q){
	printf(" Page #   | Next Ref \n");
	struct Frame* temp = q->front;
	for(int i = 0; i < q->size; i++){
		printf("%d   	|   %d\n", temp->pageNumber, temp->nextRef);
		temp = temp->next;
	}	
}

// Printf out all of the relevant data for each of the four algorithms
void printData(){
	printf("======================================\n");
	printf("Algorithm: LRU Timestamp\n");
	printf("Number of frames: %d\n", framesNumber);
	printf("Total memory accesses: %d\n", memoryAccess);
	printf("Total page faults: %d\n", timeStampFault);
	printf("======================================\n");
	
	printf("Algorithm: LRU Counter\n");
	printf("Number of frames: %d\n", framesNumber);
	printf("Period:	%d\n", period);
	printf("Total memory accesses: %d\n", memoryAccess);
	printf("Total page faults: %d\n", counterFault);
	printf("======================================\n");
	
	printf("Algorithm: Second Chance\n");
	printf("Number of frames: %d\n", framesNumber);
	printf("Total memory accesses: %d\n", memoryAccess);
	printf("Total page faults: %d\n", secondChanceFault);
	printf("======================================\n");
	
	printf("Algorithm: Optimal\n");
	printf("Number of frames: %d\n", framesNumber);
	printf("Total memory accesses: %d\n", memoryAccess);
	printf("Total page faults: %d\n", optimalFault);
	printf("======================================\n");
}

// A function whose input is the trace file name and the memory size 
// in frames along with the period value for LRU Counter Implementation
void Simulate(char* fileName) {
	FILE *fileHandler = fopen(fileName, "r");// first argument is the input file name 
	if( fileHandler == NULL){
	  printf("ERROR: File does not exist.");
	  exit(1);
	}

	FILE *fileCopy = fopen(fileName, "r");// A second file pointer used to look ahead 500 lines of input (for OPTIMAL)
	struct queue* aheadQueue = createQueue(500);// a queue to hold the next 500 lines of input 
	int i = 0;
	char line[100];
	struct Frame* temp = aheadQueue->front;
	while(fgets(line,1000, fileCopy) != NULL && i < 500){
		char *split1 = strtok(line, " ");
		split1[5] = '\0';// Chop off the last 3 bits of the address
		int pageNum = (int)strtol(split1, NULL, 16);// convert it into an int
		
		temp->pageNumber = pageNum; 
		temp = temp->next;
		i++;
	}
	char text[1000];// string to store each line of input text 
	int time = 1;// the clock-tick counter, each loop is 1 second

	struct queue* LRUtimeQueue = createQueue(framesNumber);//queue that utilizes the LRU time stamp algorithm
	struct queue* LRUcounterQueue = createQueue(framesNumber);// queue that uses LRU with counter implementation
	struct queue* secondChanceQueue = createQueue(framesNumber);// queue for the second chance algorithm
	struct queue* optimalQueue = createQueue(framesNumber);// queue for the optimal algorithm

	while(fgets(text, 1000, fileHandler) != NULL){// keep looping while we have input lines
			char *split = strtok(text, " ");
			split[5] = '\0';// Chop off the last 3 bits of the address
			int pageNum = (int)strtol(split, NULL, 16);// convert it into an int

			timeStampFault = timeStampAdd(LRUtimeQueue, pageNum, time, timeStampFault);// add to the queue using the LRU time stamp algorithm
			counterFault = LRUcounter(LRUcounterQueue, pageNum, counterFault);
			secondChanceFault = secondChance(secondChanceQueue, pageNum, secondChanceFault);
		
			if(time % period == 0){// When we have reached the period for reset value 
				counterReset(LRUcounterQueue);// reset all counters in the LRUcounter queue to 0 
			}
			
			if(fgets(line, 1000, fileCopy) != NULL){//add the next line of input to the ahead queue
				char *split2 = strtok(line, " ");
				split2[5] = '\0';// Chop off the last 3 bits of the address
				int pageNum = (int)strtol(split2, NULL, 16);// convert it into an int
				addToEnd(aheadQueue, pageNum);
			} 
			removeQueue(aheadQueue);// remove an item from the ahead queue (will eventually be emtpy by the end)
			optimalFault = optimal(aheadQueue, optimalQueue, pageNum, optimalFault);
			
			time++;
			memoryAccess++;// the number of memory references performed
	}
	printData();// when done with every line of input, print all the relevant information
}

// Run simulation
int main(int argc, char *argv[]) {
	if(argc != 4){ 
		printf("ERROR: Requires 3 command line arguments: fileName.txt frameNumbers periodValue\n");
		return 0;
	}
	framesNumber = atoi(argv[2]);// get the number of frames the table will be 
	period = atoi(argv[3]);// get the number of clock-ticks we will be resetting counters on 

	Simulate(argv[1]);// run the simulation 
}

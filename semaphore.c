/* Maeda Hanafi
   Semaphore 
   To compile: gcc -o filename filename.c -lpthread -lrt 

*/
#include<semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

//shared variables among thread a and b
int x, y;
//sem_t s1 = 0, s2 = 0;
sem_t s1, s2;
int dummy = 0;

void *threadAExec(int* threadID){
	
	while(dummy==0){
		x = write();
		//signal s1 by incrementing it
		sem_post(&s1); 
		//thread a waits for signal
		sem_wait(&s2);
		//read y
		printf("\nthread ID: %d, y: %d", *threadID, y);
		
	}
}

void *threadBExec(int* threadID){
	
	while(dummy==0){		
		//thread a waits for signal
		sem_wait(&s1);
		//read x
		printf("\nthread ID: %d, x: %d",*threadID,  x);
		y = write();
		//signal s2 by incrementing it
		sem_post(&s2); 
		
		
	}
}

int write(){
	int randNum = rand();	
	return randNum;
}

main(){

	void* result;	
	//thread variables
	int threadIDA, threadIDB;
	pthread_t thread[2];
	
	//initialize signal variables
	sem_init(&s1, 0, 1);
	sem_init(&s2, 0, 1);

	//run threads
	threadIDA = 0;	
	pthread_create(&thread[0], NULL, threadAExec,&threadIDA);
	threadIDB = 1;	
	pthread_create(&thread[1], NULL, threadBExec, &threadIDB);
	
	
	pthread_join( thread[0], &result);
	pthread_join( thread[1], &result); 
	
	
}
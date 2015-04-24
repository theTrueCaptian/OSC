/* Maeda Hanafi
   Projeect 2 Threads and Synchronization
   11/9/10
   to compile: gcc c1.c -lm -lpthread -lrt
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
#include<semaphore.h>
#include <sys/types.h>


#define NUMBER_OF_FILES 10

//mutex
pthread_mutex_t mutex1;
//condition variable for the critical section
pthread_cond_t isBeingAccessed;
//this indicates whether anyone is in the critical section
int anyoneInCS = 0;

//condition variable for scanFile function
pthread_cond_t isScanUsed;
//this indicates whether there  is a thread scanning indicated by a 1. 
int anyoneScanning = 0;

//largest prime
int largestPrimeAmongFiles = 0;
//file names
char fileNames[10][7]={"1.txt","2.txt","3.txt","4.txt","5.txt","6.txt","7.txt","8.txt","9.txt","10.txt"}; 
//char fileNames[10][30]={"test1.txt","test2.txt","test3.txt","test4.txt","test5.txt","6.txt","7.txt","8.txt","9.txt","10.txt"}; 

int numberOfThreads;
void fillFiles(){
	FILE *filePointer;
	int i = 0;
	int j = 0;	
	int randNum;

	//for(j=4; j<10; j++){
		i =0;
		filePointer = fopen("5.txt", "wt");
		
		if (filePointer == NULL) 
		{
			fprintf(stderr, "Can't open input file!\n");
			exit(1);
		}
		
		while(i<1000000)
		{
			randNum = rand();
			//fprintf(stderr,"%d ", randNum);
			fprintf(filePointer, "%d ", randNum);
			
			i++;
	
		}	
		fclose(filePointer);
	//}
}

//returns the greatest prime among the files
int executeSequentially(){
	//file pointer
	FILE *fileReader;	
	int fileLargestPrime;
	int i = 0;
	int largestPrime = 0;

	//got through all files
	for(i=0; i<NUMBER_OF_FILES; i++){
		//scan the file
		fileLargestPrime = scanFile(fileNames[i]);
		//is this the largest prime so far?
		if(fileLargestPrime>largestPrime){
			largestPrime = fileLargestPrime;
		}
	}	
	return largestPrime;
}

//returns the largesst prime in th file
int scanFile(char* file){
	FILE *fileReader;	
	int number;
	int largestPrime = 0;
	
	fileReader = fopen(file, "r");
	
	while (fscanf(fileReader, "%d", &number) != EOF){
		//check if number greater than the largeest prime so far
		if(number>largestPrime){
			//is it prime
			if(isPrime(number)){
				largestPrime = number;				
			}
		}		
	}

	//printf("\n largest prime is %d in file %s", largestPrime, file);
	return largestPrime;
}


//accepts a string of files to process
void *threadWorker(char** infiles){
	char* temp;
	char* copyTemp;
	char* files = *infiles;
	char* cpyFiles = malloc(strlen(files)+1);
	int threadID;
	int i =0;
	pthread_t scanner[10];
	FILE *fileReader;	
	int number;
	int largestPrime = 0;
	char* file;
	
	pthread_mutex_lock( &mutex1 );
	while(anyoneInCS != 0){ //wait until noone is in the critical section
		pthread_cond_wait(&isBeingAccessed ,&mutex1);
	}
			
	//printf("\nfiles to execute: %s", *infiles);
	
	strcpy(cpyFiles, files);
	
	//extract number
	temp = strtok (cpyFiles," ");
	copyTemp = malloc(strlen(temp));
	strcpy(copyTemp, temp);
	threadID = atoi(copyTemp);
	//printf(" threadId %d", threadID);

	//extract files
	temp = strtok (NULL," ");
	
	while (temp!=NULL){
		
		//send copy of file name to scan file 
		file = malloc(strlen(temp));
		strcpy(file, temp);
		//printf("\nfile extracted %s, thread %d",file, threadID);

	
		//printf("\nCURRENTLY EXECUTING FILE %s\n", file);
		//filepointer opens file
		fileReader = fopen(file, "r");
		if(fileReader == NULL){
			fprintf(stderr, "Can't open input file \n");
			exit(1);
		}
		
		anyoneInCS = 0;
		while (fscanf(fileReader, "%d", &number) != EOF){
			anyoneInCS = 1;
			//is the number larger than the largest prime so far in the file?
			if(number>largestPrime){
				//check if prime
				if(isPrime(number)){	
					//is the largest prime in the file greater than the largest prime among found among files?
					//critical section
					if(largestPrimeAmongFiles<number){
						//printf("\nLARGEST PRIME SO FAR! %d from file %s", number, file);	
						largestPrimeAmongFiles = number;
						largestPrime = number;
					}
										
				}
				anyoneInCS = 0;				
			}	
		}	
		temp = strtok (NULL, " ");
	}
	anyoneInCS = 0;
	pthread_cond_signal(&isBeingAccessed ) ;
	pthread_mutex_unlock( &mutex1 );

	//printf("\nthread %d is gonna exit\n", threadID);
	
	
	pthread_exit(NULL);

}


void *coordinatorFunction (int *inThreads){	
	int i;
	int j;
	int threads = *(inThreads);
	//this string of array holds the files the threads are gooing to excute. The files that thread 0 are
	//going to execute is in element 0
	char** filesExecute = (char**)malloc(sizeof(char*)*threads); 
	pthread_t workers[threads];
	int rc1;
	char* temp;
	int dummy = 0;
	pthread_attr_t attr;
	int rc;
	char* copyFileExec;
	//fprintf(stdout, "threadds:%d", threads);

	numberOfThreads = threads;
	//malloc
	for(j=0; j<threads; j++){
		filesExecute[j] = malloc(sizeof(char)*100);
	}

	//copying threadId into the filesexecute string
	for(i=0;i<threads;i++){
		temp = (char*) malloc(100);
		sprintf(temp, "%d", i);
		strcpy(*(filesExecute+i),temp );
		strcat(*(filesExecute+i)," ");
	}

	//assign worker	
	assignWorker(filesExecute, threads);
	
	//display
	for(i=0; i<threads; i++){
		fprintf(stdout, "\nthread: %s \n", *(filesExecute+i));
	}
	
	//initialize mutexc
	pthread_mutex_init(&mutex1 , NULL ) ;
	//init cond var
	pthread_cond_init (&isBeingAccessed, NULL);
	
	/* Initialize and set thread detached attribute */
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);



	//work!
	for(i=0; i<threads; i++){
		copyFileExec = malloc(strlen(*(filesExecute+i))+1);
		strcpy(copyFileExec,*(filesExecute+i) );
		//create the thread
		rc = pthread_create(&workers[i], &attr, threadWorker, &copyFileExec);
		if (rc) {
			 printf("ERROR; return code from pthread_create() is %d\n", rc);
			 exit(-1);
         }

		//pthread_join( workers[i], NULL); 
		fprintf(stdout, "\nThread:%d created", i);		
	}
	pthread_attr_destroy(&attr);

	
		//join threads
	for(i=0; i<threads; i++){
		rc = pthread_join( workers[i], NULL); 
		if (rc) {
			 printf("ERROR; return code from pthread_join() is %d\n", rc);
			 exit(-1);
		}

		fprintf(stdout, "\nThread:%d joined", i);	
					
	}
	pthread_mutex_lock( &mutex1 );
	printf("\nLARGEST PRIME IS %d\n",largestPrimeAmongFiles);
	pthread_mutex_unlock( &mutex1 );
	pthread_exit(NULL);
	
}


//this returns the string of array that holds the files the threads are going to be execute. The files that thread 0 are
//going to execute is in element 0
void assignWorker(char** filesExecute, int threadWorkers){
	//counter, which is also the filename location in fileName array
	int i = 1;
	int threadID = 1;
	//left over file...0 for no file left overs found
	int leftOver = 0;
	int j = 0;
		
	//assign each worker a file
	while(i<=NUMBER_OF_FILES){
		//are we assigning a left over file?
		if(leftOver == 0 && threadID<threadWorkers){
			threadID = i;
			//assign first thread with first file
			strcat(filesExecute[threadID-1], fileNames[i-1]);
		}else{//if there are left overs files, assign the rest of the files by reseting the i variable that controls the threadId
			if(leftOver==0){ //assign files to the first worker and so on
				leftOver = 1;
				threadID = 1;
			}else if(leftOver==1 && threadID<threadWorkers ){
				threadID ++;
			}else if(leftOver == 1 && threadID>=threadWorkers){
				threadID = 1;
			}
			//assign  thread with leftover file
			strcat(filesExecute[threadID-1], " ");
			strcat(filesExecute[threadID-1], fileNames[i-1]);
			
			//printf("\nThe new filexecutestring is %s!\n", filesExecute[threadID-1]);
		}
		
		i++;
	}
	//printf("\nDONE ASSIGNING WOTK\n");
	
}


//returns 1 if its prime else return 0
int isPrime(int inCheck){
	int i;
	int sqrtAns = sqrt(inCheck)+1;

	for(i=2; i<sqrtAns; i++){
		if(inCheck % i == 0)
			return 0;
	}
	return 1;
}


main(int argc, char* argv[]){
	int largestPrime;
	//number of threads	
	int threads ;
	pthread_t coordinator;
	//the coordinator thread return value (largest prime)
	void* result;	
	char *threadresult ;
	//used for time
	time_t start, stop;
   	clock_t ticks; 
	double count;
 
	
	//if user didn't supply enough input, terminate process
	if (argc != 2){
		fprintf(stdout, "Not enough or to many arguments!\n");
		exit(1);
	}
	
	threads = atoi(argv[1]);

	fprintf(stdout,"\nthreads: %d\n", threads);
	
	//time
   	time(&start);


	if(threads==1){
		largestPrime = executeSequentially();
		
		printf("\n largest prime is %d\n", largestPrime);
	}else if(threads>1 && threads<=NUMBER_OF_FILES){
		fprintf(stdout, "\nstart threads: %d\n", threads);

		pthread_create(&coordinator, NULL, coordinatorFunction, &threads);
		pthread_join(coordinator, &result); 
		
	}else if(threads>NUMBER_OF_FILES){
		printf("\nThe number of thread workers is greater than number of files (%d)!\n",NUMBER_OF_FILES );
		exit(1);
	}else{
		printf("\nInvalid argument!\n");
		exit(1);
	}
	time(&stop);
	printf("Finished in about %lf seconds. \n", difftime(stop, start));




}

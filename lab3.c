/* Maeda Hanafi
   9/21/10
   Using struct

   Maeda Hanafi
   9/23/10
   File I/O and Comman line arguments
*/

#include<stdio.h>
#include<stdlib.h>

typedef struct StudentData{ 
	char* name;
	char* major;
	double gpa;
}Student;

Student* readFile(int inSize, FILE *filePointer){
	Student* array = (Student*) malloc(inSize*sizeof(Student));
	int i, j = 0;
	char temp;
	double intTemp;
	char* stringTemp = malloc(51);

	//put poitner in second line
	fgets(stringTemp, 50, filePointer);

	for(i=0; i<inSize; i++){
		printf("\nname:");
		array[i].name = malloc(51);

		//read the whole name
		fgets(array[i].name, 50, filePointer); 
		printf("%s", array[i].name); 

		//read the whole major
		printf("major:");
		array[i].major = malloc(31);
		fgets(array[i].major, 30, filePointer); 
		printf("%s", array[i].major); 

		//scan double
		printf("GPA:");
		fscanf(filePointer, "%lf",&intTemp);
		printf("%lf",intTemp);
		array[i].gpa=intTemp;

		fscanf(filePointer, "%c",&temp);
				
	}
	
	return array;
}

void shuffle(Student* array, int inSize){
	Student temp1;
	Student temp2;
	int i=0, j;
	while(i<inSize){
		temp1 = array[i];
		if(i==inSize-1){
			break;
		}
		i++;
		temp2 = array[i];
		array[i] = temp1;
		array[i-1] = temp2;
		i++;
	}
	printf("\n\nNEW Array");
	for(i=0; i<inSize; i++){
		printf("\nname:%s",array[i].name);
		printf("major: %s",array[i].major);
		printf("GPA: %lf",array[i].gpa);
	}
}

void printToFile(Student* array, char outputFilename[], int inSize){
	FILE *outputPointer;
	int i;
	//open file	
	outputPointer = fopen(outputFilename, "w");

	if (outputPointer == NULL) {
	  fprintf(stderr, "Can't open output file %s!\n",
			  outputFilename);
	  exit(1);
	}

	printf("\n\nReading into array...");
	for(i=0; i<inSize; i++){
		fprintf(outputPointer,"name:%s",array[i].name);
		fprintf(outputPointer,"major: %s",array[i].major);
		fprintf(outputPointer,"GPA: %lf",array[i].gpa);
	}


}
//do arguments after file i/o
main(int argc, char* argv[]){
	int sizeOfArray=0;
	Student* array;
	FILE *filePointer;
	char* inputFile = malloc(argv[1]+1);
	char* outputFile= malloc(argv[2]+1);

	//if user didn’t supply enough input, terminate process
	if (argc != 3){
		printf("No input file or output file.\n");
		exit(1);
	}	
	
	strcpy(inputFile, argv[1]);
	strcpy(outputFile, argv[2]);
	
	//open file
	filePointer = fopen(inputFile, "r");
	if (filePointer == NULL) {
		fprintf(stderr, "Can't open input file \n");
		exit(1);
	}

	//read the first line to get array size. 
	fscanf(filePointer, "%d", &sizeOfArray);  

	//read data
	array = readFile(sizeOfArray, filePointer);

	shuffle(array, sizeOfArray);

	//print results to outfile
	printToFile(array, outputFile, sizeOfArray);
}
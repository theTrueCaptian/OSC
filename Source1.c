//Maeda Hanafi
//9/10/10
//reading from a file and removing the duplicates

#include<stdio.h>
#include<stdlib.h>

//size of the dynamic newArray
int sizeOfNewArray;

int* readInput(){
	//dynamic array
	int* numArray= malloc(100*sizeof(int));
		
	//check if not null then scan integers one by one
	int j;
	int x;
	for(j=0; j<100; j++){
		scanf("%d ",&x);
		//place the integer into numArray
		*(numArray+j) = x;
	}
	
	return numArray;
}
	
int sort(const void *x, const void *y) {
	return (*(int*)x - *(int*)y);
}

int* removeDups(int inArray[]){
	int i, j, lastSeenUnique;
	//arrays used to remove duplicates
	int sortArray[100], newArray[100];
	//the dynamic array to return to main
	int* returnArray;

	//copy orgArray to sortArray to not modify orgArray
	for(i=0; i<100; i++)
	    sortArray[i ]= inArray[i];

	qsort(sortArray, 100, sizeof(int), sort);

	//store the first element in newArray also to lastSeenUnique
	newArray[0] = sortArray[0];
	lastSeenUnique = sortArray[0];

	//remove duplicates
	j=1;
	for(i=1; i < 100; i++){
		//store the integer if it isn't a duplicate
		if(sortArray[i]!=lastSeenUnique){
			newArray[j]=sortArray[i];
			j++;
			//update lastSeenUnique
			lastSeenUnique = sortArray[i];
		}
	}

	//update the sizeOfNewArray
	sizeOfNewArray = j;
	//use sizeOfNewArray to determine the size of the dynamic array
	returnArray = malloc(sizeOfNewArray*sizeof(int));

	//copy contents of new array to returnArray
	for(i=0; i<j; i++)
	    *(returnArray+i)=newArray[i];

	return returnArray;
} 

int main(){
	int* orgArray;
	int* newArray;
	int i;

	orgArray = readInput();
	newArray = removeDups(orgArray);

	//display results
	printf("\nOrigianl Array : ");
	for(i = 0; i < 100; i++){
		printf("%d ", *(orgArray+i));
	}
	printf("\nNew Array : ");
	for(i = 0; i < sizeOfNewArray; i++){
		printf("%d ", *(newArray+i));
	}


}
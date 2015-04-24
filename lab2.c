/* Maeda Hanafi
   9/21/10
   Using struct
*/
#include<stdio.h>
#include<stdlib.h>
typedef struct StudentData{ 
	char* name;
	char* major;
	double gpa;
}Student;

Student* readFile(int inSize){
	Student* array = (Student*) malloc(inSize*sizeof(Student));
	int i, j = 0;
	char temp;
	double intTemp;

	
	for(i=0; i<inSize; i++){
		printf("\nname:");
		array[i].name = malloc(51);

		//read the whole name
		fgets(array[i].name, 50, stdin); 
		printf("%s", array[i].name); 

		//read the whole major
		printf("major:");
		array[i].major = malloc(31);
		fgets(array[i].major, 30, stdin); 
		printf("%s", array[i].major); 

		//scan double
		printf("GPA:");
		scanf("%lf",&intTemp);
		printf("%lf",intTemp);
		array[i].gpa=intTemp;
		scanf("%c",&temp);
				
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

main(){
	int sizeOfArray=0;
	char temp;
	int i, j = 0;
	Student* array;
	scanf("%d", &sizeOfArray);
	scanf("%c", &temp);

	//read data
	array = readFile(sizeOfArray);
	shuffle(array, sizeOfArray);
}
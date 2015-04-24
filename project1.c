/*
	Maeda Hanafi
	Project 1
	10/26/10
	Shell
	To run the shell 
*/

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>

extern char** environ;

//displays curernt directory. returns 0 if user issues exit command
int currentDir(){
	printf("\nPress ^c or ctrl c to return original prompt!\n Your current directory is:\n");
	long size = 0;
	char *buf = "";
	char *ptr = "";
	char* userInput = malloc(256);

	//get current dirrectory
	size = pathconf(".", _PC_PATH_MAX);
	if ((buf = (char *)malloc((size_t)size)) != NULL){
		ptr = getcwd(buf, (size_t)size);
	}
	//display curr dir
	printf("%s$ ", ptr);

	//get user input
	fgets(userInput, 255, stdin);
	//printf("\nCommand read is %s\n", userInput);

	//determine whether it is an exit command
	if(atoi(userInput)==27){
		return 0;
	}

	//process input
	processInput(userInput);
	return 1;

}

//decides whether to execute sequentially or not
void processInput(char* userInput){
	//variables used to tokenize the input
	char* tokUserInput = NULL;
	int i=0, j=0, m=0;
	char* command;
	char* temp;
	char* copyTemp;
	int childpid;
	int status;
	//executable command
	char*  execCommand = malloc(255);
	
	if(sequential(userInput)==1){//if the userInput has @ or only one input
		//copy contents in userInput to tokUserInput
		tokUserInput = malloc(strlen(userInput));
		for(i=0; i<strlen(userInput); i++){
			tokUserInput[i] = userInput[i];
			if(tokUserInput[i] == '\n')
				tokUserInput[i] = '\0';
		}

		//tokenize tokUserInput not userInput
		temp = strtok(tokUserInput, "@");
		while(temp!=NULL){
			//printf("\nexecuting command:%s \n",temp);
			//execute a command one by one
			processInputSequentially(temp);
			temp = NULL;
			m++;
			//copy contents in userInput to tokUserInput in case of changes made in tokUserInput by strtok
			tokUserInput = malloc(strlen(userInput));
			for(i=0; i<strlen(userInput); i++){
				tokUserInput[i] = userInput[i];
				if(tokUserInput[i] == '\n')
					tokUserInput[i] = '\0';
			}
			//adjust the strtok pointer of tokUserInput
			temp = strtok (tokUserInput, "@");
			for(j=0; j<m; j++)
				temp = strtok (NULL, "@");
		}
	}else if(sequential(userInput)==0){//multple processes

		//copy contents in userInput to tokUserInput
		tokUserInput = malloc(strlen(userInput));
		for(i=0; i<strlen(userInput); i++){
			tokUserInput[i] = userInput[i];
			if(tokUserInput[i] == '\n')
				tokUserInput[i] = '\0';
		}

		//tokenize userInput
		temp = strtok(tokUserInput, "!");
		i=0;
		while(temp!=NULL){			
			//printf("\nexecuting command:%s \n",temp);
			
			childpid = fork();
			if(childpid==0){
				//printf("child executing command:%s \n",temp);				
				processInputMProcesses(temp);
				exit(0);
			}else if(childpid>0){
				wait(&status);
			}
			
			m++;
			//copy contents in userInput to tokUserInput in case of changes made in tokUserInput by strtok
			tokUserInput = malloc(strlen(userInput));
			for(i=0; i<strlen(userInput); i++){
				tokUserInput[i] = userInput[i];
				if(tokUserInput[i] == '\n')
					tokUserInput[i] = '\0';
			}
			//adjust the strtok pointer of tokUserInput
			temp = strtok (tokUserInput, "!");
			for(j=0; j<m; j++)
				temp = strtok (NULL, "!");
		}

	}
}

//if return 1 then the userInput must be executed sequentially else execute with multiple processes
int sequential(char* userInput){
	int i=0;
	for(i=0; i<strlen(userInput); i++){
		if(userInput[i]=='!')
			return 0;
		if(userInput[i]=='@')
			return 1;
	}
	return 1;

}

//forks child process to execute commands sequentially
void processInputSequentially(char* userInput){
	//path variables
	size_t size;
	char* envVarPATH = malloc(size);
	char* copyEnvVarPATH = malloc(size);
	//variables related to coming up with the correct command combo
	char *temp;
	char *possibleCommand = malloc(300);
	char* command;
	int fail;
	int i, j, m=0;
	//child processes variables
	int childProcess;
	int pid, status;
	char* copyUserInput;
	
	copyUserInput = malloc(strlen(userInput));
	for(i=0; i<strlen(userInput); i++){
		copyUserInput[i] = userInput[i];
	}

	//extract command from userInput
	command = strtok (copyUserInput," ");
	//printf("\ncommand ready to be executed: %s\n", command);

	//scan the directories specified in Path to search for executable
	copyEnvVarPATH = envVarPATH = getenv("PATH");
	//printf("\nPath= %s\n", envVarPATH);

	//copy contents in envVarPATH to copyEnvVarPATH
	copyEnvVarPATH = malloc(strlen(envVarPATH));
	for(i=0; i<strlen(envVarPATH); i++){
		copyEnvVarPATH[i] = envVarPATH[i];
	}
	//printf("copyEnvVarPATH= %s\n", copyEnvVarPATH);

	//tokenize to find the correct command combination 
	temp = strtok (copyEnvVarPATH,":");
	while (temp != NULL){
		//come up with a possible command
		possibleCommand = malloc(300);
		strcat(possibleCommand, temp);
		strcat(possibleCommand, "/");
		strcat(possibleCommand, command);

		//verify the possible command
		fail = access(possibleCommand, X_OK);

		if(fail == 0)//if the possible command is found, break
			break;
		//increment counter 
		m++;

		//copy envVarPATH to copyEnvVarPATH in case of changes made in copyEnvVarPATH by strtok
		copyEnvVarPATH = malloc(strlen(envVarPATH));
		for(i=0; i<strlen(envVarPATH); i++){
			copyEnvVarPATH[i] = envVarPATH[i];
		}

		//adjust the strtok pointer of copyEnvVarPATH
		temp = strtok (copyEnvVarPATH, ":");
		for(j=0; j<m; j++)
			temp = strtok (NULL, ":");
		//printf("copyEnvVarPATH= %s\n", copyEnvVarPATH);
	}
	
	if(fail ==0){ //success, forks a child process
		pid = fork();
		if(pid==0){
			execute(possibleCommand, userInput);
			//printf("\nDone with executing\n");
		}
		if(pid>0){
	        	wait(&status);
		}
		
	}else{ //no command has been found or no access to command
		printf("Error in executing...\n");
	}
}

void processInputMProcesses(char* userInput){
	//path variables
	size_t size;
	char* envVarPATH = malloc(size);
	char* copyEnvVarPATH = malloc(size);
	//variables related to coming up with the correct command combo
	char *temp;
	char *possibleCommand = malloc(300);
	char* command;
	int fail;
	int i, j, m=0;
	//child processes variables
	int childProcess;
	int pid, status;
	char* copyUserInput;
	
	copyUserInput = malloc(strlen(userInput));
	for(i=0; i<strlen(userInput); i++){
		copyUserInput[i] = userInput[i];
	}
	
	//extract command from userInput
	command = strtok (copyUserInput," ");
	//printf("\ncommand ready to be executed: %s\n", command);

	//scan the directories specified in Path to search for executable
	copyEnvVarPATH = envVarPATH = getenv("PATH");
	//printf("\nPath= %s\n", envVarPATH);

	//copy contents in envVarPATH to copyEnvVarPATH
	copyEnvVarPATH = malloc(strlen(envVarPATH));
	for(i=0; i<strlen(envVarPATH); i++){
		copyEnvVarPATH[i] = envVarPATH[i];
	}
	//printf("copyEnvVarPATH= %s\n", copyEnvVarPATH);

	//tokenize to find the correct command combination 
	temp = strtok (copyEnvVarPATH,":");
	while (temp != NULL){
		//come up with a possible command
		possibleCommand = malloc(300);
		strcat(possibleCommand, temp);
		strcat(possibleCommand, "/");
		strcat(possibleCommand, command);

		//verify the possible command
		fail = access(possibleCommand, X_OK);

		if(fail == 0)//if the possible command is found, break
			break;
		//increment counter 
		m++;

		//copy envVarPATH to copyEnvVarPATH in case of changes made in copyEnvVarPATH by strtok
		copyEnvVarPATH = malloc(strlen(envVarPATH));
		for(i=0; i<strlen(envVarPATH); i++){
			copyEnvVarPATH[i] = envVarPATH[i];
		}

		//adjust the strtok pointer of copyEnvVarPATH
		temp = strtok (copyEnvVarPATH, ":");
		for(j=0; j<m; j++)
			temp = strtok (NULL, ":");
		//printf("copyEnvVarPATH= %s\n", copyEnvVarPATH);
	}
	
	if(fail ==0){ //success, forks a child process
		//printf("\nSucess: the command is in %s\n", possibleCommand);
		execute(possibleCommand, userInput);
		
		
	}else{ //no command has been found or no access to command
		printf("Error in executing...\n");
		possibleCommand = NULL;
	}

}
//mainCommand is the first part of a user command 
void execute(char* mainCommand, char* command){
	char *tokenizedInput[100];
	char* temp;
	int i = 0;

	//tokenize command and put results in tokenizedInput
	temp = strtok (command," ");
	while (temp != NULL){
		tokenizedInput[i]= temp;
				
		i++;
		temp = strtok (NULL, " ");
	}
	tokenizedInput[i]= NULL;

	//execute command
	//printf("\nmainCommand %s, command %s\n", mainCommand, command);
	execv(mainCommand, tokenizedInput);
	//execl(mainCommand, tokenizedInput, NULL);
	
	//killChildProcess
	exit(0);
}

int main(void){
	int exit;
	for(;;){
		/*shell display the current directy*/
		exit = currentDir();
		if(exit==0){
			break;
		}
	}
	
}
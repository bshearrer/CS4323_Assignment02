// CS 4323 Mini Group Project
// Group E
// Members: Tyler Krebs (tyler.krebs@okstate.edu), Cindy Liu(cindy.liu@okstate.edu)
// 			Lee Repa(lrepa@okstate.edu), Brandon Shearrer (bshearr@okstate.du)

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>


# define MAX_COMMAND_LENGTH 100
# define MAX_HISTORY_LENGTH 20

// Set up arrays to hold the current history and tasks in background
int histCount = 0;
char history[MAX_HISTORY_LENGTH][MAX_COMMAND_LENGTH];

int backgroundCount = 0;
char background[MAX_HISTORY_LENGTH][MAX_COMMAND_LENGTH];

// Function for reading in and processing commands given to shell
int processCommand(char input[], char *args[], int *inBackground) {

	// Read the command entered into input and log the number of bytes read in
	int bytesRead = read(STDIN_FILENO, input, MAX_COMMAND_LENGTH);
	
	int toReturn = 0;

	if (bytesRead == 0) {
		// Nothing to read: exit
		exit(0);
	}

	else if (bytesRead < 0) {
		// Error reading in
		printf("Error reading command. \n");
		exit(-1);
	}

	int argsIndex = 0;   // Tracks the index of the next open position in args[][]
	int startIndex;      // Tracks the index of the start of this command/argument, separated by spaces or tabs-
					     // startIndex < 0 indicates that previous character was also a space or tab


	// Iterate through each character in the input
	for (int i = 0; i < bytesRead; i++) {

		// Read the character at input[i]
		switch (input[i]) {

			// Space or tab seperates arguments
			case ' ':
			case '\t':

				// If previous character was not a space or tab (meaning it was the end of a command or argument)
				if (startIndex >= 0) {

					// Store the command/arg that just ended at args[argsIndex] and increment argsIndex
					args[argsIndex] = &input[startIndex];
					argsIndex++;
				}

				// Replace the space/tab with a null char so that args[argsIndex] correctly stores ONLY this argument
				input[i] = '\0';
				// Reset startIndex to indicate that previous char was a space/tab
				startIndex = -1;
				break;

			// Endline character indicates the end of input
			case '\n':

				// If previous character was not a space or tab (meaning it was the end of a command or argument)
				if (startIndex >= 0) {

					// Store the command/arg that just ended at args[argsIndex] and increment argsIndex
					args[argsIndex] = &input[startIndex];
					argsIndex++;
				}

				// Replace the space/tab with a null char so that args[argsIndex] correctly stores ONLY this argument
				input[i] = '\0';
				// Indicate the end of the args array by inserting null string at next index
				args[argsIndex] = NULL;
				break;
				
			// "&" flag is used to indicate that command should be run in background
			// If this is found, set inBackground flag, and don't add this argument to args index
			case '&':
			
				*inBackground = 1;
				startIndex = -1;
				toReturn = 2;
					
				break;

			// Default case: any other character is treated as part of a command or argument
			default:

				// If startIndex < 0, meaning last character was a space, this is the start of a new argument, so store this index
				if (startIndex < 0) {
					startIndex = i;
				}
		}
	// End for loop
	}

	// In case an endline character was never read in
	args[argsIndex] = NULL;


	/* PROCESSING FOR DIFFERENT COMMANDS */

	// cd
	if (strcmp(args[0], "cd") == 0) {

		// Change directory to filepath given as first argument: catch errors
		if (chdir(args[1]) < 0) {
			printf("Error changing to this directory.\n");
			exit (-1);
		}
		toReturn++;
	}

	// history
	else if (strcmp(args[0], "history") == 0) {

		if (histCount == 0) {
			printf("\nHistory is empty.\n");
			return -1;
		}

		// Iterate through history[][] histCount number of times and print out entries
		for (int i = 0; i < histCount; i++) {
			printf("%d. %s\n", i + 1, history[i]);
		}
	}
	
	// background
	else if (strcmp(args[0], "background") == 0) {

		if (backgroundCount == 0) {
			printf("No jobs in background.\n");
			return -1;
		}

		// Print out background tasks
		int i = 0;
		while (i < backgroundCount) {
			printf("%d. %s\n", i + 1, background[i]);
			i++;
		}
		toReturn++;		
	}
	// Input redirection operator eg.sort < file1.txt
	else if(strcmp(args[0], "sort") == 0){
		if(args[1][0] == '<'){
			char data[10][80],temp[32];
			int i,j;
			char name[10];

				FILE *fp = fopen(args[2],"r");
				if(fp == NULL)
				{
					printf("Cannot open file  \"%s\"\n", args[2]);
					return -1;
				}
				for(i = 0; !feof(fp); i++){
					fscanf(fp, "%s", data[i]);

				}
				printf("File input\n");
				for(i=0; i < 8; i++){
					printf("%s ", data[i]);
				}
				printf("\n");
				fclose(fp);
				int n = 8;
				for(i=0;i<n-1;i++){
					for(j=i+1;j<n;j++){
						if(strcmp(data[i],data[j])>0){
							strcpy(temp,data[i]);
							strcpy(data[i],data[j]);
							strcpy(data[j],temp);
							/*
							for(int t=0; t < 8; t++){
								printf("%s ", data[i]);
							}
							*/
						}
					}
				}
				printf("File output\n");
				for(i=0; i < 8; i++){
					printf("%s ", data[i]);
				}
				printf("\n");
				printf("finish\n");
				toReturn++;
		}
	}

	else if(strcmp(args[0], "less") == 0){
		FILE *fp;
		int num = 0;
		char *arrLines[100];
		char line[50];
		int i = 0;

		//open file
		fp = fopen(args[1], "r");
		if(fp == NULL) {
			printf("Cannot open file \n");
		}

		//read file contents
		while(fgets(line, sizeof line, fp) != NULL){
			printf("%s", line);
		}
		fclose(fp);
		toReturn++;
	}

	else if(strcmp(args[0], "more") == 0){
		FILE *fp;
		int num = 0;
		char *arrLines[100];
		char line[50];
		int i = 0;

		//open file
		fp = fopen(args[1], "r");
		if(fp == NULL) {
			printf("Cannot open file \n");
		}

		//read file contents
		while(fgets(line, sizeof line, fp) != NULL){
			printf("%s", line);
		}
		fclose(fp);
		toReturn++;
	}
	
		/* ADD MORE COMMANDS HERE */
		
		
	//cat test1 > test2
	// cat test1 >> test2
		for(int i = 1; args[i] != NULL; i++){
			char c;
			if(args[i][0] == '>' && args[i][1] != '>') {
				FILE *fp1 = fopen(args[i-1],"r");
				if(fp1 == NULL)
					{
						printf("file1 open error.\n");
						return -1;
					}
				FILE *fp2 = fopen(args[i+1],"w");
				if(fp2 == NULL)
					{
						printf("file2 open error.\n");
						return -1;
					}
				while ((c = fgetc(fp1)) != EOF)
				{	
					//printf("reading.\n");
					fputc(c,fp2);

				}
				fclose(fp1); 
				fclose(fp2); 

			}
			if(args[i][0] == '>' && args[i][1] == '>') {
				FILE *fp3 = fopen(args[i-1],"r");
				if(fp3 == NULL)
					{
						printf("file open error.\n");
						return -1;
					}
				FILE *fp4 = fopen(args[i+1],"a+");
				while ((c = fgetc(fp3)) != EOF)
				{
					fputc(c,fp4);
				}
				fclose(fp3); 
				fclose(fp4); 

			}

		}

	//cat file1 | grep “apple” 
	for(int i = 1; args[i] != NULL; i++){
			//char data1[10];
			//char a;
			char *c;
			char line[50];
			
			if(args[i][0] == '|'&& strcmp(args[i-2], "cat") == 0) {
				FILE *fp1 = fopen(args[i-1],"r");
				
				if(fp1 == NULL)
				{
					printf("Cannot open file  \"%s\"\n", args[1]);
					return -1;
				}
				
				
				if(strcmp(args[i+1], "grep") == 0){
					char *p =args[i+2];
					int j = 0;
					for(int k=0;k<20;k++){
						if(args[i+2][k] != '"'){
							args[i+2][j++]=args[i+2][k];
						}
					}
					while(fgets(line, sizeof line, fp1) != NULL){
						c = line;
						//printf("%s", p);
						if(strstr(c,p)){
							printf("get_string_value : %s\n", line);
						}

					}
					
					
					
				}
				
			}
				
				
	}
			

			

		
	

	// Update history- Shift all elements in the history up one in the array
	for (int i = MAX_HISTORY_LENGTH - 1; i > 0; i--) {
		strcpy(history[i], history[i - 1]);
	}

	// Add input to history[0]
	strcpy(history[0], input);

	int i = 1;
	while (args[i] != NULL) {
		strcat(history[0], " ");
		strcat(history[0], args[i]);
		i++;
	}
	histCount++;
	if (histCount >= MAX_HISTORY_LENGTH) {
		histCount = MAX_HISTORY_LENGTH;
	}
	
	//If in background, add to running list
	if (*inBackground == 1) {
		backgroundCount++;
		for (int i = MAX_HISTORY_LENGTH - 1; i > 0; i--) {
			strcpy(background[i], background[i - 1]);
		}

		// Add input to background[0]
		strcpy(background[0], input);

		int i = 1;
		while (args[i] != NULL) {
			strcat(background[0], " ");
			strcat(background[0], args[i]);
			i++;
		}
		if (backgroundCount >= MAX_HISTORY_LENGTH) {
			backgroundCount= MAX_HISTORY_LENGTH;
		}
	}
	return toReturn;
}

// Remove a task from background list when it finishes
void updateBackground(char *args[]) {
	char input[] = "";
	strcat(input, args[0]);
	int i = 1;
	while (args[i] != NULL) {
		strcat(input, " ");
		strcat(input, args[i]);
		i++;
	}
	printf("\n%s\n", input);
	for (int i = 0; i < MAX_HISTORY_LENGTH; i++) {
		if (strcmp(background[i], input) == 0) {
			for (int j = i; j < MAX_HISTORY_LENGTH; j++)  {
				strcpy(background[i + 1], background[i]);
			}
			backgroundCount--;
			break;
		}
	}
}
	

/* Main function */
int main(void) {

	// Set up variables- string arrays for input, arguments, current directory and history, and thread pid

	char *args[MAX_COMMAND_LENGTH / 2 + 1];

	char input[MAX_COMMAND_LENGTH];

	//gets current users okey username
	char *userName;
	userName = (char *)malloc(10*sizeof(char));
	userName = getlogin();

	char currentDir[1024];
	char homeDir[1024];
	int inBackground = 0;  // All processes are foreground by default
	pid_t pid, pidt;

	//set homeDir to the home directory
	getcwd(homeDir, sizeof(homeDir));

	// Start main loop of program
	while (1) {

		//Default setting is operations in foreground
		inBackground = 0;
		
		getcwd(currentDir, sizeof(currentDir));
		if (strcmp(currentDir, homeDir) == 0) {
			printf("%s@CS432shell:%s~$ ", userName, currentDir);
			fflush(stdout);
		}
		else {
			printf("%s@CS432shell:%s$ ", userName, currentDir);
			fflush(stdout);
		}

		// Get the next command entered and fork a process for it
		// processReturn 0- exec command in foreground
		// processReturn 1- custom command (non-exec) in foreground
		// processReturn 2- exec command in background
		
		int processReturn = processCommand(input, args, &inBackground);

		if (processReturn == 0 || processReturn == 2)  {

			pid = fork();

			// If pid < 0, fork failed
			if (pid < 0) {
				printf("Error: Fork failed.\n");
			}

			// If pid == 0, we are in child process
			else if (pid == 0) {

				// Execute command stored in args[0]- catch errors
				if (execvp(args[0], args) == -1) {
					printf("Unknown Command\n");
				}
			}

			// Parent process
			else {

				// If process is in foreground, wait for child to finish
				if (inBackground == 0) {
					wait(NULL);
				}
				else {
					// Check if child still running- if it isn't, run updateBackground to remove it
					int status;
					pid_t return_pid = waitpid(pid, &status, WNOHANG);
					if (return_pid == -1) {
					printf("Error with background task %s\n", args[0]);
					}
					else if (return_pid == pid) {
						updateBackground(args);
					}
					
				}
					
			}
		}
	}
}

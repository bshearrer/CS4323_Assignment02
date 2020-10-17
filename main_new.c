// CS 4323 Mini Group Project
// Group E - Tyler Krebs, Cindy Liu, Lee Repa, Brandon Shearrer

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>

# define MAX_COMMAND_LENGTH 100
# define MAX_HISTORY_LENGTH 20

/* TODO: inBackground is a flag for whether parent process should wait for child or not- */
/* inBackground == 1 means parent will NOT wait (in background), inBackground == 0       */
/* means parent WILL wait (in foreground)- It is passed as a pointer so it can be        */
/* changed within processCommand depending on the syntax of the command (not done yet)   */

int histCount = 0;
char history[MAX_HISTORY_LENGTH][MAX_COMMAND_LENGTH];

// Function for reading in and processing commands given to shell
int processCommand(char input[], char *args[], int *inBackground) {

	// Read the command entered into input and log the number of bytes read in
	int bytesRead = read(STDIN_FILENO, input, MAX_COMMAND_LENGTH);

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
	// Input redirection operator eg.sort < file1.txt
	else if(strcmp(args[0], "sort") == 0){
			char data[10][80],temp[32];
			int i,j;
			char name[10];
			
				FILE *fp = fopen("file1.TXT","r");
				if(fp == NULL)
				{
					printf("file open error.\n");
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
		
	}
	/* ADD MORE COMMANDS HERE */

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
	return 0;
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

		inBackground = 0;
		//Print out directory information- include ~ if currentDir == homeDir
		/* NOTE- Figure out how to get username- placeholder here */
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

		if (processCommand(input, args, &inBackground) >= 0) {

			pid = fork();

			// If pid < 0, fork failed
			if (pid < 0) {
				printf("Fork failed.\n");
			}

			// If pid == 0, we are in child process
			else if (pid == 0) {

				// Execute command stored in args[0]- catch errors
				if (execvp(args[0], args) == -1) {
					printf("Command not recognized");
				}
			}

			// Parent process
			else {

				// If process is in foreground, wait for child to finish
				if (inBackground == 0) {
					wait(NULL);
				}
			}
		}
	}
}

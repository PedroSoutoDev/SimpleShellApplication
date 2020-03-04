/*******************************************************************************************************
Class: CSC-415-01 Spring 2020
Name: Pedro Souto
Student ID: 918412864
Project: Assignment 1 - Simple Shell
File: main.c
Description: Simple shell thats runs on top of the regular command-line interpreter for Linux. The shell reads lines of user input, then parses and executes the commands by forking/creating new processes.
**********************************************************************************************************/

// Required for printf() and getchar()
#include <stdio.h>
// Required for strcmp()
#include <string.h>
// Required for isspace()
#include <ctype.h>
// Required for waitpid() and WEXITSTATUS()
#include <sys/stat.h>
// Required for execvp() and fork()
#include <unistd.h>
// Required for malloc() and free()
#include <stdlib.h>

// Function prototypes
void shellLoop(char* promptInput);
int executeCommand(char **args);

// Buffer size in bytes
#define BUFFERSIZE 1024

/**
*  int main(int argc, char *argv[])
*
*  @param argc Number of paramaters passed into program. Will be at least 1, which is the name of the program
*  @param argv Array of parameters passed into program. Second paramater may be prompt
*
*  Decription: Base function called when shell is opened. Simply determines what the the prompt will be, and passes it shellLoop()
 */
int main(int argc, char *argv[]) {
    // If no value is specified, use default prompt
    if (argc == 1) {
        shellLoop(NULL);
    }
    // If a value is specified, use it as the prompt
    else if(argc == 2) {
        shellLoop(argv[1]);
    }
    // If too many values are give, exit the program
    else {
        printf("Too many variables given. Please enter only one prompt.\n");
        return -1;
    }
}

/**
* void shellLoop(char* promptInput)
*
* @param promptInput Prompt shown before each command entry on the shell
*
* Description: Main loop of the entire shell
* 1. Prompts user for input
* 2. Parses and passes correct arguments to executeCommand()
*/
void shellLoop(char* promptInput){
    char userInput[BUFFERSIZE];
    char *argv[BUFFERSIZE];
    char *token;
    
    // Set prompt based on value passed in
    char* prompt;
    if (promptInput == NULL) {
        prompt = ">";
    }
    else {
        prompt = promptInput;
    }


    while (1) {
        // Print prompt and get user input
        printf("%s ", prompt);
        fgets(userInput, BUFFERSIZE, stdin);

        // Override the last char of input to be a null-terminator
        userInput[strlen(userInput) - 1] = '\0';
        
        // Tokenize user input into arguments list
        int argc = 0;
        token = strtok(userInput, " ");
        while (token != NULL) {
            argv[argc] = token;
            argc++;
            token = strtok(NULL, " ");
        }
        
        // If the first argument is "exit" then exit the program with code 0
        if(strcmp(argv[0],"exit") == 0) {
            exit(0);
        }
        
        // Null the next args pointer, so that execvp knows when to stop looking for more args
        argv[argc] = NULL;
        
        // Call execute command using the args list built from user input
        executeCommand(argv);
    }
    return;
}

/**
* int executeCommand(char **args)
*
* @param argv List of arguments used to execute command
*
* Description: Taken in arguments to execute command
* 1. Fork a child process to execute the command
* 2. Child process executes command via execvp()
* 3. Parrent will wait until child is finished executing, then continue back to shellLoop()
**/
int executeCommand(char *argv[]){
    pid_t pid = fork();
    // If pid == 0, then it is a child. Child will execute command, not parent
    if (pid == 0) {
        if (execvp(argv[0], argv) < 0){
            printf("Invalid command, please enter a valid command.\n");
        }
    }
    // If pid > 0, then it is a parent. Parent will wait until child is done
    else if (pid > 0) {
        int exitStatus;
        waitpid(pid, &exitStatus, 0);
        printf("Child %d, exited with %d\n", pid, WEXITSTATUS(exitStatus));
        
    }
    // If pid < 0, then there was an error when forking a child
    else {
        printf("Failed fork when executing a command.");
        return -1;
    }
    return 0;
}

/*

Authors: Andrew Ingram, Andrew Kalathra, Caleb Joiner

Class: Operating Systems 4302

Assignment: Group Project

Instructor: Bo Sun

Due: 11/27/2023 | 11:00 AM

----------------------------------------------

- Problem - 
Create a shell program that allows the user to execute commands exposed
by the OS or through path variables. (gcc, ls, dir, . . .)

----------------------------------------------

- Solution - 
> TODO: Include stuff like what algorithm was used to solve problem

----------------------------------------------

- Operational Requirements - 
Language:
    C

Compilation:
    Please compile this program on a Linux based system. This program was made using Linux Mint Cinnamon distro.
    To compile, go to project directory and use the following terminal input:
        gcc -lpthread project_part_two.c -o program_part_two
    To run the program, stay in the project directory and run this terminal input:
        ./program_part_two

Input Information:
    > TODO: describe input here

----------------------------------------------

- Incomplete Features / Missing Requirements / Bugs - 
> TODO: Talk about issues here

*/


#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>


// struct that holds the arguments passed to the command execution thread
typedef struct {
    char * commandString; // a string of the command to be parsed/processed/executed
} commandThreadArguments;



// prints the string indicating the user can type a command
void printCommandString(char computerName[], char workingDirectory[]) {
    
    // declare string for terminal to display
    char commandPrompt[2048] = "";
    
    // concat information in format: "SampleComputerName:~/This/is/a/sample/directory/$ "
    strcat(commandPrompt, computerName);
    strcat(commandPrompt, ":~");
    strcat(commandPrompt, workingDirectory);
    strcat(commandPrompt, "$ ");
    
    // print string to terminal
    printf("%s\n", commandPrompt);
}



void * executeCommand(void * args) {
    
    // get arguments from args void pointer
    commandThreadArguments * arguments = args;

    // get data from arguments
    char * commandString = arguments->commandString;

    // free arguments struct from memory
    free(arguments);

    // execute command
    printf("Executing command: %s\n", commandString);
}



int main() {

    // declare vars for later
    char computerName[512]; // holds the name of the computer to display in terminal input
    char workingDirectory[1024]; // holds the current the directory the terminal is executing in



    // TODO: get computer name
    strcpy(computerName, "SampleComputerName");

    // TODO: get working directory
    strcpy(workingDirectory, "/Sample/Directory");



    // print the message that informs the user they may begin typing a command
    printCommandString(computerName, workingDirectory);

    // main loop for checking for commands
    while (1) {

        // get commandString from terminal
        char commandString[] = "Command String";



        // declare vars for making command-processing thread
        pthread_t thread;
        commandThreadArguments * args;

        // init arguments for command-processing thread
        args = malloc(sizeof * args);
        args->commandString = commandString;

        // create thread to process command
        if (pthread_create(&thread, NULL, &executeCommand, (void *) args)) {
            // error creating thread

            // free arguments struct from heap
            free(args);
        } else {

            // join command execution thread
            pthread_join(thread, NULL);

        }


        // TODO: Remove break statement for final build
        // DEBUGGING PURPOSES ONLY
        // REMOVE "break;" IF NOT TESTING ONE ITERATION OF MAIN LOOP
        break;
    }

    // clean program
    printf("Finished program\n");

    return 0;
}



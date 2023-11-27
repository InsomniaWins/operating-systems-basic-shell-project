/*

Authors: Andrew Ingram, Andrew Kalathra, Caleb Joiner

Class: Operating Systems 4302

Assignment: Group Project

Instructor: Bo Sun

Due: 11/27/2023 | 10:00 AM

----------------------------------------------

- Problem - 
Create a shell program that allows the user to execute commands exposed
by the OS or through path variables. (gcc, ls, dir, . . .)

----------------------------------------------

- Solution - 
> Run infinite loop that reads terminal commands
> process custom commands that cannot be in the child process
> execute inputted command in child process
> wait for child process
> continue loop

----------------------------------------------

- Operational Requirements - 
Language:
    C++

Compilation:
    Please compile this program on a Linux based system with an up-to-date c++ compiler (g++).
    Make sure there is an 'out' directory in the main project directory before compiling.
    To compile, go to project directory and use the following terminal input:
        PART ONE: g++ "src/part_1.cpp" -lpthread -o "out/part_1"
        PART TWO: g++ "src/part_2.cpp" -lpthread -o "out/part_2"
    To run the program, stay in the project directory and run this terminal input:
        PART ONE: ./"out/part_1"
        PART TWO: ./"out/part_2"


Input Information:
    
    To exit the shell, type "quit" (without quotes)
    To change directories, type "cd <directory>" (without quotes, where <directory> is the directory relative to working directory)
    Any command in the path should work (i.e "g++")
    Most linux commands should work with this shell (i.e. "ls")
    ! Running programs directly with shell does not work ("./<program>")
----------------------------------------------

- Incomplete Features / Missing Requirements / Bugs - 

    Add header file for shared.cpp (could not get program to compile with seperate header file)
    Cannot specify argument that includes spaces by putting quotes (someCommand "name with spaces")
    Shell will add empty arguments if there are any trailing whitespaces before or after command

*/

#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <pwd.h>
#include "shared.cpp"

// exeutes the given commandType struct
int executeCommand(struct commandType *);


int main(){

    // start shell main loop
    while (true) {
        
        // print user input prompt
        printPrompt();



        // get user input string
        std::string inputCommand;
        getline(std::cin, inputCommand);



        // parse input into command struct
        commandType command;
        parseCommand(inputCommand, &command);


        #ifdef DEBUGGING
            for (int i = 0; i < command.arguments.size(); i++) {
                std::cout << "command argument [" << i << "] \"" << command.arguments[i] << '"' << '\n';
            }
        #endif


        
        // create new string to hold the command name in lower-case
        std::string commandNameLowerCase(command.arguments[0]);

        // convert the string to lower-case
        for (int i = 0; i < commandNameLowerCase.length(); i++) {
            if (isalpha(commandNameLowerCase[i])) {
                commandNameLowerCase[i] = tolower(commandNameLowerCase[i]);
            }
        }

        // if user types any case version of quit, then close shell program
        if(commandNameLowerCase == "quit") {
            // exit main loop and finish shell process
            break;
        }


        // if user wants to change working directory
        if (commandNameLowerCase == "cd") {
            std::string newDirectory = command.arguments[1];
            std::string workingDirectory = getWorkingDirectory();

            #ifdef DEBUGGING
                std::cout << "working directory: " << workingDirectory << '\n';
            #endif

            chdir((std::string(workingDirectory) + '/' + newDirectory).c_str());

            #ifdef DEBUGGING
                workingDirectory = getWorkingDirectory();
                std::cout << "new working directory: " << workingDirectory << '\n';
            #endif

            continue;
        }



        // make child process to execute command
        pid_t pid = fork();
        if (pid == 0) {
            
            int returnStatus = executeCommand(&command);
            exit(returnStatus);

        } else {
            // wait for child to finish executing command
            int returnStatus;
            waitpid(pid, &returnStatus, 0);

            if (returnStatus == EXIT_SUCCESS) {
                
                #ifdef DEBUGGING
                    std::cout << "process finished executing successfully!";
                #endif


            } else {
                if (returnStatus == EXIT_FAILURE) {

                #ifdef DEBUGGING
                    std::cout << "process exited with FAILURE code!";
                #endif

                }
            }
        }
    }

    //Shell termination

    std::cout << "finished shell program\n";
}


// executes command and prints output
int executeCommand(struct commandType * command) {


    // get vector of all path directories
    std::vector<std::string> paths = parsePath();
    #ifdef DEBUGGING
        for (int i = 0; i < paths.size(); i++) {
            std::cout << "path dir [" << i << "] \"" << paths[i] << '"' << '\n';
        }
    #endif



    // get path of command
    command->path = lookupPath(command->arguments[0], paths);

    // ask user for input again if path of command could not be found
    if (command->path.empty()) {
        std::cout << "command: " << command->arguments[0] << " is not a recognized command!\n";
        return EXIT_FAILURE; // changed from 'break' to 'continue' (break would end program, not ask for input again)
    }

    #ifdef DEBUGGING
        std::cout << "path of requested program/command: \"" << command->path << "\"\n";
    #endif



    // get command as a full string
    std::string fullCommandString = command->path + "/" + command->arguments[0];
    for (int i = 0; i < command->arguments.size()-1; i++) {
        fullCommandString = fullCommandString + ' ' + command->arguments[i+1];
    }

    #ifdef DEBUGGING
        std::cout << "full command string: " << fullCommandString << "\n";
    #endif

    // create a pipe stream to get output of command
    FILE * filePointer;
    filePointer = popen(fullCommandString.c_str(), "r");

    // if failed to create pipe, let user know return failure
    if (filePointer == NULL) {
        std::cout << "Unknown problem ocurred while executing that command!\n";
        return EXIT_FAILURE;
    }

    // print command output
    char outputText[2048];
    while (fgets(outputText, sizeof(outputText), filePointer) != NULL) {
        std::cout << outputText;
    }

    // close pipe
    pclose(filePointer);

    // return success
    return EXIT_SUCCESS;
}
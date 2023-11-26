// compile command: (run from main project directory)
//    g++ "part_1/part_1.cpp" -lpthread -o "out/part_1"


// [Andrew K] : 
// I did this actually:
//      g++ pt1.cpp minishell.h -o part1

// [Andrew I] : (responding to above comment)
// I think maybe your file structure is messed up? (maybe)
// try deleting your local files and redownloading the source from github to fixe file structure


// run command:
//    ./"out/part_1"


// external headers/includes
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


// custom headers/includes
#include "minishell.h"

// keep this! > allows using strings, vectors, etc. without typing 'std::'
// also, please put "std::cout" and "std::cin" rather than just "cout" and "cin" to avoid ambiguity errors
using namespace std;

// parses the path variable for absolute directories of commands/programs
vector<string> parsePath();

// looks through path environment variables for inputted command name
// returns empty string if no command path is found
string lookupPath(string, vector<string>);

// parses command into a commandType structure
int parseCommand(string, struct commandType *);

// print the input prompt
void printPrompt();

// this will be used to delete extra spaces
string eraseEndSpace(string);

// exeutes the given commandType struct
int executeCommand(struct commandType *);

// gets working directory
string getWorkingDirectory();


int main(){

    // start shell main loop
    while (true) {
        
        // print user input prompt
        printPrompt();



        // get user input string
        string inputCommand;
        getline(cin, inputCommand);



        // parse input into command struct
        commandType command;
        parseCommand(inputCommand, &command);


        #ifdef DEBUGGING
            for (int i = 0; i < command.arguments.size(); i++) {
                std::cout << "command argument [" << i << "] \"" << command.arguments[i] << '"' << '\n';
            }
        #endif


        
        // create new string to hold the command name in lower-case
        string commandNameLowerCase(command.arguments[0]);

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
            string newDirectory = command.arguments[1];
            string workingDirectory = getWorkingDirectory();

            #ifdef DEBUGGING
                std::cout << "working directory: " << workingDirectory << '\n';
            #endif

            chdir((string(workingDirectory) + '/' + newDirectory).c_str());

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


// TODO: test if function works with absolute paths, might not
// returns directory of command inputted
// searches through given array of directories for command directory to return
// if none match, return empty string
string lookupPath(string commandName, vector<string> paths){
    
    // param descriptions:
    //   commandName - name of command inputted
    //   directories - array of directories to check if command (i.e. "gcc") is inside

    // if first character of command begins with '/' character
    // then the command is an absolute path
    if (commandName[0] == '/') {
        // if the directory exists, return the directory
        //access() is part of the unistd header file (for linux)
        //R_OK test file and grants reading rights
        if (access(commandName.c_str(), R_OK) == 0) {
            // returns a copy of commandName to avoid accidental tampering with string commandName
            return string(commandName);
        } else { // else, return empty
            return string();
        }
    }


    // past this point must mean that the command inputted is not an absolute path


    // loop through path variables
    for (int i = 0; i < paths.size(); i++) {
        // if path variable matched the inputted command name, return path variable directory

        string currentAbsoluteDirectory = paths[i] + '/' + commandName;

        #ifdef DEBUGGING
            std::cout << "checking directory: \"" << currentAbsoluteDirectory << "\" for command/program \n";
        #endif

        if (access(currentAbsoluteDirectory.c_str(), R_OK) == 0) {
            // command exists in directory, return new string of current directory
            return string(paths[i]);
        }

    }


    // command is not an absolute directory
    // command could not be found in PATH
    // so, the command could not be found
    // therefore, print error and return empty
    return string();
}



// prints a prompt for user to enter a command
void printPrompt(){
    passwd * user = getpwuid(getuid());
    string username = user->pw_name;

    string workingDirectory = getWorkingDirectory();
    std::cout << '\n' << username << "~" << workingDirectory << "$ ";
}



// gets and parses the path string by using ':' as a delemiter/splitter
// then returns a vector of all directories in path string
vector<string> parsePath() {

    // init directories vector
    vector<string> directories;

    // get path string
    string pathString = getenv("PATH");

    // parse
    int copyPosition = 0; // index of character to start substr method
    for (int currentCharacterIndex = 0; currentCharacterIndex < pathString.length(); currentCharacterIndex++) {
        // if delimeter char
        if (pathString[currentCharacterIndex] == ':') {
            // get directory as string
            int directoryLength = currentCharacterIndex - copyPosition;
            string directoryString = pathString.substr(copyPosition, directoryLength);
            
            // add to return vector
            directories.push_back(directoryString);

            // new substr position to after the ':' character
            copyPosition = currentCharacterIndex+1;
        }
    }

    return directories;
    //error check 
}


// parses command input string into a commandType structure
int parseCommand(string commandString, struct commandType * commandStruct) {

    // param descriptions:
    //   commandString - the whole line of text inputted to the terminal
    //   commandStruct - pointer to struct that will be filled with command info

    // Solution: can do this (below) or can just make an understandable method
        // commandString.erase(std::find_if(commandString.rbegin(), commandString.rend(), std::bind1st(std::not_equal_to<char>(), ' ')).base(), commandString.end());
    // TODO: the eraseEndSpace function crashes when a command is given with no arguments
    //       we might also want it to clip all types of whitespace rather than just ' ' char
    //       TODO: the function also cuts off the last command argument for somereason
    //commandString = eraseEndSpace(commandString); // commented out until fixed

    // parse
    int copyPosition = 0; // index of character to start substr method
    for (int currentCharacterIndex = 0; currentCharacterIndex < commandString.length(); currentCharacterIndex++) {
        
        // check if current character is whitespace
        bool charIsWhitespace = false;
        for (int i = 0; i < sizeof(WHITESPACE); i++) {
            if (WHITESPACE[i] == commandString[currentCharacterIndex]) {
                charIsWhitespace = true;
                break;
            }
        }

        // if current character is whitespace, then the next arg is found
        if (charIsWhitespace) {
            // get arg as string
            int argLength = currentCharacterIndex - copyPosition;
            string argString = commandString.substr(copyPosition, argLength);
            
            // add arg to struct
            commandStruct->arguments.push_back(argString);

            // new substr position to after the whitespace
            copyPosition = currentCharacterIndex+1;
        } else if (currentCharacterIndex+1 == commandString.length()) {
            // end of commandString reached

            int argLength = commandString.length() - copyPosition;
            string argString = commandString.substr(copyPosition, argLength);

            // add final arg to struct
            commandStruct->arguments.push_back(argString);

            // dont need to update copyPosition, because end of string occured
        }
    }

    return 0;
}



string eraseEndSpace(string inputString){
    string shortString = inputString;
    int pos = shortString.length()-1;

    while(shortString[pos-1] != ' '){
        shortString.erase(pos);
        pos--;
    }
    
    return shortString;
}



// executes command and prints output
int executeCommand(struct commandType * command) {

    // TODO: allow users to execute program with "./programname" (programname is replaced with the name of the program to execute)

    // get vector of all path directories
    vector<string> paths = parsePath();
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
    string fullCommandString = command->path + "/" + command->arguments[0];
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



// gets the current working directory
string getWorkingDirectory() {

    char workingDirectory[2048];
    getcwd(workingDirectory, 2048);
    return string(workingDirectory);

}

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

#ifndef WHITESPACE
    #define WHITESPACE " ,\t\n"
#endif

// structure that holds information of command inputted into shell
struct commandType {
    
    // the directory of the program/command (i.e. "/bin/home/andrew" <- if program was in this directory)
    std::string path;

    // vector of command name AND arguments as strings (i.e. ["gcc", "c", "-o", "compiled program name"])
    std::vector<std::string> arguments;
};


// gets the current working directory
std::string getWorkingDirectory() {

    char workingDirectory[2048];
    getcwd(workingDirectory, 2048);
    return std::string(workingDirectory);

}

// prints a prompt for user to enter a command
void printPrompt(){
    passwd * user = getpwuid(getuid());
    std::string username = user->pw_name;

    std::string workingDirectory = getWorkingDirectory();
    std::cout << '\n' << username << "~" << workingDirectory << "$ ";
}

// returns directory of command inputted
// searches through given array of directories for command directory to return
// if none match, return empty string
std::string lookupPath(std::string commandName, std::vector<std::string> paths){
    
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
            return std::string(commandName);
        } else { // else, return empty
            return std::string();
        }
    }


    // past this point must mean that the command inputted is not an absolute path


    // loop through path variables
    for (int i = 0; i < paths.size(); i++) {
        // if path variable matched the inputted command name, return path variable directory

        std::string currentAbsoluteDirectory = paths[i] + '/' + commandName;

        #ifdef DEBUGGING
            std::cout << "checking directory: \"" << currentAbsoluteDirectory << "\" for command/program \n";
        #endif

        if (access(currentAbsoluteDirectory.c_str(), R_OK) == 0) {
            // command exists in directory, return new string of current directory
            return std::string(paths[i]);
        }

    }


    // command is not an absolute directory
    // command could not be found in PATH
    // so, the command could not be found
    // therefore, print error and return empty
    return std::string();
}


// gets and parses the path string by using ':' as a delemiter/splitter
// then returns a vector of all directories in path string
std::vector<std::string> parsePath() {

    // init directories vector
    std::vector<std::string> directories;

    // get path string
    std::string pathString = getenv("PATH");

    // parse
    int copyPosition = 0; // index of character to start substr method
    for (int currentCharacterIndex = 0; currentCharacterIndex < pathString.length(); currentCharacterIndex++) {
        // if delimeter char
        if (pathString[currentCharacterIndex] == ':') {
            // get directory as string
            int directoryLength = currentCharacterIndex - copyPosition;
            std::string directoryString = pathString.substr(copyPosition, directoryLength);
            
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
int parseCommand(std::string commandString, struct commandType * commandStruct) {

    // param descriptions:
    //   commandString - the whole line of text inputted to the terminal
    //   commandStruct - pointer to struct that will be filled with command info



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
            std::string argString = commandString.substr(copyPosition, argLength);
            
            // add arg to struct
            commandStruct->arguments.push_back(argString);

            // new substr position to after the whitespace
            copyPosition = currentCharacterIndex+1;
        } else if (currentCharacterIndex+1 == commandString.length()) {
            // end of commandString reached

            int argLength = commandString.length() - copyPosition;
            std::string argString = commandString.substr(copyPosition, argLength);

            // add final arg to struct
            commandStruct->arguments.push_back(argString);

            // dont need to update copyPosition, because end of string occured
        }
    }

    return 0;
}
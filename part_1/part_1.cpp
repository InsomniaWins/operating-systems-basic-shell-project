// compile command: (run from main project directory)
//    g++ "part_1/part_1.cpp" -lpthread -o "out/part_1"


// [Andrew K] : 
// I did this actually:
//      g++ pt1.cpp minishell.h -o part1

// [Andrew I] : (responding to above comment)
// I think maybe your file structure is messed up? (maybe)
// try deleting your local files and redownloading the source from github


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


// custom headers/includes
#include "minishell.h"

// namespace for ease of use of standard library
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

//this will be used to delete extra spaces
string eraseEndSpace(string);


int main(){

    //Shell initialization
    
    // get vector of all path directories
    vector<string> paths = parsePath();
#ifdef DEBUGGING
    for (int i = 0; i < paths.size(); i++) {
        cout << "path dir [" << i << "] \"" << paths[i] << '"' << '\n';
    }
#endif

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

// TODO: appears that last argument gets cut off (maybe something to do with the parseCommand function)
#ifdef DEBUGGING
        for (int i = 0; i < command.arguments.size(); i++) {
            cout << "command argument [" << i << "] \"" << command.arguments[i] << '"' << '\n';
        }
#endif


        // if command is quit command (no need for new process/thread)
        // TODO: make quit command NOT case sensitive
        string hold = command.arguments[0];
        
        
        if((hold == "quit") || (hold == "Quit") || (hold == "QUIT")) {
            // exit main loop and finish shell process
            break;
        }


        // get path of command
        string commandPath = lookupPath(command.arguments[0], paths);

        // ask user for input again if path of command could not be found
        if (commandPath.empty()) {
            cout << "command: " << command.arguments[0] << " is not a recognized command!\n";
            continue; // changed from 'break' to 'continue' (break would end program, not ask for input again)
        }
        
        cout << "commandPath: " << commandPath << "\n";

        int pid = fork();
        if (pid == 0) {
            
            char const **arg_list = (char const **) malloc((command.arguments.size() + 1) * sizeof(char *));
            if (arg_list == NULL) {
                exit(EXIT_FAILURE);
            }
            arg_list[0] = command.name.c_str();
            for (int i = 0; i < command.arguments.size(); i++) {
                arg_list[(i + 1)] = command.arguments[i].c_str();
            }
            execv(commandPath.c_str(), (char * const *) arg_list);
            free(arg_list); arg_list = NULL;

            cout << "done\n";

            exit(EXIT_SUCCESS);
        } else {
            // wait for child to finish executing command
            waitpid(pid, NULL, 0);
        }
    }

    //Shell termination

    cout << "finished shell program\n";
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

        // uncomment to check the directories that were checked
#ifdef DEBUGGING
        cout << "checking directory: \"" << currentAbsoluteDirectory << "\" for command/program \n";
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
    cout << "\nEnter you command: \n";
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
// TODO: last argument given appears to be cut off when put into struct
int parseCommand(string commandString, struct commandType * commandStruct) {

    // param descriptions:
    //   commandString - the whole line of text inputted to the terminal
    //   commandStruct - pointer to struct that will be filled with command info

    // we could use below, but it would require using the algorithm header which causes ambiguity of all cin and cout calls
        // Solution: can do this (below) or can just make an understandable method
        // commandString.erase(std::find_if(commandString.rbegin(), commandString.rend(), std::bind1st(std::not_equal_to<char>(), ' ')).base(), commandString.end());
    // TODO: the eraseEndSpace function doesnt work when a command is given with no arguments
    //       we might also want it to clip all types of whitespace rather than just ' ' char
    // it crashes
    commandString = eraseEndSpace(commandString);

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
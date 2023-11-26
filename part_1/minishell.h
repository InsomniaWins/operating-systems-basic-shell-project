#include <string>
#include <vector>

#define WHITESPACE      " .,\t\n"

#ifndef NULL
#define NULL 
#endif

using namespace std;

// structure that holds information of command inputted into shell
struct commandType {
    // name of the command (i.e. "gcc" or "g++" or "cd" or "ls" etc . . .)
    string name;
    // vector of arguments as strings (i.e. ["c", "-o", "compiled program name"])
    vector<string> arguments;
    char *argv[64];
    //we are going to need this for the execution
    //64 uis the original max arg length
};
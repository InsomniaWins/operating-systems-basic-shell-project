#include <string>
#include <vector>

#define WHITESPACE      " ,\t\n" // I removed the '.' char from this list to allow changing directory to ".." (parent directory)
//#define DEBUGGING       "comment this out to disable debug messages"

#ifndef NULL
#define NULL 
#endif

using namespace std;

// structure that holds information of command inputted into shell
struct commandType {
    
    string path;

    // vector of command name AND arguments as strings (i.e. ["gcc", "c", "-o", "compiled program name"])
    vector<string> arguments;



    //char *argv[64];    // I dont think we need this char*[] anymore - Andrew I
            //we are going to need this for the execution
            //64 uis the original max arg length
};
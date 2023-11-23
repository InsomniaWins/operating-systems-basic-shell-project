#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "minishell.h"
#include <string>
#include <string.h>


using namespace std;
string *lookupPath(char **, char **);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
void printPrompt();
void readCommand(string);

int main(){

    /*Shell initialization*/
    //"enter command"
    
    
    //pathv
    string dIR[MAX_PATH_LEN];
    //this will literally be a string of commands
    //aka the directory
    int parseReturnCode = parsePath(dIR); /*Get directory paths from PATH*/
    //we can error check with this var
    //this is where Path var goes

    while(1){
        printPrompt();

        /*Read the command line and parse it*/
        //readCommand(commandLine);
        string commandLine;
        cin>>commandLine;

        parseCommand(commandLine, &command);

        /*Get the full pathname for the file*/
        command.name = lookupPath(command.argv, pathv);
        if(command.name == NULL){
            /*report error*/
            printf("Error: NULL command\n");
            continue;
        }
        /*if(command.name.toLower() == "quit"){
            break;
        }*/

        /*create child and execute the command*/
        int id = fork();
        /*Wait for the child to terminate*/
        
        if(id == 0){//fork to child
            //run the command

        }else{
            waitpid(id, NULL, NULL);
            //hopefully this works on linux
        }
        //child all in main


    }

    /*Shell termination*/

    //freeing memory
    //delete thePath;


}

void printPrompt(){
    char promptString[] = "Enter you command: "; 
    printf("%s", promptString);
}

void readCommand(string buffer){
    gets(buffer);
}

int parsePath(string dirs[]){

   char* pathEnvVar;
   

    for(int i=0; i<MAX_ARGS; i++){
        dirs[i] = nullptr;
        //null pointer string to initialize the array
    }
   
   pathEnvVar = getenv("PATH");
    //PATH is the library of the commands
    //aka path vars
    //the commands are programs
   string thePath(pathEnvVar);
 
    //return string path
    //...
    int copyPos = 0;
    int availablePosInArray = 0;
    for(int i=0; i<thePath.length(); i++){
        if(thePath[i] == ':'){
            dirs[availablePosInArray] = thePath.substr(copyPos, i-1);
            copyPos = i + 1;
            availablePosInArray++;
        }

    }


    //error check 
}

string *lookupPath(char **argv, char **dir){
    char *result;
    char pName[MAX_ARG_LEN];

    if(*argv[0] == '/'){
        //...

        //this is for absolute paths
        //return Null or the path char*[]
    }

    for(int i=0; i<MAX_PATHS; i++){
        //...
        //if found, should return in here
    }

    //file not found
    fprintf(stderr, "%s: command no found\n", argv[0]);
    return NULL;
}
int parseCommand(string cLine, struct command_t *cmd){
    int argc = 0;
    //argument counter
    /*string clPtr;
    clPtr = cLine;*/
    cmd->argv[argc] = (char *)malloc(MAX_ARG_LEN);
    int i = 0;
    string hold ="";
    whileLoop: 
    while(cLine[i] != NULL){

        for(int j=0; j<strlen(WHITESPACE); j++){
            if(cLine[i] == WHITESPACE[j]){
                cmd->argv[argc] = hold;
                hold ="";
                goto whileLoop;
            }
        }
        hold+=cLine[i];
        i++;
    }
    //word.word.word
    int copyPos = 0;
    int availablePosInArray = 0;
    while((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL){

    }
    /*for(int i=0; i<thePath.length(); i++){
        if(thePath[i] == ':'){
            dirs[availablePosInArray] = thePath.substr(copyPos, i-1);
            copyPos = i + 1;
            availablePosInArray++;
        }

    }*/
    
}
/*int parseCommand(string cLine, struct command_t *cmd) {
/* Determine command name and construct the parameter list 

    int argc;
    char **clPtr;

/* Initialization 
    clPtr = &cLine;
    argc = 0;
    cmd->argv[argc] = (char *) malloc(MAX_ARG_LEN);
/* Fill argv[] 
    while((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
        cmd->argv[++argc] = (char *) malloc(MAX_ARG_LEN);
    }

/* Set the command name and argc 
    cmd->argc = argc-1;
    cmd->name = (char *) malloc(sizeof(cmd->argv[0]));
    strcpy(cmd->name, cmd->argv[0]);
//    printf("HHH: %s\n", cmd->name);
    return  1;	
}*/
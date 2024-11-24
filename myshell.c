#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>  
#include "LineParser.h"

#define MAX_LENGTH 2048
#define MAX_PATH_SIZE 4096

//for reading string - array of chars:

void execute(cmdLine *pCmdLine){
    //recieves a parsed line
    PID = fork(); //maintian shell alive
    if (PID == -1){
        perror("Frok failed");
        freeCmdLines(pCmdLine);
        _exit(1);
    }

    else if (PID == 0){ //child process
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1){ //trying to run
        perror("Error in execvp");
        freeCmdLines(pCmdLine);
        _exit(1); //to 'kill' the child :(
    
    }
    else{ //the parent
        if (pCmdLine->blocking ==1) {
            int status = 0;
            waitpid(PID, &status, 0);
        }

    }
} 
    
}


int main(int argc, char **argv){

    boolean debug = true;

    cmdLine *parsedLine;

    if (argc>1 && strcmp(argv[1], "-d") == 0){
        debug = false;
    }
    

    while (1){ //for infinte loop
        char current_dir[MAX_PATH_SIZE]; 
        if (getcwd(current_dir, MAX_PATH_SIZE) == NULL){
            perror("Fail in getcwd");
            _exit(1);
        }

        if (debug){
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: ", current_dir);
        }

        fprintf(stdout, "%s>", current_dir); //print the current directory using prompt symbol
        char infile[MAX_LENGTH]; //string is array of chars

        fgets(infile, MAX_LENGTH, stdin); //reading from stdin to line - max length is 2048

        
        parsedLine = parseCmdLines(infile); //cmd structure    
        if (strcmp (parsedLine->arguments[0], "quit") == 0 ){
            freeCmdLines(parsedLine); //avoding memory leak
            break;
        }

        if (strcmp (parsedLine->arguments[0], "cd") == 0){
            if (chdir(parsedLine->arguments[1]) == -1){ //trying yto change directory of the shell
                perror("Fail in chdir");
            }
            freeCmdLines(parsedLine);
            continue;
        }

        

        execute(parsedLine); 
        freeCmdLines(parsedLine); 
    }
}
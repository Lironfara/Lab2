#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>  
#include <stdbool.h>
#include "LineParser.h"
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>



#define MAX_LENGTH 2048
#define MAX_PATH_SIZE 4096
char input[MAX_LENGTH]; //string is array of chars
bool debug = false;

//for reading string - array of chars:
int getChildPID(cmdLine *pCmdLine){
    return atoi(pCmdLine->arguments[1]);
}

void handleStop(cmdLine *pCmdLine){
    if (pCmdLine->argCount < 2){
        perror("Fail to stop process");
        return;
    }
    int childPID = getChildPID(pCmdLine);
    if (kill(childPID, SIGSTOP) == -1){
        perror("Fail to stop process");
    }
    else {
        fprintf(stdout, "Process %d was stopped\n", childPID);
    }
}

void handleTerm(cmdLine *pCmdLine){
    if (pCmdLine->argCount < 2){
        perror("Fail to terminate process");
        return;
    }
    int childPID = getChildPID(pCmdLine);
    if (kill(childPID, SIGTERM) == -1){
        perror("Fail to terminate process");
    }
    else {
        fprintf(stdout, "Process %d was terminated\n", childPID);
    }

}

void handleWake(cmdLine *pCmdLine){
    if (pCmdLine->argCount < 2){
        perror("Fail to wake process");
        return;
    }
    int childPID = getChildPID(pCmdLine);
    if (kill(childPID, SIGCONT) == -1){
        perror("Fail to wake process");
    }
    else {
        fprintf(stdout, "Process %d was woken\n", childPID);
    }
} 

void handleCD(cmdLine *pCmdLine){
    if (pCmdLine->argCount < 2){
        perror("Fail to change directory");
        return;
    }
    if (chdir(pCmdLine->arguments[1]) == -1){
        perror("Fail to change directory");
    }
    else {
        fprintf(stdout, "Directory was changed\n");
    }
     freeCmdLines(pCmdLine);

}


void execute(cmdLine *pCmdLine){

    // terms
        if (strcmp (pCmdLine->arguments[0], "stop") == 0){
            handleStop(pCmdLine);
        }

         else if (strcmp (pCmdLine->arguments[0],"wake") == 0){
            handleWake(pCmdLine);
        }

        else if (strcmp (pCmdLine->arguments[0],"term") == 0){
            handleTerm(pCmdLine);
        }

        else if (strcmp (pCmdLine->arguments[0],"cd") == 0){
            handleCD(pCmdLine);           
        }
    //end of terms

        else{
            int PID = fork();
            if (PID == -1){
            perror("Frok failed");
            freeCmdLines(pCmdLine);
            _exit(1);
            }

            else if (PID == 0){ //child process
            //adding files:
            if (pCmdLine->inputRedirect != NULL){
                printf("Input file: %s\n", pCmdLine->inputRedirect);
                FILE *inputFile = fopen(pCmdLine->inputRedirect, "r");
                if (inputFile == NULL){
                    perror("Fail to open input file");
                    _exit(1);
                }
                dup2(fileno(inputFile), STDIN_FILENO); //to read from the file
                fclose(inputFile);
            }

            if (pCmdLine->outputRedirect != NULL){
                printf("Output file: %s\n", pCmdLine->outputRedirect);
                FILE *outputFile = fopen(pCmdLine->outputRedirect, "w");
                if (outputFile == NULL){
                    perror("Fail to open output file");
                    _exit(1);
                }
                dup2(fileno(outputFile), STDOUT_FILENO); //to write to the file
                fclose(outputFile);
            }

            if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1){ //trying to run
            perror("Error in execvp");
            freeCmdLines(pCmdLine);
            _exit(1); //to 'kill' the child :(
            }
        }
            else{ //I am the father - getting back my child ID
                if (pCmdLine->blocking ==1) {
                int status = 0;
                waitpid(PID, &status, 0); //waiting for the childPID to finish
                }

            }
        }

    //recieves a parsed line
    
} 
    



int main(int argc, char **argv){

    cmdLine *parsedLine;

    if (argc>1 && strcmp(argv[1], "-d") == 0){
        debug = true;
    }
    

    while (1){ //for infinte loop

        if (debug){
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s", input);
        }

        char current_dir[MAX_PATH_SIZE]; 
        if (getcwd(current_dir, MAX_PATH_SIZE) == NULL){
            perror("Fail in getcwd");
            _exit(1);
        }

    

        fprintf(stdout, "%s>", current_dir); //print the current directory using prompt symbol

        fgets(input, MAX_LENGTH, stdin); //reading from stdin to line - max length is 2048

      


        if (strcmp (input,  "quit\n") == 0 ){
            break;
        }
        
        parsedLine = parseCmdLines(input); //cmd structure    
        

        execute(parsedLine); 
    }
    freeCmdLines(parsedLine); 

}
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
//for reading string - array of chars:

void execute(cmdLine *pCmdLine){

        if (strcmp (pCmdLine->arguments[0], "stop") == 0){
            int childPID = atoi(pCmdLine->arguments[1]);
            if (kill(childPID, SIGSTOP) == -1){
                perror("Fail to stop process");
            }
            else {
                fprintf(stdout, "Process %d was stopped\n", childPID);
            }
        }

         else if (strcmp (pCmdLine->arguments[0],"wake") == 0){
            int childPID = atoi(pCmdLine->arguments[1]);
            if (kill(childPID, SIGCONT) == -1){
                perror("Fail to wake process");
            }
            else {
                fprintf(stdout, "Process %d was woken\n", childPID);
            }
        }

        else if (strcmp (pCmdLine->arguments[0],"term") == 0){
            int childPID = atoi(pCmdLine->arguments[1]);
            if (kill(childPID, SIGTERM) == -1){
                perror("Fail to terminate process");
            }
            else {
                fprintf(stdout, "Process %d was terminated\n", childPID);
            }
        }

        else if (strcmp (pCmdLine->arguments[0], "cd") == 0){
            if (chdir(pCmdLine->arguments[1]) == -1){ //trying yto change directory of the shell
                perror("Fail in chdir");
            }
            freeCmdLines(pCmdLine);
        }

        else{
            int PID = fork(); //maintian shell alive
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
        }
            else{ //the parent
                if (pCmdLine->blocking ==1) {
                int status = 0;
                waitpid(PID, &status, 0);
                }

                }
        }

    //recieves a parsed line
    
} 
    



int main(int argc, char **argv){

    bool debug = true;

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

    

        fprintf(stdout, "%s>", current_dir); //print the current directory using prompt symbol
        char input[MAX_LENGTH]; //string is array of chars

        fgets(input, MAX_LENGTH, stdin); //reading from stdin to line - max length is 2048

        if (debug){
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s", input);
        }


        if (strcmp (input,  "quit\n") == 0 ){
            break;
        }
        
        parsedLine = parseCmdLines(input); //cmd structure    
        

        execute(parsedLine); 
        freeCmdLines(parsedLine); 
    }
}
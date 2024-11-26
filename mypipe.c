#include <stdio.h>   
#include <stdlib.h>   
#include <unistd.h>  
#include <string.h>

#define MAX_LENGTH 2048

int main(){

    char buffer[MAX_LENGTH];

    int pipefd[2]; 
    if (pipe(pipefd) ==-1){  //for read and write
    //pipefd[0] is the read and pipefd[1] is the write
        perror("Pipe failed");   
        exit(1);
    }

    int PID = fork(); 
    
    if (PID == -1){
        perror("Frok failed");
        exit(1);
    }

    else if (PID == 0){ //for child
        write(pipefd[1], "hello", strlen("hello"));
    }

    else{ //for parent
        read(pipefd[0], buffer, MAX_LENGTH);
    }
}
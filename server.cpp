#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <sstream>
#include <errno.h>
#include <sys/resource.h>

using namespace std;

void error(char *msg) {
    perror(msg);
    exit(1);
}
int execute (char **command, int newsockfd)
{
    // change file descriptor to print to client rather than std output
    dup2(newsockfd, STDOUT_FILENO);
    // change file descriptor to print to client rather than std err
    dup2(newsockfd, STDERR_FILENO);
    int w;
    int status;

    // create a new identical child process
    w = fork();

    if (w == -1) {
        // the fork has returned failure
        printf("\nFork Failed\n");
        return -1;
    } else if (w == 0) {
        // change file descriptor to print to client rather than std output
        dup2(newsockfd, STDOUT_FILENO);
        // change file descriptor to print to client rather than std err
        dup2(newsockfd, STDERR_FILENO);
        // only the child should be in here
        execvp(command[0], command);
        // execvp only returns if an error has occurred
        return -1;
    }

    // collects return values of functions to check whether execution failed.
    int returnVal = 0;
    // check to make sure that fork worked

    // keep trying waitpid
    while (returnVal != w) {
        // capture return value of wait (should return child id number if successful)
        returnVal = waitpid(w, &status, 0);
        // if not successful
        if (returnVal < 0) {
            char buffer[256];
            strerror_r(errno, buffer, 256);
            printf("\nChild Error\n");
            break;
            // did process return normally?
        } else if (WIFEXITED(status)) {
            // did process exit successfully?
            if (WEXITSTATUS(status) != 0) {
                printf("\nTerminated with exit code %u\n", WEXITSTATUS(status));
            }
            // did process return abnormally?
        } else if (WIFSIGNALED(status)) {
            printf("\nChild terminated due to signal %u\n", WTERMSIG(status));
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {

    // check for correct number of args
    if (argc < 2) {
        printf("ERROR, please provide port");
        exit(1);
    }
    // in order:
    // file descriptor, file descriptor, port number,
    // and n is number of characters read or written
    int sockfd, newsockfd, portno, n;
    socklen_t clilen; // client address length
    char buffer[256]; // char buffer to store input/output strings
    struct sockaddr_in serv_addr, cli_addr; // internet address
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a socket
    if (sockfd < 0)
        error("ERROR opening socket");

    // now set the server address values
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); // set port number (after converting to int)
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to the port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // listen for socket connection (5 is the max # of sockets that can bind)
    listen(sockfd, SOMAXCONN);

    clilen = sizeof(cli_addr);

    // block until connection established
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    // change file descriptor to print to client rather than std output
    dup2(newsockfd, STDOUT_FILENO);
    // change file descriptor to print to client rather than std err
    dup2(newsockfd, STDERR_FILENO);
    if (newsockfd < 0)
        error("ERROR on accept");
    // come back here after we get a stats request
    LOOP:
    // continually accept input
    while (true) {
        bzero(buffer, 256); // clear buffer
        n = read(newsockfd, buffer, 255); // read bytes sent from client

        if (n < 0)
            error("ERROR reading from socket");
        if (strncmp(buffer, "stats", 5) == 0) {
            int who = RUSAGE_CHILDREN;
            struct rusage usage;
            int ret = getrusage(who, &usage);
            int secondTime = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
            int milTime = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
            printf("Total time is %s seconds and %s milliseconds.\n", secondTime, milTime);
            goto LOOP;
        }
        // remove the newline symbol
        buffer[strlen(buffer) - 1] = '\0';
        int numSpaces = 0;
        for(int i = 0; i < strlen(buffer); i++) {
            if(buffer[i] == ' ')
                numSpaces++;
        }
        // gets the command (the first word of the buffer)
        char *str = strtok(buffer, " ");
        // +2 -> one for words and one for null at the end
        int totalWords = numSpaces + 2;
        char* args[totalWords];
              //  = (char**)malloc(totalWords * sizeof(char*));
        args[0] = str;
        // put the rest of the arguments in args
        for(int i = 1; i < totalWords - 1; i++) {
            args[i] = strtok(NULL, " ");
        }
        // set the last word in args to NULL for execvp
        args[totalWords - 1] = NULL;
        // let's try executing the code
         int retval = execute(args, newsockfd);

        if (retval < 0) {
            error("ERROR executing command");
        }
            //bzero(buffer, 256);
            // THIS IS WHERE WE RUN THE INPUT AND INSERT THE OUTPUT OF THAT
            // INTO OUR BUFFER!!
            /*n = write(newsockfd, buffer, strlen(buffer)); // write to the socket
            if (n < 0)
                error("ERROR writing to socket");*/
            //n = write(newsockfd, "I got your message", 18);
            /*if (n < 0)
                error("ERROR writing to socket");*/
           // free(args);
        }
        return 0;
    }

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

using namespace std;

void error(char *msg) {
    perror(msg);
    exit(1);
}
int execute (char **command)
{
    int w;
    int status;
    /*istringstream iss (str);
    std::vector < string > command;
    //Command string can contain the main command and a number of command line arguments.
    do
    {
        string sub;
        iss >> sub;
        command.push_back (sub);
    }
    while (iss);
    char **args = (char **) malloc (command.size () * sizeof (char *));
    for (int i = 0; i < command.size (); i++)
    {
        args[i] = strdup (command[i].c_str ());
    }
    args[command.size () - 1] = 0;*/

    // Please complete the code below to run the command with the specified arguments.
    // The args array already contains the parsed command and the array.
    // It should print "child exited successfully" if the process finished correctly
    // Otherwise it should print "child Error"
    // Quotes are for informational purpose only and does not need to appear in the final output.
    // Ensure that you check for error during the fork step, the exec step and the wait step.

    // create a new identical child process
    w = fork();

    if (w == -1) {
        // the fork has returned failure
        printf("\nFork Failed\n");
        return -1;
    } else if (w == 0) {
        // only the child should be in here
        printf("Command: %s Length: %u\n", command[0], sizeof(command)/4);
        printf("Command List:\n");
        for (int i = 0; i < (sizeof(command)/4); i++) {
            printf("%s\n", command[i]);
        }
        printf("done.\n");
        execvp(command[0], command);
        // execvp only returns if an error has occurred
        printf("\nExec Failed\n");
        exit(-1);
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
            if (WEXITSTATUS(status) == 0) {
                printf("\nChild exited successfully.\n");
            } else {
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
    if (newsockfd < 0)
        error("ERROR on accept");

    while (true) {
        bzero(buffer, 256); // clear buffer
        n = read(newsockfd, buffer, 255); // read bytes sent from client
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the message: %s\n", buffer);

        //this gets the command
        char *firstWord = strtok(buffer, " ");
        int numSpaces = 0;
        //math to calculate the total number of words in the input
        for (int i = 0; i < sizeof(buffer); i++) {
            if (buffer[i] == ' ') {
                numSpaces++;
            }
        }
        int totalWords = numSpaces + 2;
        printf("totalWords: %u\n", totalWords);
        char **args = (char **) malloc(totalWords * sizeof(char *));
        //args[0] is the command
        args[0] = firstWord;
        //everything after args[0] is arguments to the command
        for (int i = 1; i < totalWords; i++) {
            args[i] = strtok(NULL, " ");
        }
        // let's try executing the code
         int retval = execute(args);

        if (retval < 0) {
            error("ERROR executing command");
        }
            //bzero(buffer, 256);
            // THIS IS WHERE WE RUN THE INPUT AND INSERT THE OUTPUT OF THAT
            // INTO OUR BUFFER!!
            /*n = write(newsockfd, buffer, strlen(buffer)); // write to the socket
            if (n < 0)
                error("ERROR writing to socket");*/
            n = write(newsockfd, "I got your message", 18);
            if (n < 0)
                error("ERROR writing to socket");
            free(args);
        }
        return 0;
    }





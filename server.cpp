#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    // check for correct number of args
    if (argc < 2) {
        fprintf(stderr, "ERROR, please provide port");
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

    bool running = true;
    while (running) {
        bzero(buffer,256); // clear buffer
        n = read(newsockfd,buffer,255); // read bytes sent from client
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);
        bzero(buffer, 256);
        // THIS IS WHERE WE RUN THE INPUT AND INSERT THE OUTPUT OF THAT
        // INTO OUR BUFFER!!
        /*n = write(newsockfd, buffer, strlen(buffer)); // write to the socket
        if (n < 0)
            error("ERROR writing to socket");*/
        n = write(newsockfd, "I got your message", 18);
        if (n < 0)
            error("ERROR writing to socket");
    }
    return 0;
}
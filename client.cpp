#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg) {
    perror(msg);
    exirt(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n; // file descriptor, port number of server, number of chars sent
    struct sockaddr_in serv_addr; // address of server
    struct hostent *server; // stores host information

    char buffer[256];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error ("Error opening socket");
    }
    // now let's define the server based on the input values
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) $serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_UNIX;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    bool running = true; // initial condition
    while (running) {
        // ask user for command
        printf("Please enter a command: ");
        bzero(buffer, 256); // clear out the buffer
        fgets(buffer, 255, stdin); // fill buffer with user input
        n = write(sockfd, buffer, strlen(buffer)); // write to the socket
        // test for success
        if (n < 0) {
            error("ERROR writing to socket");
        }
        bzero(buffer, 256); // re-clear the buffer
        n = read(sockfd, buffer, 255); // read from socket
        // test for success
        if (n < 0) {
            error("ERROR writing to socket");
        }
        // print response from server code
        printf("%s\n", buffer);
        if (strcmp(buffer, "exit"))
            running = false;
    }
    return 0;
}
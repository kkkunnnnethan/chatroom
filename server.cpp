#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/wait.h>

void error(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    if(argc != 2) error("Usage: port");

    //sockfd and newsockfd are file descriptors
    //portno stores the port number on which the server accepts connections
    int sockfd, newsockfd, portno, n;

    //clilen stores the size of the address of the client. This is needed for the accept system call
    socklen_t clilen;

    //buffer to send and receive messages with
    char msg[256];
     
    //setup a socket and connection tools
    sockaddr_in servAddr;
    sockaddr_in cliAddr;

    //open stream oriented socket with internet address also keep track of the socket descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("Error establishing the server socket");

    //The function bzero() sets all values in a buffer to zero.
    //It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer.
    //Thus, this line initializes serv_addr to zeros.
    bzero((char*)&servAddr, sizeof(servAddr));

    //grab the port number
    portno = atoi(argv[1]);

    servAddr.sin_family = AF_INET;
    //htonl-> converts a port number in host byte order to a port number in network byte order.
    servAddr.sin_port = htons(portno);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to its local address
    if (bind(sockfd, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        error("Error binding socket to local address");

    std::cout << "Waiting for a client to connect..." << std::endl;

    //listen for up to 5 requests at a time
    listen(sockfd, 5);

    //receive a request from client using accept
    //we need a new address to connect with the client
    clilen = sizeof(cliAddr);
    //accept, create a new socket descriptor to 
    //handle the new connection with client
    newsockfd = accept(sockfd, (sockaddr *)&cliAddr, &clilen);
    if (newsockfd < 0) error("Error accepting request from client!");

    std::cout << "Connected with client!" << std::endl;

    while(true)
    {
        //receive a message from the client (listen)
        std::cout << "Awaiting client response..." << std::endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer

        //the read() will block until there is something for it to read in the socket,
        // i.e. after the client has executed a write()
        n = recv(newsockfd, (char*)&msg, sizeof(msg), 0);
        if (n < 0) error("Error reading from socket");

        if(!strcmp(msg, "exit"))
        {
            std::cout << "Client has quit the session" << std::endl;
            break;
        }

        std::cout << "Client: " << msg << std::endl;
        std::cout << ">";
        std::string data;
        getline(std::cin, data);
        memset(&msg, 0, sizeof(msg)); //clear the buffer
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            //send to the client that server has closed the connection
            send(newsockfd, (char*)&msg, sizeof(msg), 0);
            break;
        }

        //send the message to client
        n = send(newsockfd, (char*)&msg, sizeof(msg), 0);
        if (n < 0) error("Error writing to socket");
    }

    //we need to close the socket descriptors after we're all done
    close(newsockfd);
    close(sockfd);
    std::cout << "Connection closed..." << std::endl;
    return 0;   
}


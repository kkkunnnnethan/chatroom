#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <pthread.h>
#include <vector>

#include "Semaphore.h"

#define numberOfClient 1
int num;
std::vector<int> sockfdArray{};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void error(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

void broadcastMessage(char* msg, int sockfd) {
    //send the message to client
    pthread_mutex_lock(&lock);
    for (auto socks : sockfdArray) {
        if (socks == sockfd) continue;
        if (send(socks, msg, strlen(msg), 0) < 0)
            error("Error writing to socket");
    }
    pthread_mutex_unlock(&lock);
}

void* socketThread(void *arg) {
    int sockfd = *((int *)arg);
    char msg[256];

    while(true)
    {
        //receive a message from the client (listen)
        std::cout << "Awaiting client response..." << std::endl;
        memset(msg, 0, sizeof(msg));//clear the buffer

        //the read() will block until there is something for it to read in the socket,
        // i.e. after the client has executed a write()
        if (recv(sockfd, (char*)&msg, sizeof(msg), 0) < 0)
            error("Error reading from socket");

        if(!strcmp(msg, "exit"))
        {
            pthread_mutex_lock(&lock);
            auto temp = std::remove(sockfdArray.begin(), sockfdArray.end(), sockfd);
            sockfdArray.erase(temp, sockfdArray.end());
            pthread_mutex_unlock(&lock);
            close(sockfd);
            pthread_exit(nullptr);
        }

        broadcastMessage(msg, sockfd);
    }
}

int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    if(argc != 2) error("Usage: port");

    //sockfd and newsockfd are file descriptors
    //portno stores the port number on which the server accepts connections
    int sockfd, newsockfd, portno;

    //clilen stores the size of the address of the client. This is needed for the accept system call
    socklen_t clilen;
     
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
    pthread_t tid[5];

    //receive a request from client using accept
    //we need a new address to connect with the client
    clilen = sizeof(cliAddr);

    while(true)
    {
        //accept, create a new socket descriptor to handle the new connection with client
        newsockfd = accept(sockfd, (sockaddr *)&cliAddr, &clilen);
        sockfdArray.push_back(newsockfd);
        if (newsockfd < 0) error("Error accepting request from client!");
        std::cout << "Connected with client!" << std::endl;

        if (pthread_create(&tid[(num++) % 5], nullptr, socketThread, &newsockfd) != 0)
            error("Failed to create thread");
    }
    //we need to close the socket descriptors after we're all done
    close(sockfd);
    std::cout << "Connection closed..." << std::endl;
    return 0;
}

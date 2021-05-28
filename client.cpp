#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/wait.h>

void error(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    //we need 2 things: ip address and port number, in that order
    if(argc != 3) error("Usage: ip_address port");

    int sockfd, portno, n;
    sockaddr_in servAddr;
    struct hostent* server;
    char* serverIp;

    //create a message buffer 
    char msg[256];

    //grab port number
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) error("Error establishing the server socket");

    //grab the IP address
    serverIp = argv[1];
    //Takes such a name as an argument and returns a pointer to a hostent containing information about that host
    server = gethostbyname(serverIp);

    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    //int inet_addr(const char* strptr)-> 將十進制點分形式轉換為二進制形式
    //char* inet_ntoa(struct in_addr in)-> 將二進制形式轉換成十進制點分形式字符串
    //這邊不知道為什麼要做兩次轉換，感覺inet_addr(*server->h_addr_list)也可以但嘗試之後發現不行
    servAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*server->h_addr_list));
    servAddr.sin_port = htons(portno);
    //try to connect...
    if (connect(sockfd, (sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        error("Error connecting to socket!");

    std::cout << "Connected to the server!" << std::endl;

    while(true)
    {
        std::cout << ">";
        std::string data;
        getline(std::cin, data);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            send(sockfd, (char*)&msg, strlen(msg), 0);
            break;
        }
        n = send(sockfd, (char*)&msg, strlen(msg), 0);
        if (n < 0) error("Error writing to socket");

        std::cout << "Awaiting server response..." << std::endl;

        memset(&msg, 0, sizeof(msg));//clear the buffer
        n = recv(sockfd, (char*)&msg, sizeof(msg), 0);
        if (n < 0) error("Error reading from socket");

        if(!strcmp(msg, "exit"))
        {
            std::cout << "Server has quit the session" << std::endl;
            break;
        }
        std::cout << "Server: " << msg << std::endl;
    }

    close(sockfd);
    return 0;    
}

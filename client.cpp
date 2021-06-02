#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/uio.h>
#include <sys/wait.h>
#include <pthread.h>
#include <vector>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstdio>
#include <string>

#include "client.h"

std::string name;
bool flag = true;

void error(const std::string& msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

void* sendThread(void* arg) {
    char msg[256];
    int sockfd = *((int *)arg);

    while (true) {
        std::cout << ">";
        std::string data;
        getline(std::cin, data);
        memset(msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, name.c_str());
        strcat(msg, " : ");
        strcat(msg, data.c_str());
        if(data == "exit")
        {
            std::string leaveMsg = name + " has quit";
            if (send(sockfd, leaveMsg.c_str(), leaveMsg.size() + 1, 0) < 0)
                error("Error writing to socket");
            sleep(1);
            send(sockfd, (char*)(data.c_str()), strlen(msg), 0);
            flag = false;
            pthread_exit(nullptr);
        }

        if (send(sockfd, (char*)&msg, strlen(msg), 0) < 0)
            error("Error writing to socket");
    }
}

void* recvThread(void* arg) {
    char msg[256];
    int sockfd = *((int *)arg);

    while (true)
    {
        memset(msg, 0, sizeof(msg));//clear the buffer
        if (recv(sockfd, (char*)&msg, sizeof(msg), 0) < 0)
            error("Error reading from socket");

        std::cout << msg << std::endl;

        if (!flag) break;
    }
    return nullptr;
}

int main(int argc, char *argv[])
{
    //we need 2 things: ip address and port number, in that order
    if(argc != 3) error("Usage: ip_address port");

    int sockfd, portno, n;
    sockaddr_in servAddr;
    struct hostent* server;
    char* serverIp;

    std::cout << "Your name: ";
    getline(std::cin, name);
    std::cout << std::endl;

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
    std::cout << "=== WELCOME TO THE CHATROOM ===" << std::endl;

    std::string welcomeMsg = name + " has joined";
    if (send(sockfd, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0) < 0)
        error("Error writing to socket");

    pthread_t tid1, tid2;
    if (pthread_create(&tid1, nullptr, recvThread, &sockfd) != 0)
        error("Failed to create recvThread");

    if (pthread_create(&tid2, nullptr, sendThread, &sockfd) != 0)
        error("Failed to create sendThread");

    void* ret; // 子執行緒傳回值
    pthread_join(tid2, &ret);

    sleep(1);
    close(sockfd);
    return 0;    
}

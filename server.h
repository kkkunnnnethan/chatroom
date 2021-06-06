//
// Created by Kun on 6/2/21.
//

#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/uio.h>
#include <sys/wait.h>
#include <pthread.h>
#include <algorithm>
#include <vector>

void error(const std::string& msg);
void* sendThread(void* arg);
void broadcastMessage(char* msg, int sockfd);
void* socketThread(void *arg);

#endif //CHATROOM_SERVER_H

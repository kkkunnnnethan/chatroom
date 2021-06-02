//
// Created by Kun on 6/2/21.
//

#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

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
#include <pthread.h>
#include <algorithm>

void error(const std::string& msg);
void* sendThread(void* arg);
void* recvThread(void* arg);

#endif //CHATROOM_CLIENT_H

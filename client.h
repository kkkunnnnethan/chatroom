//
// Created by Kun on 6/2/21.
//

#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

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
#include <string>

void error(const std::string& msg);
void* sendThread(void* arg);
void* recvThread(void* arg);

#endif //CHATROOM_CLIENT_H

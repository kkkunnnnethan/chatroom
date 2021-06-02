//
// Created by Kun on 6/2/21.
//

#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H

void error(const std::string& msg);
void* sendThread(void* arg);
void broadcastMessage(char* msg, int sockfd);
void* socketThread(void *arg);

#endif //CHATROOM_SERVER_H

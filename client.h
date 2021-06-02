//
// Created by Kun on 6/2/21.
//

#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

void error(const std::string& msg);
void* sendThread(void* arg);
void* recvThread(void* arg);

#endif //CHATROOM_CLIENT_H

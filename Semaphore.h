//
// Created by Kun on 5/28/21.
//

#ifndef CHATROOM_SEMAPHORE_H
#define CHATROOM_SEMAPHORE_H

#include <iostream>
#include <mutex>
#include <condition_variable>

class Semaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
public:
    Semaphore() : count(0) {}
    explicit Semaphore(int count_) : count(count_) {}

    void Signal() {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mtx);
        while (count == 0) {
            cv.wait(lock);
        }
        count--;
    }
};

#endif //CHATROOM_SEMAPHORE_H

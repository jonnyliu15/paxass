#ifndef NODE_H
#define NODE_H

#include "globals.h"
#include "messaging/socket_messaging.cpp"
#include <functional>
#include <iostream>
#include <thread>

class PaxosNode {
  public:
    PaxosNode(int id, NodeMessaging *messaging);

    ~PaxosNode();

    int write(const char *data);

    char *read(int index);

    void say_hi(int destination_id);

  private:
    int id;
    int last_vote;
    FILE *my_memory;
    NodeMessaging *messaging;
    std::thread *listener;

    static void handle_message(char *msg);

    void initiate_vote(char *data);

    void vote();
};

#endif
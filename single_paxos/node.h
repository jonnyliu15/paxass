#ifndef NODE_H
#define NODE_H

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <sys/socket.h>
#include <thread>
#include <netinet/in.h>
#include <netdb.h>
#include <functional>
#include <filesystem>
#include "globals.h"

class PaxosNode {
    public:
        PaxosNode(int id);

        ~PaxosNode();

    private:
        int id;
        int receiver_socket_fd;
        int last_vote;
        std::thread * listener;

        void listen();

};

#endif
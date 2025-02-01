#include "messaging.h"
#include <iostream>
#include <netdb.h>
#include <unistd.h>

#define PORT 8080
#ifndef SOCKET_MESSAGING_H
#define SOCKET_MESSAGING_H

static int get_socket(int port_num) {
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM; // UDP more closely adheres to assumptions
                                    // made about a distributed system
    hints.ai_flags = AI_PASSIVE;

    addrinfo *socket_addr;

    int status;
    if ((status = getaddrinfo(NULL, std::to_string(port_num).c_str(), &hints,
                              &socket_addr)) != 0) {
        std::cerr << "Error getting address info" << strerror(errno)
                  << std::endl;
        throw std::exception();
        return -1;
    }

    int yes = 1;
    int socket_fd = 0;
    for (auto p = socket_addr; p != nullptr; p = p->ai_next) {
        if ((socket_fd =
                 socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cerr << "Error socketing" << std::endl;
            continue;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == 1) {
            std::cerr << "Socket opened, but error setting socket options"
                      << strerror(errno) << std::endl;
            exit(1);
        }

        if (::bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            std::cerr << "Error binding" << strerror(errno) << std::endl;
            continue;
        }
    }

    std::cout << "socket bound to port " << port_num << std::endl;
    return socket_fd;
}

class NodeMessagingSocket : public NodeMessaging {
  public:
    NodeMessagingSocket(int node_id) : NodeMessaging() {
        receiver_socket_fd = get_socket(PORT + node_id * 2 - 1);
        send_socket_fd = get_socket(PORT + node_id * 2);
    }

    ~NodeMessagingSocket() {
        close(receiver_socket_fd);
        close(send_socket_fd);
    }

    void listen(void (*msg_handler)(char *)) override {
        char buffer[1000];
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        while (true) {
            int bytes_received =
                recvfrom(this->receiver_socket_fd, buffer, sizeof(buffer), 0,
                         (sockaddr *)&client_addr, &addr_len);
            if (bytes_received < 0) {
                std::cerr << "Error receiving data " << strerror(errno)
                          << std::endl;
                break;
            }
            buffer[bytes_received] = '\0';
            std::cout << "hello" << std::endl;
            msg_handler(buffer);

            const char *ack = "ack";
            sendto(this->receiver_socket_fd, (const char *)ack, strlen(ack), 0,
                   (sockaddr *)&client_addr, addr_len);
            memset(buffer, 0, 1000);
        }
    }

    void send(char *msg, int size, int destination_id) override {
        sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT + destination_id * 2);
        dest_addr.sin_addr.s_addr = INADDR_ANY;
        socklen_t addr_len = sizeof(dest_addr);
        if (sendto(this->send_socket_fd, msg, size, 0, (sockaddr *)&dest_addr,
                   addr_len) != size) {
            std::cerr << "Error sending all the bytes" << strerror(errno)
                      << std::endl;
        }

        char buffer[1000];
        int bytes_received =
            recvfrom(this->send_socket_fd, buffer, sizeof(buffer), 0,
                     (sockaddr *)&dest_addr, &addr_len);
        buffer[bytes_received] = '\0';
    }

  private:
    int receiver_socket_fd;
    int send_socket_fd;
};

#endif
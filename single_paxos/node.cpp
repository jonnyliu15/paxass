
#include "node.h"

#define PORT 8080

int get_socket(int port_num) {
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM; // UDP more closely adheres to assumptions made about a distributed system
    hints.ai_flags = AI_PASSIVE;

    addrinfo * socket_addr;

    int status;
    if ((status = getaddrinfo(NULL, std::to_string(port_num).c_str(), &hints, &socket_addr)) != 0 ) {
        std::cerr << "Error getting address info" << strerror(errno) << std::endl;
        throw std::exception();
        return -1;
    }

    int yes = 1;
    int socket_fd = 0;
    for (auto p = socket_addr; p != nullptr; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cerr << "Error socketing" << std::endl;
            continue;
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == 1) {
            std::cerr << "Socket opened, but error setting socket options" << strerror(errno) << std::endl;
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

PaxosNode::PaxosNode(int id) {
    std::cout << "Starting PaxosNode: " << id << " in process " << getpid() << std::endl;
    this->id = id;
    this->send_socket_fd = get_socket(PORT + id * 2 - 1); 
    this->receiver_socket_fd = get_socket(PORT + id * 2);
    std::thread * listener = new std::thread(std::bind(&PaxosNode::listen, this));
    listener->detach();
}

PaxosNode::~PaxosNode() {
    listener->~thread();
    close(this->receiver_socket_fd);
    close(this->send_socket_fd);
}

void PaxosNode::sayHi(PaxosNode * destination) {
    std::cout << "hello" << std::endl;
    this->send("hi", 2, destination);
}

void PaxosNode::listen() {
    char buffer[1000];
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    while (!terminate_flag) {
        int bytes_received = recvfrom(this->receiver_socket_fd, buffer, sizeof(buffer), 0, (sockaddr *) &client_addr, &addr_len);
        if (bytes_received < 0) {
            std::cerr << "Error receiving data " << strerror(errno) << std::endl;
            break;
        }
        buffer[bytes_received] = '\0';

        std::cout << "received " << buffer << std::endl;

        const char * ack = "ack";
        sendto(this->receiver_socket_fd, (const char *) ack, strlen(ack), 0, (sockaddr *) &client_addr, addr_len);
        memset(buffer, 0, 1000);
    }
}

void PaxosNode::send(char * msg, int size, PaxosNode * destination) {
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT + destination->id * 2);
    dest_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t addr_len = sizeof(dest_addr);
     std::cout << "hello" << std::endl;
    if (sendto(this->send_socket_fd, msg, size, 0, (sockaddr *) & dest_addr, addr_len) != size) {
        std::cerr << "Error sending all the bytes" << strerror(errno) << std::endl;
    }

    char buffer[1000];
    int bytes_received = recvfrom(this->send_socket_fd, buffer, sizeof(buffer), 0, (sockaddr * ) &dest_addr, &addr_len);
    buffer[bytes_received] = '\0';
    std::cout << buffer << std::endl;
}

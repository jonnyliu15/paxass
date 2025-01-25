
#include "node.h"

#define PORT 8080


PaxosNode::PaxosNode(int id) {
    std::cout << "Starting PaxosNode: " << id << " in process " << getpid() << std::endl;
    this->id = id;
    // this->send_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0); // UDP more closely adheres to assumptions made about a distributed system
    std::thread * listener = new std::thread(std::bind(&PaxosNode::listen, this));
    listener->join();
}

PaxosNode::~PaxosNode() {
    listener->~thread();
    close(this->receiver_socket_fd);
    std::string socket_path = "/tmp/socket-" + std::to_string(this->id) + ".sock";
    std::__fs::filesystem::remove(socket_path);

}

void PaxosNode::listen() {
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo * receive_socket_addr;

    int port_num = PORT + this->id;

    int status;
    if ((status = getaddrinfo(NULL, std::to_string(port_num).c_str(), &hints, &receive_socket_addr)) != 0 ) {
        std::cerr << "Error getting address info" << strerror(errno) << std::endl;
        throw std::exception();
        return;
    }

    int yes = 1;
    for (auto p = receive_socket_addr; p!= nullptr; p = p->ai_next) {
        if ((this->receiver_socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cerr << "Error socketing" << std::endl;
            continue;
        }

        if (setsockopt(this->receiver_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            std::cerr << "Socket opened, but error setting socket options" << strerror(errno) << std::endl;
            exit(1);
        }

        if (::bind(this->receiver_socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(this->receiver_socket_fd);
            std::cerr << "Error binding" << strerror(errno) << std::endl;
            continue;
        }
    }
    freeaddrinfo(receive_socket_addr);



    std::cout << "Socket is bound to port " << port_num << std::endl;
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

        std::cout << "received " << bytes_received << std::endl;
    }
}

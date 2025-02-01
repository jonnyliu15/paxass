
#include "node.h"

PaxosNode::PaxosNode(int id, NodeMessaging *msg) {
    this->id = id;
    this->messaging = msg;

    this->listener = new std::thread(std::bind(
        &NodeMessaging::listen, this->messaging, PaxosNode::handle_message));
    this->listener->detach();
};

PaxosNode::~PaxosNode() { delete this->messaging; }

void PaxosNode::say_hi(int destination_id) {
    this->messaging->send("hi", 2, destination_id);
}

void PaxosNode::handle_message(char *msg) { std::cout << msg << std::endl; }

void PaxosNode::initiate_vote(char *data) {
    std::cout << "not implemented" << std::endl;
}

void PaxosNode::vote() { std::cout << "not implemented" << std::endl; }

int PaxosNode::write(const char *data) {
    std::cout << "not implemented" << std::endl;
}

char *read(int index) { std::cout << "not implemented" << std::endl; }
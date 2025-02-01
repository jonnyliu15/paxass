#ifndef NODEMESSAGING_H
#define NODEMESSAGING_H

class NodeMessaging {
  public:
    NodeMessaging() {}
    virtual void listen(void (*msg_handler)(char *)) = 0;

    virtual void send(char *msg, int size, int destination_id) = 0;
};

#endif
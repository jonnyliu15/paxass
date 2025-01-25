
#include "globals.h"

int sigterm_received = 0;

volatile int terminate_flag = 0;

void sigterm_handler(int signum) {
    sigterm_received = 1;
}
void node_sigterm_handler(int signum) {
    std::cout << "exiting" << std::endl;
    terminate_flag = 1;
}
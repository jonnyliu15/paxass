
#ifndef GLOBALS_H
#define GLOBALS_H

#include <iostream>
#include <unistd.h>

extern int sigterm_received;

extern volatile int terminate_flag;

void sigterm_handler(int signum);

void node_sigterm_handler(int signum);

#endif
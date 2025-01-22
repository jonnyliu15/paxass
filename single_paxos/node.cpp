
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>

class PaxosNode {
    public:
        PaxosNode() {
            std::cout << "hey" << std::endl;
            pid_t pid = fork();
            if (pid == 0) {
                this->pid = getpid();
                run();
            }
        }

        ~PaxosNode() {
            std::cout << "bye" << std::endl;
            kill(this->pid, SIGTERM);
        }
    private:
        pid_t pid;

        void run() {
            std::cout << "Starting PaxosNode: " << this->pid << std::endl;
            while (true) {
                std::cout << "beep" << std::endl;
                sleep(random() % 3);
            }
        }
};
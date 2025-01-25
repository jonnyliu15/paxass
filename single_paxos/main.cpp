using namespace std;

#include <iostream>
#include <vector>

#include "node.h";
#include "globals.h";


class RunPaxos {
    public:
        RunPaxos(int n_nodes) {
            cout << "Initializing Paxos nodes" << endl;
            for (int i = 0; i < n_nodes; i++) {
                pid_t pid = fork();
                if (pid == 0) {
                    struct sigaction sa;
                    sa.sa_handler = node_sigterm_handler;
                    sa.sa_flags = 0;
                    sigemptyset(&sa.sa_mask);
                    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
                        perror("sigaction");
                        exit(1);
                    }
                    PaxosNode node(i); 
                    exit(0);
                } else if (pid > 0) {
                    processes.push_back(pid);
                } else {
                    cerr << "Fork failed for node " << i << endl;
                    exit(1);
                }
            }
        }

        ~RunPaxos() {
            for(int i = 0; i < processes.size(); i++) {
                cout << "Sending SIGTERM to process " << processes[i] << endl;
                kill(processes[i], SIGTERM);
            }
            for (pid_t pid : processes) {
                waitpid(pid, nullptr, 0);
            }
        }

    vector<pid_t> processes;
};

int main() {
    int n_nodes = 0;
    cout << "Enter the number of nodes: ";
    cin >> n_nodes;
    cout << "Number of nodes: " << n_nodes << endl;
    RunPaxos paxos(n_nodes);
    signal(SIGINT, sigterm_handler);
    while (!sigterm_received) {
        sleep(1);
    }
    return 0;
}
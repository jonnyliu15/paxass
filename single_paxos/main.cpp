using namespace std;

#include <iostream>
#include <vector>
#include <unordered_set>
#include <sstream>

#include "utils.cpp";
#include "node.h";
#include "globals.h";


class RunPaxos {
    public:
        RunPaxos(int n_nodes) {
            cout << "Initializing Paxos nodes" << endl;
            for (int i = 0; i < n_nodes; i++) {
               PaxosNode * node = new PaxosNode(i + 1); 
               nodes.push_back(node);
            }
        }

        ~RunPaxos() {
            for (int i = 0; i < nodes.size(); i++) {
                delete nodes[i];
            }
        }

        void runCommand(vector<string> & parameters) {
            try {
                if (parameters[0] == "sayHi") {
                    if (parameters.size() > 3) {
                        throw runtime_error("Too many arguments for command: " + parameters[0]);
                    }
                    sayHi(stoi(parameters[1]), stoi(parameters[2]));
                    return;
                }
                throw runtime_error("Invalid command: " + parameters[0]);
            }
            catch (exception &e) {
                cout << e.what() << endl;
            }
        }
    private:
        void sayHi(int source, int destination) {
                nodes[source]->sayHi(nodes[destination]);
            }
        vector<PaxosNode *> nodes;
};

int main() {
    int n_nodes = 0;
    cout << "Enter the number of nodes: ";
    cin >> n_nodes;
    cout << "Number of nodes: " << n_nodes << endl;
    cin.ignore();
    RunPaxos paxos(n_nodes);
    signal(SIGINT, sigterm_handler);
    while (!sigterm_received) {
        cout << "Enter a command: ";
        string commandString;
        getline(cin, commandString);
        stringstream stream (commandString);
        string token;
        vector<string> tokens;
        while (getline(stream, token, ' ')) {
            tokens.push_back(token);
        }
        paxos.runCommand(tokens);

    }
    return 0;
}
using namespace std;

#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "globals.h";
#include "messaging/socket_messaging.cpp";
#include "node.h";
#include "utils.cpp";

class Paxos {
  public:
    Paxos(int n_nodes) {
        cout << "Initializing Paxos nodes" << endl;
        for (int i = 0; i < n_nodes; i++) {
            NodeMessagingSocket *msg = new NodeMessagingSocket(i + 1);
            PaxosNode *node = new PaxosNode(i + 1, msg);
            nodes.push_back(node);
        }
    }

    ~Paxos() {
        for (int i = 0; i < nodes.size(); i++) {
            delete nodes[i];
        }
    }

    void say_hi(int source, int destination) {
        nodes[source]->say_hi(destination);
    }

    int write(const char *data) { nodes[leader]->write(data); }

    char *read(int index) {
        int nodeIndex = rand() % nodes.size();
        return nodes[nodeIndex]->read(index);
    }

  private:
    void elect_leader() { leader = nodes.size() - 1; }

    int leader;
    vector<PaxosNode *> nodes;
};

class RunPaxos {
  public:
    RunPaxos(int n_nodes) { paxos = new Paxos(n_nodes); }

    ~RunPaxos() { delete paxos; }

    void run_command(vector<string> &parameters) {
        try {
            if (parameters[0] == "sayHi") {
                if (parameters.size() > 3) {
                    throw runtime_error("Too many arguments for command: " +
                                        parameters[0]);
                }
                paxos->say_hi(stoi(parameters[1]), stoi(parameters[2]));
                return;
            } else if (parameters[0] == "write") {
                paxos->write(parameters[1].c_str());
                return;
            }
            throw runtime_error("Invalid command: " + parameters[0]);
        } catch (exception &e) {
            cout << e.what() << endl;
        }
    }

  private:
    Paxos *paxos;
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
        stringstream stream(commandString);
        string token;
        vector<string> tokens;
        while (getline(stream, token, ' ')) {
            tokens.push_back(token);
        }
        paxos.run_command(tokens);
    }
    return 0;
}
using namespace std;

#include <iostream>
#include "node.cpp";

void paxos_init(int n_nodes) {
    cout << "Initializing Paxos nodes" << endl;
    for (int i = 0; i < n_nodes; i++) {
        PaxosNode * node_ptr = new PaxosNode();
    }
}

int main() {
    int n_nodes = 0;
    cout << "Enter the number of nodes: ";
    cin >> n_nodes;
    cout << "Number of nodes: " << n_nodes << endl;
    paxos_init(n_nodes);
    while (true) {
        cout << "stuff" << endl;
        sleep(1);
    }
    return 0;
}
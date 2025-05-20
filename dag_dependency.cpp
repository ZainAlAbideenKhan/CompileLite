#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

using namespace std;

// Node structure for linked list
struct AdjNode {
    string funcName;
    AdjNode* next;
    AdjNode(string name) : funcName(name), next(nullptr) {}
};

// DAG using adjacency list representation
class DependencyDAG {
private:
    unordered_map<string, AdjNode*> adjList; // function -> list of dependent functions

public:
    void addEdge(const string& from, const string& to) {
        AdjNode* node = new AdjNode(to);
        node->next = adjList[from];
        adjList[from] = node;
    }

    void addFunction(const string& func) {
        if (adjList.find(func) == adjList.end()) {
            adjList[func] = nullptr;
        }
    }

    void printDAG() const {
        cout << "\n--- Function Dependency DAG ---\n";
        for (const auto& entry : adjList) {
            cout << entry.first << " -> ";
            AdjNode* temp = entry.second;
            while (temp) {
                cout << temp->funcName << " ";
                temp = temp->next;
            }
            cout << endl;
        }
    }

    ~DependencyDAG() {
        for (auto& entry : adjList) {
            AdjNode* current = entry.second;
            while (current) {
                AdjNode* next = current->next;
                delete current;
                current = next;
            }
        }
    }
};

// Utility: parse function body and extract used functions/variables
unordered_set<string> extractDependencies(const string& body, const vector<string>& allFuncs) {
    unordered_set<string> deps;
    for (const string& func : allFuncs) {
        regex r("\\b" + func + "\\s*\\(");
        if (regex_search(body, r)) {
            deps.insert(func);
        }
    }
    return deps;
}

// Sample function definitions
struct Function {
    string name;
    string body;
};

// Main process to build dependency DAG
void buildDependencyGraph(const vector<Function>& functions) {
    DependencyDAG dag;

    vector<string> functionNames;
    for (const auto& fn : functions) {
        dag.addFunction(fn.name);
        functionNames.push_back(fn.name);
    }

    for (const auto& fn : functions) {
        unordered_set<string> deps = extractDependencies(fn.body, functionNames);
        for (const string& dep : deps) {
            if (dep != fn.name) // avoid self-loop
                dag.addEdge(fn.name, dep);
        }
    }

    dag.printDAG();
}

// Example usage
int main() {
    vector<Function> code = {
        {"A", "x = 30;"},
        {"B", "printf(\"%d\", x);"},
        {"C", "A(); B();"},
        {"D", "int y = 10;"}
    };

    buildDependencyGraph(code);
    return 0;
}

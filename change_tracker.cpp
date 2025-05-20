#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Struct to hold change information
struct Change {
    enum Type { ADDED, MODIFIED, DELETED } type;
    int lineNumber;
    string content;

    string typeToStr() const {
        switch (type) {
            case ADDED: return "ADDED";
            case MODIFIED: return "MODIFIED";
            case DELETED: return "DELETED";
        }
        return "";
    }
};

// Function to read a file line by line
vector<string> readFileLines(const string& filename) {
    vector<string> lines;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return lines;
    }
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

// Compare files line by line and detect changes
vector<Change> compareFiles(const string& oldFile, const string& newFile) {
    vector<string> oldLines = readFileLines(oldFile);
    vector<string> newLines = readFileLines(newFile);
    vector<Change> changes;

    size_t maxLines = max(oldLines.size(), newLines.size());

    for (size_t i = 0; i < maxLines; ++i) {
        if (i >= oldLines.size()) {
            changes.push_back({Change::ADDED, static_cast<int>(i + 1), newLines[i]});
        } else if (i >= newLines.size()) {
            changes.push_back({Change::DELETED, static_cast<int>(i + 1), oldLines[i]});
        } else if (oldLines[i] != newLines[i]) {
            changes.push_back({Change::MODIFIED, static_cast<int>(i + 1), newLines[i]});
        }
    }

    return changes;
}

// Print the changes for debugging
void printChanges(const vector<Change>& changes) {
    cout << "\n--- Tracked Changes ---\n";
    for (const auto& change : changes) {
        cout << "[" << change.typeToStr() << "] Line " << change.lineNumber << ": " << change.content << endl;
    }
}

// Save changes to a log file
void saveChangesToFile(const vector<Change>& changes, const string& outputFile) {
    ofstream out(outputFile);
    if (!out.is_open()) {
        cerr << "Error: Could not write to file " << outputFile << endl;
        return;
    }
    for (const auto& change : changes) {
        out << change.typeToStr() << " " << change.lineNumber << ": " << change.content << "\n";
    }
    out.close();
}

// Simple test in main function
int main() {
    // Define file names
    string oldFile = "old.txt";
    string newFile = "new.txt";

    // Create dummy old.txt
    ofstream outOld(oldFile);
    outOld << "int x = 10;\n";
    outOld << "void A() {\n";
    outOld << "  printf(\"lol nothing\");\n";
    outOld << "}\n";
    outOld.close();

    // Create dummy new.txt
    ofstream outNew(newFile);
    outNew << "int x = 10;\n";
    outNew << "void A() {\n";
    outNew << "  x = 30;\n";  // modified
    outNew << "}\n";
    outNew << "void B() {\n"; // added
    outNew << "  printf(\"%d\", x);\n";
    outNew << "}\n";
    outNew.close();

    // Run comparison
    vector<Change> changes = compareFiles(oldFile, newFile);
    printChanges(changes);
    saveChangesToFile(changes, "change_log.txt");

    cout << "\nChanges saved to change_log.txt\n";
    return 0;
}

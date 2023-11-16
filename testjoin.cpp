#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

vector<vector<string>> database; // Global variable to store the database

void readDatabaseFromFile(const string& filename) {
    ifstream file(filename); // Open the file

    if (!file.is_open()) {
        cout << "Error opening the file." << endl;
        return;
    }

    string line;
    while (getline(file, line)) { // Read each line
        vector<string> row;
        size_t pos = 0;
        string delimiter = ","; // Assuming comma-separated values

        while ((pos = line.find(delimiter)) != string::npos) {
            string token = line.substr(0, pos); // Get value up to delimiter
            row.push_back(token); // Add value to the row vector
            line.erase(0, pos + delimiter.length()); // Erase processed value
        }
        
        row.push_back(line); // Add the last value in the row
        database.push_back(row); // Add row to the database
    }

    file.close(); // Close the file
}

int countUniqueValues() {
    unordered_set<string> uniqueValues;

    for (const auto& row : database) {
        unordered_set<string> uniqueInRow;
        for (const auto& value : row) {
            if (value != "-1" && value != "-2" && uniqueInRow.find(value) == uniqueInRow.end()) {
                uniqueValues.insert(value);
                uniqueInRow.insert(value);
            }
        }
    }

    return uniqueValues.size();
}

int main() {
    readDatabaseFromFile("dataset.txt"); // Read database from file

    int uniqueCount = countUniqueValues();

    cout << "Number of unique values (excluding -1 and -2): " << uniqueCount << endl;

    return 0;
}

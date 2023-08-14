#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;
struct patternlist{
    string sequence;

    int finalT = 0;
    int frequency = 0;
    vector<int> T_P[64000] = NULL;

    void position(int p, int t)
    {
        T_P[t].push_back(p);
        if(t > finalT) {
            frequency++;
            finalT = t;
        }
    }


};

int main() {
    std::ifstream inputFile("dataset.txt");
    if (!inputFile.is_open()) {
        cerr << "Failed to open the input file." << endl;
        return 1;
    }

    unordered_map<string, int> itemFrequency;
    string line;

    int row = 1;
    int column = 1;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string item;

        while (iss >> item) {
            if (item == "-1") {
                column++;
            } else if(item == "-2"){
                row++;
                column = 1;
            } else {
                itemFrequency[item]++;
            }
        }
    }

    inputFile.close();

    // Reopen the file to print the results alongside the dataset
    inputFile.open("dataset.txt");
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string item;

        while (iss >> item) {
            if (item == "-1") {
                cout << ", ";
            } else if (item == "-2") {
                cout << "-2 ";
                cout <<endl;
            } else {
                cout << item << " ";
            }
        }

        cout << "(Freq: ";
        for (const auto& entry : itemFrequency) {
            const string& item = entry.first;
            const int frequency = entry.second;
            cout << item << ": " << frequency << ", ";
        }
        cout << ")";
        cout << endl;
    }

    return 0;
}

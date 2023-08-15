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
    int bit_vector = 0;
    vector<int> T_P[64000];

    patternlist(string _sequence)
    {
        sequence = _sequence;
        
        // Initialize vectors within the array
        for (int i = 0; i < 64000; i++) {
            T_P[i] = vector<int>();
        }
    }

    void position(int t, int p, string _sequence) 
    {
        if(_sequence != sequence) return;
        T_P[t].push_back(p);
        if(t > finalT) {
            frequency++;
            finalT = t;
            bit_vector+= pow(2, finalT-1);
        }
    }

    vector<int> bitPositions() 
    {
        int number = bit_vector;
        vector<int> positions;
        int position = 1;
        while (number != 0) {
            if ((number & 1) != 0) {
                positions.push_back(position);
            }
            position++;
            number = number >> 1;
        }
        return positions;
    }

    void printinfo() 
    {
        cout << "sequence: " << sequence << endl;
        cout << "T and P values: " << endl;
        for(int i = 1; i<=finalT; i++)
        {
            if(!T_P[i].empty())
            {
                cout << "T: " << i << " - P: ";
                for(int T: T_P[i]) 
                {
                    cout << T << " ";
                }
                cout << endl << endl;
            }
        }

        cout << "Bit vector of the sequence: " << bit_vector <<  endl;

        vector<int> positions = bitPositions();

        cout << "Bit position of 1's in the bit vector: ";
        int bit = 0;
        for(int position: positions)
        {
            cout << position << " ";

            bit += pow(10, position-1);
        }
        cout << endl;
        cout << "Binary bit: " << bit << endl;
    }

};

int main() {

    patternlist A("A");         //testing patternList with random sequence

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
                    A.position(row, column, item);
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

    A.printinfo();

    return 0;
}

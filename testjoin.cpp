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

    patternlist()
    {
        //default constructor
    }

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
            bit_vector |= (1 << (sizeof(int)*3 - finalT));
        }
    }

    vector<int> bitPositions() const
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

    int getFrequency() const
    {
        return frequency;
    }

    void printinfo() const 
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
        cout << "Binary bit: " << bitset<sizeof(int)*3>(bit_vector) << endl;
    }

};

int main() {

    ///patternlist A("A");         //testing patternList with random sequence

    std::ifstream inputFile("dataset.txt");
    if (!inputFile.is_open()) {
        cerr << "Failed to open the input file." << endl;
        return 1;
    }

    unordered_map<string, patternlist> items;
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
                    items[item].sequence = item;
                    items[item].position(row, column, item);
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
        for (const auto& entry : items) {
            const string& item = entry.first;
            const patternlist frequency = entry.second;
            cout << item << ": " << frequency.getFrequency() << ", ";
        }
        cout << ")";
        cout << endl;
    }

    cout << "sequences info: " << endl;

    for (const auto& entry : items) {
        const string& item = entry.first;
        const patternlist inteminfo = entry.second;

        cout << "Sequence: " <<  item << endl;
        inteminfo.printinfo();
    }

    //A.printinfo();
    //cout << "testing popwer: " << pow(10, 9) + pow(10, 7) + pow(10, 5) + pow(10, 2) + 1 << endl;
    return 0;
}

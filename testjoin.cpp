#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

int minsup = 4;

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

    patternlist(const patternlist& other) {
        sequence = other.sequence;
        finalT = other.finalT;
        frequency = other.frequency;
        bit_vector = other.bit_vector;
        for (int i = 0; i <= finalT; i++) {
            T_P[i] = other.T_P[i];
        }
    }

    void position(int t, int p)
    {
        T_P[t].push_back(p);
        if(t > finalT) {
            frequency++;
            finalT = t;
            bit_vector |= (1 << (sizeof(int)*3 - finalT));
        }
    }

        vector<int> bitPositions() const {
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

    int getFrequency() const //returns frequency of the sequence
    {
        return frequency;
    }

    void printinfo() const // printing all sorts of information with all kinds of format related to this patternlist
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

       // vector<int> positions = bitPositions();

        cout << "Bit position of 1's in the bit vector: ";
        cout << "Binary bit: " << bitset<sizeof(int)*3>(bit_vector) << endl;
    }

};

map<string, patternlist> items;

patternlist single_itemset_join(const patternlist& seq1, const patternlist& seq2) {
    cout << "testing" << endl;
    patternlist answer(seq1.sequence + " " + seq2.sequence + " -1 -2");
    answer.bit_vector = seq1.bit_vector & seq2.bit_vector;
    vector<int> t_value = answer.bitPositions();

    for (int i : t_value) {
        int match = 0;
        for (int value1 : seq1.T_P[i]) {
            if (std::find(seq2.T_P[i].begin(), seq2.T_P[i].end(), value1) != seq2.T_P[i].end()) {
                answer.T_P[i].push_back(value1);
                match = 1;
            }
        }
        answer.frequency += match;
    }

    //patternlist answer("A B");

    return seq1;
}

int main() {

    ///patternlist A("A");         //testing patternList with random sequence

    std::ifstream inputFile("dataset.txt");
    if (!inputFile.is_open()) {
        cerr << "Failed to open the input file." << endl;
        return 1;
    }

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
                    items[item].position(row, column);
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
    }

        // Create a list of keys to be removed
        vector<string> keysToRemove;

        // Iterate over the map to identify keys to remove
        for (const auto& entry : items) {
            const string& item = entry.first;
            const patternlist& frequency = entry.second;
            int support = frequency.getFrequency();
            
            if (support < minsup) {
                keysToRemove.push_back(item);
            }
        }

        // Remove the identified keys
        for (const string& key : keysToRemove) {
            items.erase(key);
        }

        cout << "(Freq: ";
        for (const auto& entry : items) {
            const string& item = entry.first;
            const patternlist frequency = entry.second;
            int support = frequency.getFrequency();
            
            cout << item << ": " << support << ", ";
        }
        cout << ")";
        cout << endl;

    cout << "sequences info: " << endl;

    for (const auto& entry : items) {
        const string& item = entry.first;
        const patternlist inteminfo = entry.second;

        cout << "Sequence: " <<  item << endl;
        inteminfo.printinfo();
    }

    cout << "checking join operation" << endl;
    // patternlist a(items["A"]);
    // patternlist b(items["B"]);
    patternlist join = single_itemset_join(items["A"], items["B"]);
    join.printinfo();

    // patternlist a = items["A"];
    // a.printinfo();
    return 0;
}

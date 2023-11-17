#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include<bits/stdc++.h>

using namespace std;
vector<vector<string>> database; // Global variable to store the database

struct patternList
{
    vector<string> pattern;
    int support = 0;
    int start;
    map<int, vector<int>> T_P;

    void add_num(int num, vector<string> values) {
        support += num;
        for(const auto& value : values) {
            pattern.push_back(value);
        }
    }

    int print_count() {
        return support;
    }

    void add_T_P(int t, int p) {
        T_P[t].push_back(p);
    }

    void print_pattern() {
        for(const auto& t_values : T_P) {
            cout << "t : " << t_values.first << " p : ";
            for(const auto& p_values : T_P[t_values.first]) {
                cout << p_values << ", ";
            } 
            cout << endl;
        }
    }
};

map<string, patternList> uniqueValues; // items with support count
map<vector<string>, patternList> frequent_patterns; //frequent sequences after join

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
        string delimiter = " "; //  space-separated values

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
void countUniqueValues() {
    int row_num = 0;
    for (const auto& row : database) {
        row_num++;
        set<string> uniqueInRow;
        int itemset_num = 1;
        for (const auto& value : row) {
            if(value == "-1") {
                itemset_num++;
            }
            if (value != "-1" && value != "-2") {
                if(uniqueInRow.find(value) == uniqueInRow.end()) {
                    vector<string> temp_vect{value};
                    uniqueValues[value].add_num(1, temp_vect);
                    uniqueInRow.insert(value);
                }
                uniqueValues[value].add_T_P(row_num, itemset_num);
            }
        }
    }
    // return uniqueValues.size();
}
// int countUniqueValues() {
//     std::set<char> uniqueValues;

//     for (auto& row : database) {
//         //cout << "row   " << row << endl ;
//         //unordered_set<string> uniqueInRow;
//         //cout << row.size() << endl ;
//         //cout << _row << endl ;
//         for (auto& value : row) {
//             string _row = value;

//             //cout << "Value   " << _row << endl ;
//             for(int i=0; _row[i]; i++)
//             {
//                 if (_row[i] != '1' &&_row[i] != '2' && _row[i] !='-' && _row[i]!=' ' && uniqueValues.find(_row[i]) == uniqueValues.end()) 
//                 {
//                 uniqueValues.insert(_row[i]);
//                 cout << _row[i] << endl;
//                 //uniqueInRow.insert(_row[i]);
//                 //cout << value <<  endl ;
//             }

//             }
//         }
//     }

//     return uniqueValues.size();
// }

int main() {
    readDatabaseFromFile("dataset.txt"); // Read database from file

    countUniqueValues();

    cout << "Number of unique values (excluding -1 and -2): " << endl;
    for(const auto& items : uniqueValues) {
        // uniqueValues[items.first].add_num(0);
        cout << items.first << " : " << uniqueValues[items.first].print_count() << endl;
        cout << "PatternLists : " << endl;
        uniqueValues[items.first].print_pattern();
        cout << endl;
    }

    return 0;
}

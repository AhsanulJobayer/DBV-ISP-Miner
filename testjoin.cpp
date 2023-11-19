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
            if(!pattern.size()) {
                        for(const auto& value : values) {
                pattern.push_back(value);
            }
        }
    }

    int print_count() {
        return support;
    }

    void add_T_P(int t, int p) {
        T_P[t].push_back(p);
    }

    void print_pattern() {
        for(const auto& p : pattern) {
            cout << p << " ";
        }
        cout << endl;
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
map<int, map<vector<string>, patternList>> frequent_patterns; //frequent sequences after join

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
                    vector<string> temp_vect{value, "-1", "-2"};
                    uniqueValues[value].add_num(1, temp_vect);
                    uniqueInRow.insert(value);
                }
                uniqueValues[value].add_T_P(row_num, itemset_num);
            }
        }
    }
}

vector<int> intersect(const vector<int>& v1, const vector<int>& v2) {
    vector<int> result;

    vector<int> sorted_v1 = v1;
    vector<int> sorted_v2 = v2;
    sort(sorted_v1.begin(), sorted_v1.end());
    sort(sorted_v2.begin(), sorted_v2.end());

    set_intersection(sorted_v1.begin(), sorted_v1.end(),
                     sorted_v2.begin(), sorted_v2.end(),
                     back_inserter(result));

    return result;
}

// ------------------------------------Single join operations---------------------------------------
patternList single_itemset_join(patternList A, patternList B) {
    patternList C;
    if(A.pattern[0] == B.pattern[0]) {return C;}
    int support = 0;
    for(const auto& tp : A.T_P) {
        vector<int> joined_p = intersect(A.T_P[tp.first], B.T_P[tp.first]);
        if(joined_p.size()) {
            C.T_P[tp.first] = joined_p;
            support++;
        }
    }

    C.support = support;
    C.pattern.push_back(A.pattern[0]);
    C.pattern.push_back(B. pattern[0]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");

    return C;
}

patternList single_sequence_join(patternList A, patternList B) {
    patternList C;
    int support = 0;
    for(const auto& tp : A.T_P) {
        vector<int> joined_p;
        auto min = min_element(A.T_P[tp.first].begin(), A.T_P[tp.first].end());
        for(const auto& scnd : B.T_P[tp.first]) {
            if(scnd > *min) {
                joined_p.push_back(scnd);
            } 
        }
        // vector<int> joined_p = intersect(A.T_P[tp.first], B.T_P[tp.first]);
        if(joined_p.size()) {
            C.T_P[tp.first] = joined_p;
            support++;
        }
    }

    C.support = support;
    C.pattern.push_back(A.pattern[0]);
    C.pattern.push_back("-1");
    C.pattern.push_back(B. pattern[0]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");

    return C;
}

patternList single_inter_join(patternList A, patternList B) {
    patternList C;
    int support = 0;
    for(const auto& tp : A.T_P) {
        vector<int> joined_p;
        if(B.T_P[tp.first + 1].size()) {
                for(const auto& scnd : B.T_P[tp.first + 1]) {
                    joined_p.push_back(scnd);
                }
        }

        // vector<int> joined_p = intersect(A.T_P[tp.first], B.T_P[tp.first]);
        if(joined_p.size()) {
            C.T_P[tp.first + 1] = joined_p;
            support++;
        }
    }

    C.support = support;
    C.pattern.push_back(A.pattern[0]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");
    C.pattern.push_back(B. pattern[0]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");

    return C;
}

//------------------------ K-sequence join ---------------------------------

bool areVectorsEqual(const vector<string>& v1, const vector<string>& v2) {
    if (v1.size() != v2.size()) {
        return false;
    }
    return equal(v1.begin(), v1.end(), v2.begin());
}

patternList K_itemset_join(patternList A, patternList B) {
    patternList C;
    vector<string> _A(A.pattern.begin(), A.pattern.begin() + A.pattern.size() -3);
    vector<string> _B(B.pattern.begin(), B.pattern.begin() + B.pattern.size() -3);
    if(A.pattern[_A.size()] == B.pattern[_B.size()]) {return C;}
    if(!areVectorsEqual(_A, _B)) {
        return C;
    }
    int support = 0;
    for(const auto& tp : A.T_P) {
        vector<int> joined_p = intersect(A.T_P[tp.first], B.T_P[tp.first]);
        if(joined_p.size()) {
            C.T_P[tp.first] = joined_p;
            support++;
        }
    }

    C.support = support;
    for(const auto& item : _A) {
        C.pattern.push_back(item);
    }
    int size = C.pattern.size();
    C.pattern.push_back(A.pattern[size]);
    C.pattern.push_back(B.pattern[size]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");

    return C;
}

patternList K_sequence_join(patternList A, patternList B) {
    patternList C;
    int flag = 0;
    vector<string> _A(A.pattern.begin(), A.pattern.begin() + A.pattern.size() -3);
    vector<string> _B(B.pattern.begin(), B.pattern.begin() + B.pattern.size() -3);
    vector<string> _BB(B.pattern.begin(), B.pattern.begin() + B.pattern.size() -4);
    if(!areVectorsEqual(_A, _B)) {
        if(!areVectorsEqual(_A, _BB)) {
            return C;
        }
        else {
            flag = 1;
        }
    }
    else {
        if(_B.back() != "-1" && _B.back() != "-2") {
            return C;
        }
    }
    int support = 0;
    for(const auto& tp : A.T_P) {
        vector<int> joined_p;
        auto min = min_element(A.T_P[tp.first].begin(), A.T_P[tp.first].end());
        for(const auto& scnd : B.T_P[tp.first]) {
            if(scnd > *min) {
                joined_p.push_back(scnd);
            } 
        }
        // vector<int> joined_p = intersect(A.T_P[tp.first], B.T_P[tp.first]);
        if(joined_p.size()) {
            C.T_P[tp.first] = joined_p;
            support++;
        }
    }

    C.support = support;
    for(const auto& item : _A) {
        C.pattern.push_back(item);
    }
    int shift = 0;
    if(flag == 1) {
        shift = 1;
        C.pattern.push_back("-1");
    }
    int size = C.pattern.size();
    C.pattern.push_back(A.pattern[size]);
    C.pattern.push_back("-1");
    C.pattern.push_back(B. pattern[size + shift]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");

    return C;
}

patternList K_inter_join(patternList A, patternList B) {
    patternList C;
    if(B.pattern.size() < 6) {
        return C;
    }
    vector<string> _A(A.pattern.begin(), A.pattern.begin() + A.pattern.size() -3);
    vector<string> _B(B.pattern.begin(), B.pattern.begin() + B.pattern.size() -4);
    vector<string> _BB(B.pattern.begin(), B.pattern.begin() + B.pattern.size() -5);
    if(!areVectorsEqual(_A, _B)) {
        if(!areVectorsEqual(_A, _BB)) {
            return C;
        }
    }
    int support = 0;
    for(const auto& tp : A.T_P) {
        vector<int> joined_p;
        if(B.T_P[tp.first + 1].size()) {
                for(const auto& scnd : B.T_P[tp.first + 1]) {
                    joined_p.push_back(scnd);
                }
        }

        // vector<int> joined_p = intersect(A.T_P[tp.first], B.T_P[tp.first]);
        if(joined_p.size()) {
            C.T_P[tp.first + 1] = joined_p;
            support++;
        }
    }

    C.support = support;
    for(const auto& item : A.pattern) {
        C.pattern.push_back(item);
    }
    int size = _B.size();
    C.pattern.push_back(B.pattern[size + 1]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");

    // cout << "support of inter : " << support << endl;

    return C;
}

//----------------------- main ---------------------------------------------
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

    int start = 0, end = 0;

    cout << "Looping single join for 2-frequent" << endl;
    for(const auto& A : uniqueValues) {
        for(const auto& B : uniqueValues) {
            patternList answer1 = single_itemset_join(uniqueValues[A.first], uniqueValues[B.first]);
            if(answer1.support >=4) {
                frequent_patterns[2][answer1.pattern] = answer1;
            }
            patternList answer2 = single_sequence_join(uniqueValues[A.first], uniqueValues[B.first]);
            if(answer2.support >=4) {
                frequent_patterns[2][answer2.pattern] = answer2;
            }
            patternList answer3 = single_inter_join(uniqueValues[A.first], uniqueValues[B.first]);
            if(answer3.support >=4) {
                frequent_patterns[2][answer3.pattern] = answer3;
            }
        }
    }

    cout << "Looping single join for K-frequent" << endl;
    int k_length = 2;
    while(frequent_patterns[k_length].size() != 0) {
        // if(k_length > 6){break;}
        cout << "Looping single join for " << k_length + 1 << "-frequent" << " size : " << frequent_patterns[k_length].size() <<endl;
        for(const auto& A : frequent_patterns[k_length]) {
            for(const auto& B : frequent_patterns[k_length]) {
                patternList answer1 = K_itemset_join(frequent_patterns[k_length][A.first], frequent_patterns[k_length][B.first]);
                if(answer1.support >=4) {
                    frequent_patterns[k_length + 1][answer1.pattern] = answer1;
                }
                patternList answer2 = K_sequence_join(frequent_patterns[k_length][A.first], frequent_patterns[k_length][B.first]);
                if(answer2.support >=4) {
                    frequent_patterns[k_length + 1][answer2.pattern] = answer2;
                }
                patternList answer3 = K_inter_join(frequent_patterns[k_length][A.first], frequent_patterns[k_length][B.first]);
                if(answer3.support >=4) {
                    frequent_patterns[k_length + 1][answer3.pattern] = answer3;
                }
            }
        }
        k_length++;
    }    

    for(const auto& index : frequent_patterns) {
        cout << "frequent " << index.first << "-Patterns : " << endl;
        for(const auto& freq : frequent_patterns[index.first]) {
            cout << "support" << " : " << frequent_patterns[index.first][freq.first].print_count() << endl;
            cout << "PatternLists : " << endl;
            frequent_patterns[index.first][freq.first].print_pattern();
            cout << endl;
        }
    }

    return 0;
}

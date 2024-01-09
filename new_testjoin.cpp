#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include<bits/stdc++.h>
#include <chrono>

using namespace std;
int patternNum;
// map<string,double> element_weight = {{"A", 1}, {"B", 0.8}, {"C", 1.1}, {"D", 1.5},{"E", 0.5}, {"F", 0.5}, {"G", 2}, {"H", 0.6}, {"I", 1}, {"J", 1}, {"K", 1.4}};
map<string,double> element_weight;
vector<vector<string>> database; // Global variable to store the database
vector<int> possibleLegth;
vector<string> Pm;
double MaxW = 0;
vector<double> lmaxw;
vector<double> lmmw;
double tmmw;

int algoType = 1;

struct patternList
{
    vector<string> pattern;
    int support = 0;
    int T_length = 1;
    double weight = 0;
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

        cout << "weight : " << weight << endl;
        cout << "T_length : " << T_length << endl;
    }
};

map<string, patternList> uniqueValues; // items with support count
map<int, map<vector<string>, patternList>> frequent_patterns; //frequent sequences after join

// --------------------------------------- dataset input from txt file -------------------------------------------
int readDatabaseFromFile(const string& filename) {
    ifstream file(filename); // Open the file

    if (!file.is_open()) {
        cout << "Error opening the dataset file." << endl;
        return 1;
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

    return 0;
}

// ------------------------------------element weight input from file---------------------------------
int readWeightFromFile(const string& filename) {
    ifstream file(filename); // Open the file

    if (!file.is_open()) {
        cout << "Error opening the weight file." << endl;
        return 1;
    }

    string line;
    while (getline(file, line)) { // Read each line
        stringstream ss(line);
        string key;
        double value;
        // Extracting the first and third columns
        ss >> key; // Read the first column as the key
        ss.ignore(256, ' '); // Ignore the first column
        ss.ignore(256, ' ');//Igonre the second column
        ss >> value; // Read the third column as the value
        element_weight[key] = value;
    }

    file.close(); // Close the file

    return 0;
}

//------------------------------Avg weight count------------------------------------------------------
vector<double> weightSeq(patternList seq) {
    vector<double> weightis;

    double weights = 0;
    double weightseq = 0;
    double elementNum = 0;
    double seqNum = 0;
    for(auto i : seq.pattern) {
        if(i != "-1" && i != "-2") {
            weights += element_weight[i];
            elementNum++;
        }
        else if(i == "-1") {
            weights = weights/elementNum;
            weightseq += weights;
            seqNum++;
            weights = 0;
            elementNum = 0;
        }
        else {
            weightis.push_back(weightseq/seqNum);
            weightseq = 0;
            seqNum = 0;
        }
    }

    return weightis;
}

//------------------------------WSUP ---------------------------------------------------------------

double WSUP( patternList A) {
    double weight = 0;

    vector<double> weightseq = weightSeq(A);

    for(auto i : weightseq) {
        weight += i;
    }

    weight = weight/weightseq.size();

    return weight*A.support/tmmw;
}

//------------------------------TWSPAN----------------------------------------------------------------
double TWSPAN(patternList A) {
    double weight = 0;

    // vector<double> weightseq = weightSeq(A);

    // for(auto i : weightseq) {
    //     weight += i;
    // }

    // weight = weight/weightseq.size();

    return MaxW*A.support/tmmw;
}

//------------------------------IUA condition check -------------------------------------------------
double TIUA(patternList A) {

    double weight = 0;

    for(auto T : A.T_P) {
        int position = T.first - (A.T_length);
        
        weight += lmmw[position];
    }

    return weight/tmmw;
}


//------------------------------Max Possible Weight Support ------------------------------------------
double MaxPossibleWeight(patternList A) {
    int maxspan = 2;
    double weightit = 0;
    map<int, double> MaxPWS;
    double max_pws = 0;

    vector<double> weightseq = weightSeq(A);

    for(auto i : weightseq) {
        weightit += i;
    }
    int inter_length = weightseq.size();
    weightit = weightit/inter_length;
    set<int> uniqueT;
    for (auto T = A.T_P.rbegin(); T != A.T_P.rend(); ++T) {
        int t = maxspan - A.T_length + 1;
        uniqueT.insert(t);
        for(auto newT: uniqueT) {
            int tempT = newT;

            double pws = (A.T_length - 1) * weightit;
            int i = T->first - 1;
            while(tempT--) {
                pws += (element_weight[Pm[i]] + (possibleLegth[i] - 1) * lmaxw[i])/possibleLegth[i];
                i++;
            }
            MaxPWS[newT] += pws/A.T_length;

            if(max_pws < MaxPWS[newT]) {max_pws = MaxPWS[newT];}
        }


        // if(pws > MaxPWS) {MaxPWS = pws;}
    }

    // double tmmw = accumulate(lmmw.begin(), lmmw.end(), 0);

    return max_pws/tmmw;
}

double weightCall(patternList A, int algo) {
    if(algo == 1) {
        return TWSPAN(A);
    }
    if(algo == 2) {
        return TIUA(A);
    }
    if(algo == 3) {
        return MaxPossibleWeight(A);
    }
    return 0;
}

// ---------------------------single unique item with possible sequence length-------------------------------------
void countUniqueValues() {
    int row_num = 0;
    for (const auto& row : database) {
        string maxPm = row[0];
        double max = 0;
        row_num++;
        set<string> uniqueInRow;
        int itemset_num = 1;
        int seqnum = 0;
        for (const auto& value : row) {
            if(value == "-1") {
                itemset_num++;
                if(seqnum == 0) {
                    Pm.push_back(maxPm);
                }
                seqnum++;
            }
            if (value != "-1" && value != "-2") {
                if(element_weight[value] > max) {max = element_weight[value];}
                if(uniqueInRow.find(value) == uniqueInRow.end()) {
                    vector<string> temp_vect{value, "-1", "-2"};
                    uniqueValues[value].add_num(1, temp_vect);
                    uniqueInRow.insert(value);
                }
                uniqueValues[value].add_T_P(row_num, itemset_num);
                if(MaxW < element_weight[value]) {MaxW = element_weight[value];}
                if(element_weight[maxPm] < element_weight[value] && seqnum == 0) {maxPm = value;}
            }
        }
        possibleLegth.push_back(itemset_num - 1);

        lmaxw.push_back(max);
    }

    patternNum = row_num;
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
    if(A.pattern[0] == B.pattern[0]) {C.weight = 0; return C;}
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

    if(support != 0) {
        C.T_length = A.T_length;
        C.weight = weightCall(C, algoType);
    }
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

    if(support != 0) {
        C.T_length = A.T_length;
        C.weight = weightCall(C, algoType);
    }

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

    if(support != 0) {
        C.T_length = A.T_length + 1;
        C.weight = weightCall(C, algoType);
    }

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
    if(A.pattern[_A.size()] == B.pattern[_B.size()]) {C.weight = 0; return C;}
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

    if(support != 0) {
        C.T_length = A.T_length;
        C.weight = weightCall(C, algoType);
    }

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
    if(B. pattern[size + shift] == "-1" || B. pattern[size + shift] == "-2") {C.support = 0;}
    C.pattern.push_back(A.pattern[size]);
    C.pattern.push_back("-1");
    C.pattern.push_back(B. pattern[size + shift]);
    C.pattern.push_back("-1");
    C.pattern.push_back("-2");

    if(support != 0) {
        C.T_length = A.T_length;
        C.weight = weightCall(C, algoType);
    }

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

    if(support != 0) {
        C.T_length = A.T_length + 1;
        C.weight = weightCall(C, algoType);
    }

    return C;
}

//-------------------------------answer print-------------------------------
void answer_print(int k_length) {
    cout << "frequent " << k_length << "-Patterns : " << endl;
    for(const auto& freq : frequent_patterns[k_length]) {
        cout << "support" << " : " << frequent_patterns[k_length][freq.first].print_count() << endl;
        cout << "PatternLists : " << endl;
        frequent_patterns[k_length][freq.first].print_pattern();
        cout << endl;
    }

    cout << "MaxW = " << MaxW << " tmmw = " << tmmw << endl;
}

//----------------------- main ---------------------------------------------
int main() {

    cout << "Algo Type: " << endl;
    cout << "1: TWSPAN" << endl;
    cout << "2: TIUA" << endl;
    cout << "3: MaxPWS" << endl;

    cin >> algoType;

    string data_file;
    string data_weight_file;

    cout << "DATASET File: ";
    cin >> data_file;
    cout << endl;

    cout << "DATA WEIGHT FILE: ";
    cin >> data_weight_file;
    cout << endl;

    double threshold = 0.5;
    cout << "threshold: ";
    cin >> threshold;
    cout << endl;

     // Record the start time
    auto start = chrono::high_resolution_clock::now();

    int D = readDatabaseFromFile(data_file); // Read database from file
    if(D == 1) return 1;
    int W = readWeightFromFile(data_weight_file);// Read data weight from file
    if(W == 1) return 1;

    countUniqueValues();
    tmmw = 0;
    int maxspan = 2;
    int ll = lmaxw.size();
    cout << "lmmw : " << endl;
    for(int lw = 0; lw < ll; lw++) {
        double max = lmaxw[lw];
        for(int j = lw; j < lw + maxspan; j++) {
            if(j == ll) {break;}
            cout << "max change : " << lmaxw[j] << endl;
            if(max < lmaxw[j]) {max = lmaxw[j];}
        }
        lmmw.push_back(max);
        cout << max << endl;
        tmmw += max;
    }

    int elNUM = 0;

    for(const auto& items : uniqueValues) {
        uniqueValues[items.first].weight = weightCall(uniqueValues[items.first], algoType);
        if(uniqueValues[items.first].weight >= threshold) {
            elNUM++;
        }
        cout << items.first << " : " << uniqueValues[items.first].print_count() << endl;
        cout << "PatternLists : " << endl;
        uniqueValues[items.first].print_pattern();
        cout << endl;
    }

    // int start = 0, end = 0;
    
    cout << "Looping single join for 2-frequent" << endl;
    for(const auto& A : uniqueValues) {
        for(const auto& B : uniqueValues) {
            if(uniqueValues[A.first].weight >= threshold && uniqueValues[B.first].weight >= threshold) {
                // cout << A.first[0] << " " << B.first[0] << endl;

                patternList answer1 = single_itemset_join(uniqueValues[A.first], uniqueValues[B.first]);
                if(answer1.weight >=threshold) {
                    // cout << answer1.weight << " " << answer1.support << endl;
                    frequent_patterns[2][answer1.pattern] = answer1;
                }
                patternList answer2 = single_sequence_join(uniqueValues[A.first], uniqueValues[B.first]);
                if(answer2.weight >=threshold) {
                    // cout << answer2.weight << " " << answer2.support << endl;
                    frequent_patterns[2][answer2.pattern] = answer2;
                }
                patternList answer3 = single_inter_join(uniqueValues[A.first], uniqueValues[B.first]);
                if(answer3.weight >=threshold) {
                    // cout << answer3.weight << " " << answer3.support << endl;
                    frequent_patterns[2][answer3.pattern] = answer3;
                }    
            }

        }
    }

    cout << "Number of unique values (excluding -1 and -2): " << elNUM << endl;

    cout << "Looping single join for K-frequent" << endl;
    int k_length = 2;
    while(frequent_patterns[k_length].size() != 0) {
        // if(k_length == 2) {
        //     answer_print(k_length);       
        // }
         
        
        // if(k_length > 6){break;}
        cout << "Looping single join for " << k_length + 1 << "-frequent" << " size : " << frequent_patterns[k_length].size() <<endl;
        for(const auto& A : frequent_patterns[k_length]) {
            for(const auto& B : frequent_patterns[k_length]) {
                // cout << "I'm In..." << A.first[0] << " " << B.first[0] << endl;

                patternList answer1 = K_itemset_join(frequent_patterns[k_length][A.first], frequent_patterns[k_length][B.first]);
                if(answer1.weight >=threshold) {
                    frequent_patterns[k_length + 1][answer1.pattern] = answer1;
                }
                patternList answer2 = K_sequence_join(frequent_patterns[k_length][A.first], frequent_patterns[k_length][B.first]);
                if(answer2.weight >=threshold) {
                    frequent_patterns[k_length + 1][answer2.pattern] = answer2;
                }
                patternList answer3 = K_inter_join(frequent_patterns[k_length][A.first], frequent_patterns[k_length][B.first]);
                if(answer3.weight >=threshold) {
                    frequent_patterns[k_length + 1][answer3.pattern] = answer3;
                }

            }
        }
        frequent_patterns.erase(k_length);
        k_length++;
    }    

    // for(const auto& index : frequent_patterns) {
    //     cout << "frequent " << index.first << "-Patterns : " << endl;
    //     for(const auto& freq : frequent_patterns[index.first]) {
    //         cout << "support" << " : " << frequent_patterns[index.first][freq.first].print_count() << endl;
    //         cout << "PatternLists : " << endl;
    //         frequent_patterns[index.first][freq.first].print_pattern();
    //         cout << endl;
    //     }
    // }

    cout << "patternNum : " << patternNum << endl;

    // Record the end time
    auto end = chrono::high_resolution_clock::now();

    // Calculate the duration and convert it to microseconds
    chrono::duration<double, std::micro> duration = end - start;

    // Print the execution time
    cout << "Execution time: " << duration.count()/double(1000000) << " seconds" << endl;

    return 0;
}

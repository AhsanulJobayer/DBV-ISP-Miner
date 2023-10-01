#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <bitset>

using namespace std;

class Patternlist {
public:
    string sequence;
    int finalT;
    int frequency;
    int bit_vector;
    vector<vector<int>> T_P;
    vector<int> children;

public:
    Patternlist(const string& _sequence) : sequence(_sequence), finalT(0), frequency(0), bit_vector(0) {
        T_P.resize(64000);
    }

    void position(int t, int p) {
        T_P[t].push_back(p);
        if (t > finalT) {
            frequency++;
            finalT = t;
            bit_vector |= (1 << (4 * 3 - finalT));
        }
    }

    vector<int> bit_positions() {
        vector<int> positions;
        int position = 12;
        int number = bit_vector;
        while (number > 0) {
            if (number & 1) {
                positions.push_back(position);
                if (position > finalT) {
                    finalT = position;
                }
            }
            position--;
            number >>= 1;
        }
        return positions;
    }

    void input_T_P(int index, int value) {
        T_P[index].push_back(value);
    }

    int get_frequency() {
        return frequency;
    }

    void print_info() {
        cout << "sequence: " << sequence << endl;
        cout << "T and P values:" << endl;
        for (int i = 1; i <= finalT; i++) {
            if (!T_P[i].empty()) {
                cout << "T: " << i << " - P: ";
                for (int j : T_P[i]) {
                    cout << j << ' ';
                }
                cout << endl;
            }
        }
        cout << "Frequency: " << frequency << endl;
        cout << "Bit vector of the sequence: " << bit_vector << endl;
        cout << "Bit position of 1's in the bit vector: ";
        bitset<12> binaryBit(bit_vector);
        cout << "Binary bit: " << binaryBit << endl;
        vector<int> positions = bit_positions();
        cout << "List of positions: ";
        for (int pos : positions) {
            cout << pos << ' ';
        }
        cout << endl;
    }
};

class Tree {
private:
    Patternlist* root;

public:
    Tree() : root(nullptr) {}

    void add_root(const string& root_value) {
        root = new Patternlist(root_value);
    }

    void add_child(Patternlist* parent, Patternlist* child_patternlist) {
        parent->children.push_back(child_patternlist);
    }

    vector<Patternlist*>& get_nodes_at_level(int level) {  // Update the return type
        return _get_nodes_at_level(root, level, 0);
    }

    vector<Patternlist*> _get_nodes_at_level(Patternlist* current_node, int target_level, int current_level) {
        vector<Patternlist*> nodes;
        if (current_level == target_level) {
            nodes.push_back(current_node);
            return nodes;
        }

        for (Patternlist* child : current_node->children) {
            vector<Patternlist*> child_nodes = _get_nodes_at_level(child, target_level, current_level + 1);
            nodes.insert(nodes.end(), child_nodes.begin(), child_nodes.end());
        }

        return nodes;
    }
};


int minsup = 4;

int main() {
    map<string, Patternlist> items;
    int row = 1;
    int column = 1;
    ifstream inputFile("dataset.txt");

    if (!inputFile) {
        cerr << "Failed to open the input file." << endl;
        exit(1);
    }

    for (string line; getline(inputFile, line);) {
        istringstream iss(line);
        vector<string> items_list(istream_iterator<string>(iss), {});
        for (const string& item : items_list) {
            if (item == "-1") {
                column += 1;
            } else if (item == "-2") {
                row += 1;
                column = 1;
            } else {
                if (items.find(item) == items.end()) {
                    items[item] = Patternlist(item);
                }
                items[item].position(row, column);
            }
        }
    }

    inputFile.close();

    ifstream inputFile2("dataset.txt");
    for (string line; getline(inputFile2, line);) {
        istringstream iss(line);
        vector<string> items_list(istream_iterator<string>(iss), {});
        for (const string& item : items_list) {
            if (item == "-1") {
                cout << ", ";
            } else if (item == "-2") {
                cout << "-2 " << endl;
                cout << endl;
            } else {
                cout << item << " ";
            }
        }
        cout << endl;
    }

    vector<string> keys_to_remove;

    for (auto& pair : items) {
        string item = pair.first;
        Patternlist& frequency = pair.second;
        int support = frequency.get_frequency();
        if (support < minsup) {
            keys_to_remove.push_back(item);
        }
    }

    for (const string& key : keys_to_remove) {
        items.erase(key);
    }

    cout << "(Freq: ";
    for (const auto& pair : items) {
        string item = pair.first;
        Patternlist frequency = pair.second;
        int support = frequency.get_frequency();
        cout << item << ":" << support << ", ";
    }
    cout << ")" << endl;
    cout << "sequences info:" << endl;

    for (const auto& pair : items) {
        string item = pair.first;
        Patternlist item_info = pair.second;
        cout << "Sequence: " << item << endl;
        item_info.print_info();
    }

    // Tree structure maintenance
    Tree tree;
    Patternlist null_pattern("null");
    tree.add_root(null_pattern);

    for (auto& pair : items) {
        Patternlist& freq = pair.second;
        tree.add_child(tree.root, &freq);
    }

    // Level 1 operation
    vector<Patternlist*> new_items = tree.get_nodes_at_level(1);

    cout << "Checking level 1 in tree: " << endl;
    for (Patternlist* checks : new_items) {
        cout << "sequence------- " << checks->get_sequence() << " and frequency---- " << checks->get_frequency() << endl;
    }

    // Level 2 operation
    int index = 2;

    while (!new_items.empty()) {
        cout << "checking level " << index << " in tree: " << endl;
        for (Patternlist* checks : new_items) {
            cout << "sequence------- " << checks->get_sequence() << " and frequency---- " << checks->get_frequency() << endl;
        }

        for (Patternlist* X : new_items) {
            for (Patternlist* Y : new_items) {
                // k-itemset join
                Patternlist iJoin = k_itemmset_join(*X, *Y);
                if (iJoin.get_frequency() >= minsup) {
                    tree.add_child(X, &iJoin);
                }
                // K-sequence join
                iJoin = k_sequence_join(*X, *Y);
                if (iJoin.get_frequency() >= minsup) {
                    tree.add_child(X, &iJoin);
                }
                // K-inter join
                iJoin = k_intersequence_join(*X, *Y);
                if (iJoin.get_frequency() >= minsup) {
                    tree.add_child(X, &iJoin);
                }
            }
        }

        index++;
        new_items = tree.get_nodes_at_level(index);
    }

    return 0;
}

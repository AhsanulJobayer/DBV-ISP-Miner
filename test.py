import time
# Record the start time
start_time = time.time()

class Patternlist:
    def __init__(self, _sequence):
        self.sequence = _sequence
        self.finalT = 0
        self.frequency = 0
        self.bit_vector = 0
        self.T_P = [[] for _ in range(64000)]
        self.children = []
    
    def position(self, t, p):
        self.T_P[t].append(p)
        if t > self.finalT:
            self.frequency += 1
            self.finalT = t
            self.bit_vector |= (1 << (4* 3 - self.finalT))  
    
    def bit_positions(self):
        positions = []
        position = 12
        number = self.bit_vector
        while number > 0:
            if number & 1:
                positions.append(position)
                if(position > self.finalT):
                    self.finalT = position
            position -= 1
            number >>= 1
        return positions

    def input_T_P(self, index, value):
        self.T_P[index].append(value)
    
    def get_frequency(self):
        return self.frequency
    
    def print_info(self):
        print("sequence:", self.sequence)
        print("T and P values:")
        for i in range(1, self.finalT + 1):
            if self.T_P[i]:
                print("T:", i, "- P:", end=" ")
                print(*self.T_P[i])
                print()
        print("Frequency: ", self.frequency)
        print("Bit vector of the sequence:", self.bit_vector)
        print("Bit position of 1's in the bit vector:", end=" ")
        print("Binary bit:", bin(self.bit_vector)[2:].zfill(self.finalT))
        print("List of positions: ", self.bit_positions())


items = {}

# --------------------------------------------tree---------------------------------------------------------
class Tree:
    def __init__(self):
        self.root = None

    def add_root(self, root_value):
        self.root = Patternlist(root_value)

    def add_child(self, parent, child_patternlist):
        parent.children.append(child_patternlist)

    def get_nodes_at_level(self, level):
        return self._get_nodes_at_level(self.root, level, 0)

    def _get_nodes_at_level(self, current_node, target_level, current_level):
        if current_level == target_level:
            return [current_node]
        
        nodes = []
        for child in current_node.children:
            nodes.extend(self._get_nodes_at_level(child, target_level, current_level + 1))
        
        return nodes

# ---------------------------------------single join -------------------------------------------------------
def single_itemset_join(seq1, seq2):
    answer = Patternlist(seq1.sequence + " " + seq2.sequence + " -1 -2")
    answer.bit_vector = seq1.bit_vector & seq2.bit_vector
    t_value = answer.bit_positions()

    count = 1
    if seq1.sequence == seq2.sequence:
        count = 2
    print("itemset join")
    for i in t_value:
        match = 0
        value_count = 0  # Initialize value_count
        for value1 in seq1.T_P[i]:
            if value1 in seq2.T_P[i]:
                answer.T_P[i].append(value1)
                value_count += seq2.T_P[i].count(value1)  # Count occurrences in seq2.T_P[i]
                if value_count >= count:
                    match = 1
        answer.frequency += match
        print(f"value1: {value1}, value_count: {value_count}")  # Print for debugging purposes

    return answer


def single_sequence_join(seq1, seq2):
    answer = Patternlist(seq1.sequence + " -1 " + seq2.sequence + " -1 -2")
    answer.bit_vector = seq1.bit_vector & seq2.bit_vector
    t_value = answer.bit_positions()
    print("sequence join")
    for i in t_value:
        match = 0
        for value1 in seq1.T_P[i]:
            for val in seq2.T_P[i]:  # Define val within this loop
                if val > value1:
                    answer.T_P[i].append(val)
                    match = 1
        answer.frequency += match

    return answer

def single_intersequence_join(seq1, seq2):
    answer = Patternlist(seq1.sequence + " -1 -2 " + seq2.sequence + " -1 -2")
    answer.bit_vector = ((seq1.bit_vector >> 1) & seq2.bit_vector)
    t_value = answer.bit_positions()
    print("Check bit vector if bin or dec: ", seq1.bit_vector)
    for i in t_value:
        match = 0
        for value1 in seq2.T_P[i]:
            answer.T_P[i].append(value1)
            match = 1
        answer.frequency += match

    return answer

# -------------------------------K join ---------------------------------------------------
def k_itemmset_join(seq1, seq2):
    items1 = seq1.sequence.split()  # Split sequence 1 into items
    items2 = seq2.sequence.split()  # Split sequence 2 into items

    # Compare items up to the second-to-last item
    if items1[:-3] == items2[:-3]:
        answer = Patternlist(" ".join(items1[:-3]) + " " + items1[-3] + " " + items2[-3] + " -1 -2")
    else:
        answer = Patternlist("none")
        return answer
    # answer = Patternlist(seq1.sequence + " " + seq2.sequence + " -1 -2")
    answer.bit_vector = seq1.bit_vector & seq2.bit_vector
    t_value = answer.bit_positions()
    count = 1
    if items1[-3] == items2[-3]:
        count = 2
    print("k-itemset join")
    for i in t_value:
        match = 0
        value_count = 0  # Initialize value_count
        for value1 in seq1.T_P[i]:
            if value1 in seq2.T_P[i]:
                answer.T_P[i].append(value1)
                value_count += seq2.T_P[i].count(value1)  # Count occurrences in seq2.T_P[i]
                if value_count >= count:
                    match = 1
        answer.frequency += match
        print(f"value1: {value1}, value_count: {value_count}")  # Print for debugging purposes

    return answer

def k_sequence_join(seq1, seq2):
    items1 = seq1.sequence.split()  # Split sequence 1 into items
    items2 = seq2.sequence.split()  # Split sequence 2 into items

    # Compare items up to the second-to-last item
    if items1[:-3] == items2[:-3]:
        answer = Patternlist(" ".join(items1[:-3]) + " " + items1[-3] + " -1 " + items2[-3] + " -1 -2")
    elif items1[:-3] == items2[:-4]:
        answer = Patternlist(" ".join(items1[:-3]) + " " + items1[-3] + " -1 " + items2[-3] + " -1 -2")
    else:
        answer = Patternlist("none")
        return answer
    answer.bit_vector = seq1.bit_vector & seq2.bit_vector
    t_value = answer.bit_positions()
    print("sequence join")
    for i in t_value:
        match = 0
        for value1 in seq1.T_P[i]:
            for val in seq2.T_P[i]:  # Define val within this loop
                if val > value1:
                    answer.T_P[i].append(val)
                    match = 1
        answer.frequency += match

    return answer

def k_intersequence_join(seq1, seq2):
    items1 = seq1.sequence.split()  # Split sequence 1 into items
    items2 = seq2.sequence.split()  # Split sequence 2 into items

    # Compare items up to the second-to-last item
    if items1[:-3] == items2[:-5]:
        answer = Patternlist(" ".join(items1[:-3]) + " " + items1[-3] + " -1 -2 " + items2[-3] + " -1 -2")
    elif items1[:-4] == items2[:-5]:
        answer = Patternlist(" ".join(items1[:-3]) + " " + items1[-3] + " -1 -2 " + items2[-3] + " -1 -2")
    else:
        answer = Patternlist("none")
        return answer
    answer.bit_vector = ((seq1.bit_vector >> 1) & seq2.bit_vector)
    t_value = answer.bit_positions()
    print("Check bit vector if bin or dec: ", seq1.bit_vector)
    for i in t_value:
        match = 0
        for value1 in seq2.T_P[i]:
            answer.T_P[i].append(value1)
            match = 1
        answer.frequency += match

    return answer

# -----------------------------------Main functions with file reads -------------------------------------

minsup = 4

inputFile = open("dataset.txt", "r")
if not inputFile:
    print("Failed to open the input file.")
    exit(1)

row = 1
column = 1

for line in inputFile:
    items_list = line.strip().split()
    for item in items_list:
        if item == "-1":
            column += 1
        elif item == "-2":
            row += 1
            column = 1
        else:
            if item not in items:
                items[item] = Patternlist(item)
            items[item].position(row, column)
    # items[].reverse_bit_vector()

inputFile.close()

inputFile = open("dataset.txt", "r")
for line in inputFile:
    items_list = line.strip().split()
    for item in items_list:
        if item == "-1":
            print(", ", end="")
        elif item == "-2":
            print("-2 ")
            print()
        else:
            print(item, " ", end="")
    print()

keys_to_remove = []

for item, frequency in items.items():
    support = frequency.get_frequency()
    if support < minsup:
        keys_to_remove.append(item)

for key in keys_to_remove:
    items.pop(key)

print("(Freq: ", end="")
for item, frequency in items.items():
    support = frequency.get_frequency()
    print(item, ":", support, ", ", end="")
print(")")
print("sequences info:")

for item, item_info in items.items():
    print("Sequence:", item)
    item_info.print_info()

# tree structure maintanance
tree = Tree()
null_pattern = Patternlist("null")
tree.add_root(null_pattern)
for one_length, freq in items.items():
    tree.add_child(tree.root, freq)

# ------level 1 operation--------
new_items = tree.get_nodes_at_level(1)

print("Checking level 1 in tree: ")
for checks in new_items:
    print("sequence------- ", checks.sequence, " and frequency---- ", checks.frequency)

# -----level 2 operation---------
for X in new_items:
    for Y in new_items:
        # itemset join
        iJoin = single_itemset_join(X, Y)
        if iJoin.frequency >= minsup:
            tree.add_child(X, iJoin)
        # sequence join
        iJoin = single_sequence_join(X, Y)
        if iJoin.frequency >= minsup:
            tree.add_child(X, iJoin)
        # inter join
        iJoin = single_intersequence_join(X, Y)
        if iJoin.frequency >= minsup:
            tree.add_child(X, iJoin)

index = 2

new_items = tree.get_nodes_at_level(index)

while new_items:
    print("checking level",  index, " in tree: ")
    for checks in new_items:
        print("sequence------- ", checks.sequence, " and frequency---- ", checks.frequency)

    for X in new_items:
        for Y in new_items:
            #k-itemset join
            iJoin = k_itemmset_join(X, Y)
            if iJoin.frequency >= minsup:
                tree.add_child(X, iJoin)
            #K-sequence join
            iJoin = k_sequence_join(X, Y)
            if iJoin.frequency >= minsup:
                tree.add_child(X, iJoin)
            # K-inter join
            iJoin = k_intersequence_join(X, Y)
            if iJoin.frequency >= minsup:
                tree.add_child(X, iJoin)
    index+=1
    new_items = tree.get_nodes_at_level(index)

# print("checking level 3 in tree: ")
# for checks in new_items:
#     print("sequence------- ", checks.sequence, " and frequency---- ", checks.frequency)

# --------------------------- visualize the tree ------------------------------------------
# import networkx as nx
# import matplotlib.pyplot as plt

# def visualize_tree(tree):
#     G = nx.DiGraph()

#     def add_nodes_and_edges(node, parent=None):
#         G.add_node(node.sequence)
#         if parent is not None:
#             G.add_edge(parent.sequence, node.sequence)
#         for child in node.children:
#             add_nodes_and_edges(child, node)

#     add_nodes_and_edges(tree.root)

#     pos = nx.spring_layout(G)
#     plt.figure(figsize=(12, 8))
#     nx.draw(G, pos, with_labels=True, node_size=1000, node_color='skyblue', font_size=10, font_color='black', font_weight='bold')
#     plt.title("Tree Visualization")
#     plt.show()

# visualize_tree(tree)

# Record the end time
end_time = time.time()

# Calculate and print the elapsed time
elapsed_time = end_time - start_time
print(f"Elapsed time: {elapsed_time} seconds")
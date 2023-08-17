class Patternlist:
    def __init__(self, _sequence):
        self.sequence = _sequence
        self.finalT = 0
        self.frequency = 0
        self.bit_vector = 0
        self.T_P = [[] for _ in range(64000)]
    
    def position(self, t, p):
        self.T_P[t].append(p)
        if t > self.finalT:
            self.frequency += 1
            self.finalT = t
            self.bit_vector |= (1 << (self.finalT - 1))
    


    
    def bit_positions(self):
        positions = []
        position = 1
        number = self.bit_vector
        while number > 0:
            if number & 1:
                positions.append(position)
            position += 1
            number >>= 1
        return positions
    
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


items = {}

def single_itemset_join(seq1, seq2):
    answer = Patternlist(seq1.sequence + " " + seq2.sequence + " -1 -2")
    answer.bit_vector = seq1.bit_vector & seq2.bit_vector
    t_value = answer.bit_positions()
    print("item set join")
    for i in t_value:
        match = 0
        for value1 in seq1.T_P[i]:
            if value1 in seq2.T_P[i]:
                answer.T_P[i].append(value1)
                match = 1
        answer.frequency += match

    return answer

def single_sequence_join(seq1, seq2):
    answer = Patternlist(seq1.sequence + " -1 " + seq2.sequence + " -1 -2")
    answer.bit_vector = seq1.bit_vector & seq2.bit_vector
    t_value = answer.bit_positions()
    print("sequence join")
    for i in t_value:
        match = 0
        for value1 in seq1.T_P[i]:
            if (value1 + 1) in seq2.T_P[i]:
                answer.T_P[i].append(seq2.T_P[i])
                match = 1
        answer.frequency += match

    return answer

def single_intersequence_join(seq1, seq2):
    answer = Patternlist(seq1.sequence + " -1 -2 " + seq2.sequence + " -1 -2")
    answer.bit_vector = (seq1.bit_vector >> 1) & seq2.bit_vector
    t_value = answer.bit_positions()
    
    for i in t_value:
        match = 0
        for value1 in seq1.T_P[i]:
            if (value1 + 1) in seq2.T_P[i + 1]:
                answer.T_P[i].append(value1)
                match = 1
        answer.frequency += match

    return answer


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

print("checking join operation")
join = single_itemset_join(items["A"], items["B"])
join.print_info()
join1 = single_sequence_join(items["A"], items["B"])
join1.print_info()
join2 = single_intersequence_join(items["A"], items["B"])
join2.print_info()
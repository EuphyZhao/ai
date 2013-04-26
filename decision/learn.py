#!/usr/bin/python

from math import log
import sys

# attrs = ['Alt', 'Bar', 'Fri', 'Hun', 'Pat',
#          'Price', 'Rain', 'Res', 'Type', 'Est']

# maps value to id
val2id = []
# maps id to value
id2val = []
goalattr = -1

class DecisionTree:
    def __init__(self, pivot):
        self.pivot = pivot
        self.children = []

    def __str__(self):
        pass

    def add_child(self, child):
        self.children.append(child)

def isleaf(tree):
    return not isinstance(tree, DecisionTree)

class Codegen:
    def __init__(self, output=sys.stdout):
        self.f = output
        self.indent = 0
    
    def fill(self, text=''):
        self.f.write('\n' + self.indent * '\t' + text)

    def write(self, text):
        self.f.write(text)

    def enter(self):
        self.indent += 1

    def leave(self):
        self.indent -= 1
        
    def gen_header(self):
        pass

    def gen_parse(self):
        self.fill('line = open(testfile).readlines()[0]')
        self.fill("record = [field.strip() for field in line.split(',')]")

    def gen_func(self):
        self.fill('def classify(testfile):')
        self.enter()
        # generate parsing code
        self.gen_parse()
        self.gen_classify()
        self.leave()
        self.fill()

    # recursively call gen_classify to generate code
    # only need to change the indentation level
    def gen_classify(self, tree):
        if isleaf(tree):
            self.fill("return " + str(id2val[goalattr][tree]))
            return
        values = id2val[tree.pivot]
        # generate the first if
        for idx in range(len(values)):
            if (idx == 0):
                self.fill(str.format("if record[%d]==%s:" % (tree.pivot, values[idx])))
            else:
                self.fill(str.format("elif record[%d]==%s:" % (tree.pivot, values[idx])))
            self.enter()
            self.gen_classify(tree.children[idx])
            self.leave()


    # generate code for this decision tree    
    # the code generated is a single function
    # that consists mostly of branches
    # the essence is to encode the tree model
    # into your code, instead of the other way around
    def gencode(self, tree):
        self.gen_header()
        self.gen_parse()
        self.gen_classify(tree)
        self.fill()
        

def majority(examples):
    # binary classification only
    count = [0, 0]
    for example in examples:
        count[example[-1]] += 1
    return 0 if count[0] > count[1] else 1

def all_same(examples):
    assert examples
    result = examples[0][-1]
    for example in examples[1:]:
        if example[-1] != result: return False
    return True
        
def count_positive(examples):
    return len(filter(lambda x : x[-1]==1, examples))

def count_negative(examples):
    return len(filter(lambda x : x[-1]==0, examples))


def importance(examples, attr):
    def boolean_entrophy(p, n):
        # entropy is 0
        if p == 0 or n == 0: return 0
        q = 1.0 * p / (p + n)
        return -(q*log(q,2) + (1-q)*log(1-q,2))

    remainder = 0
    d = len(val2id[attr])
    for k in range(d):
        exsi = select(examples, attr, k)
        positives = count_positive(exsi)
        negatives = count_negative(exsi)
        e = 1.0 * len(exsi) / len(examples) \
            * boolean_entrophy(positives, negatives)
        remainder += e

    positives = count_positive(examples)
    negatives = count_negative(examples)
    gain = boolean_entrophy(positives, negatives) - remainder
    return gain
        
def select(examples, attr, val):
    match = filter(lambda x: x[attr]==val, examples)
    return match

def decision_tree_learning(examples, attributes, parent_examples):
    if not examples: return majority(examples)
    if all_same(examples): return examples[0][-1]
    if not attributes: return majority(examples)

    gains = [importance(examples, attr) for attr in attributes]
    pivot = gains.index(max(gains))
    
    tree = DecisionTree(pivot)
    d = len(val2id[pivot])
    for v in range(d):
        exs = select(examples, pivot, v)
        nattributes = filter(lambda x: x != pivot, attributes)
        subtree = decision_tree_learning(exs, nattributes, examples)
        assert isinstance(subtree, DecisionTree) or isinstance(subtree, int)
        tree.add_child(subtree)
    return tree
        

def print_examples(examples):
    for example in examples:
        print decode(example)

def decode(example):
    record = [id2val[item[0]][item[1]] for item in enumerate(example)]
    return record

def encode(record):
    global val2id, id2val
    example = []
    for f in range(len(record)):
        domain = val2id[f]
        coding = domain.get(record[f], len(domain))
        if coding == len(domain):
            val2id[f].update({record[f]:coding})
            id2val[f].update({coding:record[f]})
        example.append(coding)
    return example

def read_data(datafile):
    global val2id, id2val, goalattr
    examples = []
    lines = open(datafile).readlines()
    assert lines

    fields = len(lines[0].split(','))
    # initialize domain mapping
    val2id = [{} for i in range(fields)]
    id2val = [{} for i in range(fields)]
    goalattr = fields - 1
    val2id[goalattr] = {'Yes':1, 'No':0}
    id2val[goalattr] = {1:'Yes', 0:'No'}

    for line in lines:
        # extract the record
        record = [field.strip() for field in line.split(',')]
        # encode record with integer category
        example = encode(record)
        examples.append(example)
    return examples

def classify(tree, record):
    example = encode(record)
    while isinstance(tree, DecisionTree):
        child = example[tree.pivot]
        tree = tree.children[child]
    result = id2val[goalattr][tree]
    assert result == record[-1]
    return result

def main(args):
    datafile = args[0]
    testfile = args[1]
    examples = read_data(datafile)
    print examples
    print_examples(examples)

    attributes = list(range(len(id2val)-1))
    print attributes
    tree = decision_tree_learning(examples, attributes, None)

    testdata = open(testfile).readlines()
    for line in testdata:
        test = [field.strip() for field in line.split(',')]
        print test
        result = classify(tree, test)
        print "Classifying result:", result

    codegen = Codegen()
    codegen.gencode(tree)

if __name__=='__main__':
    training = 'restaurant.csv'
    testing = 'test.csv'
    main([training, testing])

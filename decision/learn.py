#!/usr/bin/python

import sys
from math import log

val2id = [] # maps value to id
id2val = [] # maps id to value

outfile="classify.py"
    
class DecisionTree:
    def __init__(self, pivot):
        self.pivot = pivot
        self.children = []

    def __str__(self):
        import StringIO
        s = StringIO.StringIO()
        print_tree(self, s)
        return s.getvalue()

    def add_child(self, child):
        self.children.append(child)

def print_tree(tree, output=sys.stdout):
    f = output
    def _print_tree(tree, level):
        f.write('\t' * level)
        if isleaf(tree):
            f.write(id2val[-1][tree] + '\n')
        else:
            f.write('*'+str(tree.pivot)+'*:\n')
            for c in enumerate(tree.children):
                f.write('\t' * level)
                f.write('<'+id2val[tree.pivot][c[0]]+'>\n')
                _print_tree(c[1], level+1)
        
    _print_tree(tree, 0)

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
        self.write('#!/usr/bin/python\n')
        self.write('import sys\n')
        self.fill()

    def gen_main(self):
        self.fill("if __name__=='__main__':")
        self.enter()
        self.fill("if len(sys.argv) != 2:")
        self.enter()
        self.fill("print 'Usage: python classify.py [testfile]'")
        self.fill("sys.exit()")
        self.leave() # leave if
        self.fill("testfile=sys.argv[1]")
        self.fill('lines = open(testfile).readlines()')
        self.fill('for line in lines:')
        self.enter()
        self.fill("record = [field.strip() for field in line.split(',')]")
        self.fill("print 'Classifying:', record") 
        self.fill("result=classify(record)")
        self.fill("print '-->', result")
        self.leave() # leave for
        self.leave() # leave if __main__
        self.fill()

    def gen_func(self, tree):
        self.fill('def classify(record):')
        self.enter()
        self.gen_classify(tree)
        self.leave() # leave def
        self.fill()

    # recursively call gen_classify to generate code
    # only need to change the indentation level
    def gen_classify(self, tree):
        if isleaf(tree):
            self.fill('return "' + str(id2val[-1][tree]) + '"')
            return
        values = id2val[tree.pivot]
        for idx in range(len(values)):
            if (idx == 0):
                self.fill(str.format('if record[%d]=="%s":' % (tree.pivot, values[idx])))
            else:
                self.fill(str.format('elif record[%d]=="%s":' % (tree.pivot, values[idx])))
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
        self.gen_func(tree)
        self.gen_main()

def majority(examples):
    # generic majority
    count = [0] * len(id2val[-1])
    for example in examples:
        count[example[-1]] += 1
    return count.index(max(count))

def all_same(examples):
    assert examples
    result = examples[0][-1]
    for example in examples[1:]:
        if example[-1] != result: return False
    return True

# generic count
def count_value(examples, attr, val):
    return len(filter(lambda x : x[attr]==val, examples))

        
# def count_positive(examples):
#    return len(filter(lambda x : x[-1]==1, examples))
# def count_negative(examples):
#     return len(filter(lambda x : x[-1]==0, examples))


def importance(examples, attr):
    # generic entrophy
    def entrophy(hist):
        total = sum(hist)
        hist = filter(lambda x : x > 0, hist)
        qs = [1.0 * h / total for h in hist]
        e = -sum([q * log(q, 2) for q in qs])
        return e

    # def boolean_entrophy(p, n):
    #     if p == 0 or n == 0: return 0 # entropy is 0
    #     q = 1.0 * p / (p + n)
    #     e =  -(q*log(q,2) + (1-q)*log(1-q,2))
    #     print e, entrophy([p, n])
    #     assert entrophy([p, n]) == e
    #     return e

    remainder = 0
    d = len(val2id[attr])
    goals = len(val2id[-1])
    for k in range(d):
        exsi = select(examples, attr, k)

        hist = [count_value(exsi, -1, v) for v in range(goals)]
        # positives = count_positive(exsi)
        # negatives = count_negative(exsi)
        # e = 1.0 * len(exsi) / len(examples) \
        #     * boolean_entrophy(positives, negatives)
        e = 1.0 * len(exsi) / len(examples) \
            * entrophy(hist)
        remainder += e

    hist = [count_value(examples, -1, v) for v in range(goals)]
    gain = entrophy(hist) - remainder

    # positives = count_positive(examples)
    # negatives = count_negative(examples)
    # gain = boolean_entrophy(positives, negatives) - remainder
    return gain
        
def select(examples, attr, val):
    match = filter(lambda x: x[attr]==val, examples)
    return match

def decision_tree_learning(examples, attributes, parent_examples):
    if not examples: return majority(parent_examples)
    if all_same(examples): return examples[0][-1]
    if not attributes: return majority(examples)

    gains = [importance(examples, attr) for attr in attributes]
    pivot = attributes[gains.index(max(gains))]
    
    tree = DecisionTree(pivot)
    d = len(val2id[pivot])
    for k in range(d):
        exs = select(examples, pivot, k)
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
    global val2id, id2val
    examples = []
    lines = open(datafile).readlines()
    assert lines

    fields = len(lines[0].split(','))
    # initialize domain mapping
    val2id = [{} for i in range(fields)]
    id2val = [{} for i in range(fields)]

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
    result = id2val[-1][tree]
    assert result == record[-1]
    return result

def main(args):
    datafile = args[0]
    testfile = args[1]
    output = args[2]
    examples = read_data(datafile)

    attributes = list(range(len(id2val)-1))
    tree = decision_tree_learning(examples, attributes, None)
    print "Decision tree:"
    print tree
    
    testdata = open(testfile).readlines()
    for line in testdata:
        test = [field.strip() for field in line.split(',')]
        print test
        result = classify(tree, test)
        print "result:", result

    codegen = Codegen(output)
    codegen.gencode(tree)

if __name__=='__main__':
    training = 'restaurant.csv'
    testing = 'test.csv'
    output = sys.stdout

    if len(sys.argv) > 1:
        training = sys.argv[1]
    if len(sys.argv) > 2:
        testing = sys.argv[2]
    if len(sys.argv) > 3:
        outfile = sys.argv[3]
        output = open(sys.argv[3], 'w')

    main([training, testing, output])
    output.close()

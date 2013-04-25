#!/usr/bin/python

import sys

attrs = ['Alt', 'Bar', 'Fri', 'Hun', 'Pat',
         'Price', 'Rain', 'Res', 'Type', 'Est']

domains = {}

class DecisionTree:
    def __init__(self, root):
        self.root = root
        self.children = []

    def add_child(self, child):
        self.children.append(child)

def plurality(examples):
    pass

def all_same(examples):
    pass

def importance(attr, examples):
    pass

def select(examples, attr, val):
    pass

def decision_tree_learning(examples, attributes, parent_examples):
    if not examples: return plurality(examples)
    if all_same(examples): return 
    if not attributes: return plurality(examples)

    gains = [importance(attr) for attr in attributes]
    pivot = max(gains)
    
    tree = Decision(pivot)
    for v in domains[pivot]:
        exs = select(examples, pivot, val)
        subtree = decision_tree_learning(exs, attributes - pivot, examples)
        tree.add_child(subtree)
    return tree
        

def read_data(datafile):
    examples = []
    lines = open(datafile).readAll().split('\n')
    for line in lines:
        example = line.split(',')
        examples.append(example)
    return examples
        
def main(args):
    pass

if __name__=='__main__':
    main(sys.argv[1:])

"""
15 Puzzles

"""

import sys
from multiprocessing import Process
import string
import heapq
import itertools
from copy import copy
from random import randint
from random import shuffle
from collections import deque

#------------------------------------------------
# Board settings

directions = ['N','S','W','E']
maxnodes = 50000
maxdepth = 12
maxtraindep = 20

N = 15
board_size = 4

loops = list(itertools.product(range(board_size), range(board_size)))

#------------------------------------------------
# Class definitions

class Solution:
    def __init__(self, algo, steps, stats):
        self.algo = algo
        self.steps = steps
        self.stats = stats

    def __str__(self):
        outs = self.algo + ":\n"
        outs += "("
        outs += "("
        if self.steps:            
            outs += ("(" + string.join(self.steps, " ") + ") ")
            outs += str(len(self.steps))
        else:
            outs += "NIL NIL"
        outs += ") "
        outs += string.join(map(str, self.stats), " ")
        outs += ")"
        return outs

class State:
    offsets = { "N": (-1,  0),
                "S": ( 1,  0),
                "W": ( 0, -1),
                "E": ( 0,  1)}

    def at(self,r,c):
        return (self.coding>>((r*4+c)*4)) & N

    def set(self, r, c, v):
        self.coding |= (v<<((r*4+c)*4))

    def clear(self,r,c):
        self.coding &= ~(N<<((r*4+c)*4))

    # return the coding with all tiles not in the pattern masked
    def mask(self, pattern):
        masked = self.coding
        for r,c in loops:
            if self.at(r,c) not in pattern:
                masked &= ~(N<<((r*4+c)*4))
        return masked
        
    def __init__(self, coding, blank, parent=None, action=None, cost=0, fcost=0):
        self.coding = coding
        self.blank = blank
        self.parent = parent
        self.action = action
        self.cost = cost
            
    def swap(self, row, col, ro, co):
        self.blank = (row+ro,col+co)
        self.set(row,col,self.at(row+ro,col+co))
        self.clear(row+ro,col+co)

    def move(self, direction, cost=1):
        row, col = self.blank
        ro, co = State.offsets[direction]
        if row+ro >= 0 and row+ro < board_size and col+co >= 0 and col+co < board_size:       
            child = State(self.coding,self.blank,self,direction,self.cost+cost)
            child.swap(row,col,ro,co)
            return child
        else:
            return None      

    def trace(self):
        steps = []
        node = self
        while node.parent:
            steps.append(node.action)
            node = node.parent
        return steps
            
    def win(self, goal):        
        return self.coding == goal.coding

    def __hash__(self):
        return hash(self.coding) # hash(blank), depends on which one is faster
        
    def __eq__(self, other):
        return self.blank==other.blank and self.coding==other.coding
    
    def __str__(self):
        string = "-------------------\n"
        for r in range(board_size):
            for c in range(board_size):
                string += ("%4d" % self.at(r, c))
            string += "\n"
        string += "-------------------\n"
        return string

#------------------------
# puzzle generator

def generate_random():
    x = range(0,N+1)
    shuffle(x)
    
    pos = x.index(0)
    row = pos / board_size
    col = pos % board_size

    game = State(0L,(row,col))
    for n, i in zip(x, range(N+1)):
        r = i / board_size
        c = i - r * board_size
        game.set(r,c,n)
    game.blank = (row,col)
    return game

def generate(goal, nsteps):
    def random_dir():
        return directions[randint(0, len(directions)-1)]

    current = goal
    visited = set()
    visited.add(goal.coding)
    step = 0
    while step < nsteps:
        d = random_dir()
        next = current.move(d)
        if next and next.coding not in visited:
            visited.add(next.coding)
            current = next
            step += 1
    current.parent = current.action = None
    current.cost = current.fcost = 0
    return current

#------------------------
# dfs

def do_dfs(start, goal, maxd):
    total = repeats = 0
    visited = set()
    frontier = [] # use list to simulate the stack
    #    explored = []
    frontier.append(start)
    visited.add(start.coding)

    while frontier:
        current = frontier.pop()
        if current.win(goal):
            return (current.trace(), (total, repeats, len(frontier), len(visited)-len(frontier)-1))
        #        explored.append(current)
        
        if current.cost >= maxd: continue

        for direction in directions:
            child = current.move(direction)
            if not child: continue

            total += 1
            if child.coding not in visited:
                visited.add(child.coding)
                frontier.append(child)
            else:
                repeats += 1

    return (None, (total, repeats, len(frontier), len(visited)-len(frontier)-1))

def dfs(start, goal):
    '''DFS'''
    return do_dfs(start, goal, maxdepth)


def idfs(start, goal):
    '''IDFS'''
    def update_stats(stats, ns):
        return (stats[0]+ns[0], stats[1]+ns[1], ns[2], ns[3])
    
    depth = 0
    stats = (0,0,0,0)
    for depth in range(maxdepth):
        solution = do_dfs(start, goal, depth)
        stats = update_stats(stats, solution[1])
        if solution[0]:
            return (solution[0], stats)
    return (None, stats)

#------------------------
    
def bfs(start, goal):
    '''BFS'''
    total = repeats = 0
    frontier = deque() # FIFO queue
    #    explored = []
    visited = set()
    visited.add(start.coding)
    frontier.append(start)
    
    while frontier:
        current = frontier.popleft()

        if current.win(goal):
            return (current.trace(), (total, repeats, len(frontier), len(visited)-len(frontier)-1))
        #        explored.append(current)
        
        for direction in directions:
            child = current.move(direction)
            if not child: continue

            total += 1
            if child.coding not in visited:
                frontier.append(child)
                visited.add(child.coding)
            else:
                repeats += 1
    return (None, (total, repeats, len(frontier), len(visited)-len(frontier)-1))


#------------------------
# Be careful about the stability issue in heapq
    
def search(start, goal, costfun, hfun):
    total = repeats = 0    
    frontier = []
        #    explored = []
    visited = set()
    start.fvalue = hfun(start, goal)
    heapq.heappush(frontier, (start.fvalue, start))
    visited.add(start.coding)

    while frontier:
        f, current = heapq.heappop(frontier)
        if current.win(goal):
            return (current.trace(), (total, repeats, len(frontier), len(visited)-len(frontier)-1))
        #        explored.append(current)

        if total > maxnodes: break
        
        for direction in directions:
            child = current.move(direction, costfun(direction))
            if child is None: continue

            child.fvalue = child.cost + hfun(child, goal)                          
            total += 1
            if child.coding not in visited:
                heapq.heappush(frontier, (child.fvalue, child))
                visited.add(child.coding)
            else:
                repeats += 1    
                # Ugly update
                index = [idx for idx in range(len(frontier)) if frontier[idx][1]==child]
                if len(index) == 1 and frontier[index[0]][0] > child.fvalue:
                    frontier[index[0]] = (child.fvalue, child)
                    heapq.heapify(frontier)
                
    return (None, (total, repeats, len(frontier), len(visited)-len(frontier)-1))
    
def uniform(start, goal):
    '''Uniform'''
    def h_none(state1, state2):
        return 0
    return search(start, goal, uniform_cost, h_none)

def uniform_cost(action):
    return 1

def zero_cost(action):
    return 0
    
def h_manhattan(state1, state2):
    pos1 = [0] * 16
    pos2 = [0] * 16        
        
    for r,c in loops:
        pos1[state1.at(r,c)] = (r,c)
        pos2[state2.at(r,c)] = (r,c)

    pos1[0] = pos2[0] = (0,0)
    dist = sum([abs(x[0]-y[0])+abs(x[1]-y[1]) for x,y in zip(pos1, pos2)])
    return dist

class PatternDB:
    def __init__(self, pattern):
        self.pattern = pattern
        self.cache = {}

    def search(self, state):
        code = state.mask(self.pattern)
        return self.cache.get(code, 0)

    def add(self, state, steps):
        code = state.mask(self.pattern)
        if code not in self.cache:
            self.cache[code] = steps

            
patterns = [[1,2,3,4,5,6,7,8],[9,10,11,12,13,14,15]]            
#patterns = [[x] for x in range(1,16)] # train manhattan

class PatternState(State):
    def __init__(self, coding, blank, steps=0, parent=None, action=None, cost=0, fcost=0):
        self.steps = steps
        State.__init__(self,coding, blank, parent, action, cost, fcost)        
        
    def move(self, direction, cost=1):
        row, col = self.blank
        ro, co = State.offsets[direction]
        
        if row+ro >= 0 and row+ro < board_size and col+co >= 0 and col+co < board_size:       
            child = PatternState(self.coding,self.blank,self.steps,self,direction,self.cost+cost)
            child.swap(row,col,ro,co)
            v = child.at(row,col)
            if v != 0: child.steps += 1
            return child
        else:
            return None          

            
def train_pattern(goal, pattern):
    db = PatternDB(pattern)
    goal = PatternState(goal.coding, goal.blank)
    goal.coding = goal.mask(pattern)
    frontier = deque() # FIFO queue
    visited = set()
    visited.add((goal.coding, goal.blank))
    frontier.append(goal)
    
    while frontier:
        current = frontier.popleft()
        db.add(current, current.steps)        
        
        if current.cost > maxtraindep: break
        
        for direction in directions:
            child = current.move(direction)
            if not child: continue

            if (child.coding,child.blank) not in visited:
                frontier.append(child)
                visited.add((child.coding, child.blank))
    return db
    
pdbs = []        
def train(goal):
    for pattern in patterns:
        pdbs.append(train_pattern(goal, pattern))

import affinity

def do_parallel_train(goal, pattern, cpu):
    affinity.set_process_affinity_mask(0, cpu)
    db = train_pattern(goal, pattern)
    pdbs.append(db)
    
def parallel_train(goal):
    t1 = Process(target=do_parallel_train, args=(goal, patterns[0], 1))
    t2 = Process(target=do_parallel_train, args=(goal, patterns[1], 2))
    t1.start()
    t2.start()
    t1.join()
    t2.join()
    
        
def h_patterndb(state, goal):
    h = sum([pdb.search(state) for pdb in pdbs])
    return h
        
def h_linear_conflict(s, goal):
    # return a priority queue
    # key: num of conflict
    # value:  (tile id, <list-of-conflict-ids>)
    def nconflict(values, goal):
        both = set(values).intersection(set(goal))
        if 0 in both:
            both.remove(0)
        q = []
        for v in both:
            conflicts = []
            for v2 in both:
                if v==v2: continue
                if (values.index(v)<values.index(v2) and goal.index(v)>goal.index(v2)) or (values.index(v)>values.index(v2) and goal.index(v)<goal.index(v2)):
                    conflicts.append(v2)
            if len(conflicts) > 0:
                heapq.heappush(q, (-len(conflicts), (v, conflicts)))
        return q
                    

    def ith(state, axis, i):
        ilist = []
        for j in range(4):
            v = state.at(i,j) if axis=="row" else state.at(j,i)
            ilist.append(v)
        return ilist

    lcs = []
    for axis, i in itertools.product(["row","col"],range(4)):
        lci = 0
        conflicts = nconflict(ith(s,axis,i), ith(goal,axis,i))
        while conflicts: # conflicts is priority queue
            nc,(v,cfs) = heapq.heappop(conflicts)
            for idx in range(len(conflicts)):
                nc2,(v2,cfs2) = conflicts[idx]
                if v in cfs2:
                    cfs2.remove(v)
                    conflicts[idx] = (-len(cfs2), (v2, cfs2))
            heapq.heapify(conflicts)
            lci += 1
        lcs.append(lci)
    return 2*sum(lcs) +  h_manhattan(s, goal)

def astar_pattern(start, goal):
    '''A* - Pattern DBs'''
    return search(start, goal, uniform_cost, h_patterndb)
    
def mygreedy(start, goal):
    '''Greedy - Manhattan + Linear conflicts'''
    return search(start, goal, zero_cost, h_linear_conflict)

def myastar(start, goal):
    '''A* - Manhattan + Linear conflicts'''
    return search(start, goal, uniform_cost, h_linear_conflict)


def astar(start, goal):
    '''A* - Manhattan'''
    return search(start, goal, uniform_cost, h_manhattan)

def greedy(puzzle, goal):
    '''Greedy - Manhattan'''
    return search(puzzle, goal, zero_cost, h_manhattan)

def solve(puzzle, goal, algo):
    solution = algo(puzzle, goal)
    return Solution(algo.__doc__, solution[0], solution[1])

hard = "((1 5 3 7) (4 9 2 11) (8 13 10 14) (12 15 0 6) (3 2))"
easy = "((1 2 3 0) (4 5 6 7) (8 9 10 11) (12 13 14 15) (0 3))"
moderate = "((1 2 6 3) (4 5 10 7) (0 9 14 11) (8 12 13 15) (2 0))"
goal1 = "((0 1 2 3) (4 5 6 7) (8 9 10 11) (12 13 14 15) (0 0))"
goal2 = "((1 2 3 4) (8 7 6 5) (9 10 11 12) (0 15 14 13) (3 0))"

import re
def parse_state(statestr):
    statestr.strip('"\'')
    statestr = statestr[1:-1] # remove parenthesis
    rowsstr = re.findall('\(.*?\)', statestr)    
    blank = tuple(map(int, rowsstr[-1][1:-1].split(' ')))
    rows = [map(int, row[1:-1].split(' ')) for row in rowsstr[0:-1]]

    state = State(0L,blank)
    for r,c in loops:
        state.set(r,c,rows[r][c])
    return state
import time

if __name__ == "__main__":
    # if len(sys.argv) != 3:
    #     print "Usage: python puzzle.py <start-state> <goal-state>"
    #     sys.exit(1)
    start = parse_state(moderate)
    goal = parse_state(goal1)

    t = time.time()
    train(goal)
    print time.time() - t
    #    algos = [bfs,dfs,idfs,uniform,astar, greedy, myastar, mygreedy]
    algos = [astar, astar_pattern]
        
    solutions = [solve(start, goal, algo) for algo in algos]

    for solution in solutions:
        print solution
    

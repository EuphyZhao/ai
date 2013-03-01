from puzzle import *

difficulty = 20
rounds = 1
goal = parse_state(goal1)
#algos = [bfs,dfs,idfs,uniform,astar, greedy, myastar, mygreedy]
algos = [astar,myastar,astar_pattern]

if __name__ == "__main__":
    train(goal)

    print "Training finishes."
    
    # single round
    game = generate(goal, difficulty)
    solutions = [solve(game, goal, algo) for algo in algos]

    for solution in solutions:
        print solution

    print "----------------"
    
    # average performance
    stats = {}
    for algo in algos:
        stats[algo.__doc__] = [0,0,0,0]
        
    for i in range(rounds):
        game = generate(goal, difficulty)
        solutions = [solve(game, goal, algo) for algo in algos]
        for solution in solutions:
            stats[solution.algo] = [a+b for a,b in zip(stats[solution.algo],solution.stats)]

    for algo in stats.keys():
        stats[algo] = [x/rounds for x in stats[algo]]

    for algo, stat in stats.items():
        print algo + ":" + str(stat)

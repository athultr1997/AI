# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print "Start:", problem.getStartState()
    print "Is the start a goal?", problem.isGoalState(problem.getStartState())
    print "Start's successors:", problem.getSuccessors(problem.getStartState())
    """
    "*** YOUR CODE HERE ***"
    from game import Directions
    from util import Stack

    startState = (problem.getStartState(),Directions.STOP,0)
    print startState
    frontiers = Stack()
    frontiers.push(startState)
    exploredSet = list()
    exploredSet.append(startState[0])
    actions = list()

    while not frontiers.isEmpty():
        node = frontiers.pop()
        exploredSet.append(node)
        successors = problem.getSuccessors(node[0])

        for successor in successors:
            if successor[0] not in list(zip(*exploredSet)[0]):
                if problem.isGoalState(successor[0]):
                    actions.append(successor[1])
                    parent = [x for x in problem.getSuccessors(successor[0]) if x[1]==Directions.REVERSE[successor[1]]][0]
                    while parent[0] != startState[0]:
                        actions.insert(0,[x for x in exploredSet if x[0]==parent[0]][0][1])
                        parent = [x for x in problem.getSuccessors(parent[0]) if x[1]==Directions.REVERSE[actions[0]]][0]
                    return actions
                elif not frontiers.isEmpty():
                    if successor[0] not in list(zip(*frontiers.list)[0]):
                        frontiers.push(successor)
                else:
                    frontiers.push(successor)

    return [Directions.STOP]

def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""
    "*** YOUR CODE HERE ***"
    from game import Directions
    from util import Queue

    startState = (problem.getStartState(),Directions.STOP,0)
    frontiers = Queue()
    frontiers.push(startState)
    exploredSet = list()
    exploredSet.append(startState[0])
    actions = list()

    while not frontiers.isEmpty():
        node = frontiers.pop()
        exploredSet.append(node)
        successors = problem.getSuccessors(node[0])

        for successor in successors:
            if successor[0] not in list(zip(*exploredSet)[0]):#checking if the successors have already been explored (add frontiers)
                if problem.isGoalState(successor[0]):                
                    actions.append(successor[1])
                    parent = [x for x in problem.getSuccessors(successor[0]) if x[1]==Directions.REVERSE[successor[1]]][0]
                    while parent[0] != startState[0]:
                        actions.insert(0,[x for x in exploredSet if x[0]==parent[0]][0][1])
                        parent = [x for x in problem.getSuccessors(parent[0]) if x[1]==Directions.REVERSE[actions[0]]][0]
                    return actions
                elif not frontiers.isEmpty():
                    if successor[0] not in list(zip(*frontiers.list)[0]):
                        frontiers.push(successor)    
                else:
                    frontiers.push(successor)

    return [Directions.STOP]

def uniformCostSearch(problem):
    """Search the node of least total cost first."""
    "*** YOUR CODE HERE ***"
    from util import PriorityQueue
    from game import Directions

    exploredSet = list()
    actions = list()
    startNode = (problem.getStartState(),Directions.STOP,0)
    frontiers = PriorityQueue()
    frontiers.push(startNode,0)

    while not frontiers.isEmpty():
        node = frontiers.pop()
        if problem.isGoalState(node[0]):

            return actions
        else:
            exploredSet.append(node)
            successors = problem.getSuccessors(node[0])
            for successor in successors:
                if successor not in list(zip(*exploredSet)[0]):
                    frontier.push(successor)
                    









    return [Directions.STOP]

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch

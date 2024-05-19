# Unblock Me Solver Algorithm

## Introduction

"Unblock Me" is a popular sliding block puzzle game where the objective is to move a red block out of the board by sliding other blocks out of the way. Blocks can only move in the direction they are oriented (either horizontally or vertically), and they cannot overlap with other blocks. The goal is to find the shortest sequence of moves that will free the red block.

This document explains the algorithm used to solve the "Unblock Me" puzzle programmatically.

## Algorithm Overview

The solver algorithm employs a Breadth-First Search (BFS) strategy to explore all possible states of the board. BFS is suitable for this problem because it guarantees finding the shortest path to the solution. The algorithm can be broken down into the following steps:

1. **Initialization**: 
   - Define the board state and the goal state.
   - Create a queue to hold the board states to be explored.
   - Use a set to keep track of visited states to avoid redundant work.

2. **State Representation**:
   - Represent the board as a grid where each block has a specific orientation and position.
   - Encode the state of the board in a format that can be easily compared and stored.

3. **Generating Moves**:
   - For each block, generate all possible moves by sliding it in its allowed direction until it hits another block or the edge of the board.
   - Create new board states resulting from these moves.

4. **BFS Exploration**:
   - Dequeue a state from the queue.
   - Check if it matches the goal state.
   - If not, generate all possible next states and enqueue them if they haven't been visited.

5. **Solution Extraction**:
   - Once the goal state is reached, backtrack to extract the sequence of moves that led to the solution.

## Detailed Steps

### Initialization

- **Board State**: Represent the board as a two-dimensional array where each element corresponds to a cell that can either be empty or occupied by a part of a block.
- **Goal State**: Define the goal state as the state where the red block has reached the exit point on the board.
- **Queue**: Use a queue to manage the states to be explored in BFS. Each element in the queue is a board state.
- **Visited Set**: Maintain a set of visited states to avoid revisiting and processing the same state multiple times.

### State Representation

- Each block is defined by its position, length, and orientation (horizontal or vertical).
- Encode the entire board state as a string or tuple to uniquely identify it. This encoding should include the positions and orientations of all blocks.

### Generating Moves

- For each block, determine all valid moves:
  - **Horizontal Blocks**: Can move left or right if there is enough space.
  - **Vertical Blocks**: Can move up or down if there is enough space.
- Apply each move to generate a new board state.
- Ensure that the new board state is valid (blocks do not overlap and remain within the board boundaries).

### BFS Exploration

- Start with the initial board state and enqueue it.
- Dequeue the first state from the queue.
- Check if the dequeued state matches the goal state. If yes, the solution is found.
- If not, generate all possible moves from the current state and enqueue the resulting states if they haven't been visited.

### Solution Extraction

- Once the goal state is reached, trace back the sequence of moves from the initial state to the goal state.
- This can be done by maintaining a parent pointer for each state, pointing to the state from which it was generated.
- Extract the sequence of moves by backtracking from the goal state to the initial state using these parent pointers.

## Conclusion

This algorithm effectively solves the "Unblock Me" puzzle by exploring all possible moves using BFS, ensuring that the shortest path to the solution is found. The modular structure allows for easy modifications and extensions, such as handling different board sizes or adding more complex heuristics to improve efficiency.

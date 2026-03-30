# Reversi AI

A terminal-based Reversi (Othello) game with an AI opponent, written in C.

The AI uses iterative-deepening alpha-beta pruning with positional weight heuristics and a configurable time limit to search for the best move.

## Building

```
make
```

## Usage

```
./reversi
```

You'll be prompted to choose whether the computer plays Black or White. Moves are entered as two-character row-column pairs (e.g. `cd` for row c, column d), using letters `a` through `h`.

## Project Structure

| File | Description |
|------|-------------|
| `main.c` | Game loop and human input handling |
| `board.c` | Board initialization, move validation, and display |
| `ai.c` | AI engine (alpha-beta search with positional evaluation) |
| `reversi.h` | Shared type definitions and function declarations |
| `Makefile` | Build configuration |

## Cleaning

```
make clean
```

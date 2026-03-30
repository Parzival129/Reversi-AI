# Reversi AI

A competitive Reversi (Othello) engine written in C. The AI combines **iterative-deepening search**, **alpha-beta pruning**, and a **multi-component evaluation function** to play at a strong level under a strict per-move time budget.

## How It Works

### Search: Minimax with Alpha-Beta Pruning

The core of the AI is a [minimax](https://en.wikipedia.org/wiki/Minimax) game tree search enhanced with [alpha-beta pruning](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning) to eliminate branches that cannot influence the final decision.

Minimax models the game as a two-player zero-sum problem. At each node in the search tree the algorithm alternates between:

- **Maximizing** the evaluation when it is the AI's turn
- **Minimizing** the evaluation when it is the opponent's turn

Alpha-beta pruning maintains a window **[α, β]** during recursion. When the search proves that a subtree's score falls outside this window, the entire subtree is pruned:

```
if α ≥ β then prune (cutoff)
```

In the best case this reduces the effective branching factor from **b** to **√b**, allowing the search to reach roughly **twice the depth** in the same amount of time compared to plain minimax.

### Iterative Deepening with Time Control

Rather than searching to a fixed depth, the engine uses **iterative deepening** — it runs a full alpha-beta search at depth 1, then depth 2, and so on, up to a maximum of 50 plies. Search is governed by a **0.97-second CPU time budget** measured via `getrusage(RUSAGE_SELF)`.

```
for depth = 1 to 50:
    if elapsed CPU time > 0.97s: break
    run full alpha-beta search at this depth
    if search completed without timeout: accept result
```

This guarantees the AI always has a valid move ready (from the last fully completed depth) while using as much of the time budget as possible. In practice the engine typically reaches depth 8–12 in the midgame and can solve endgame positions to completion.

### Evaluation Function

The static evaluation function scores a board position from the AI's perspective using three weighted components:

#### 1. Positional Weights (Strategic Value)

Each square on the board has a fixed strategic weight reflecting its long-term value in Reversi theory:

```
 120  -20   20    5    5   20  -20  120
 -20  -40   -5   -5   -5   -5  -40  -20
  20   -5   15    3    3   15   -5   20
   5   -5    3    3    3    3   -5    5
   5   -5    3    3    3    3   -5    5
  20   -5   15    3    3   15   -5   20
 -20  -40   -5   -5   -5   -5  -40  -20
 120  -20   20    5    5   20  -20  120
```

The design rationale:

| Region | Weight | Reasoning |
|--------|--------|-----------|
| **Corners** | **+120** | Once captured, corners can never be flipped. They anchor stable disc chains along edges and diagonals. |
| **C-squares** (adjacent to corners diagonally) | **-40** | Playing here before the adjacent corner is taken gives the opponent easy access to the corner. |
| **X-squares** (edge cells adjacent to corners) | **-20** | Similar risk to C-squares — concedes corner access. |
| **Edges** (non-corner, non-adjacent) | **+15 to +20** | Edge discs are harder to flip and help build stable walls. |
| **Center** | **+3 to +5** | Modest value; central discs are easily flipped but provide some mobility. |

The positional score is computed as:

```
S_pos = Σ w(r,c) · 𝟙[cell is AI's] − Σ w(r,c) · 𝟙[cell is opponent's]
```

#### 2. Mobility (Frontier Control)

Mobility measures how many legal moves each side has. Higher mobility means more options and more control over the game's tempo:

```
S_mob = 10 × (moves_AI − moves_opponent)
```

This encourages the AI to maximize its own choices while restricting the opponent's — a core principle of strong Reversi play.

#### 3. Late-Game Piece Differential

When the board is more than 75% full (>48 occupied squares), raw disc count becomes increasingly important since the game is approaching final scoring:

```
S_disc = 5 × (pieces_AI − pieces_opponent)    [if board > 75% full]
```

#### Combined Score

```
eval(board) = S_pos + S_mob + S_disc
```

### Move Ordering

Before searching, candidate moves are **sorted by positional weight** (descending). This is a critical optimization for alpha-beta — by exploring the most promising moves first, more cutoffs occur earlier in the tree, dramatically improving effective search depth.

### Corner Proximity Penalty

After the full search, moves adjacent to an **uncaptured corner** (the four diagonal-adjacent "X-squares") receive an additional **-60 penalty**. This discourages the AI from playing near corners it hasn't secured, since doing so often hands the corner to the opponent on the next move.

```
if move is diagonal to a corner AND that corner is empty:
    score -= 60
```

## Building

```
make
```

Requires `gcc` (or any C99-compatible compiler). No external libraries needed.

## Usage

```
./reversi
```

You'll be prompted to choose whether the computer plays Black (first move) or White. Moves are entered as two-character row-column coordinates using letters `a`–`h` (e.g., `cd` = row c, column d).

## Project Structure

| File | Description |
|------|-------------|
| `main.c` | Game loop, input parsing, and turn management |
| `board.c` | Board state, move validation (8-directional ray scanning), and disc flipping |
| `ai.c` | Search engine: iterative deepening, alpha-beta, evaluation, and move ordering |
| `reversi.h` | Shared types (`Board`, constants) and function declarations |
| `Makefile` | Build configuration (`gcc -Wall -Wextra -O2`) |

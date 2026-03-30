#ifndef REVERSI_H
#define REVERSI_H

#include <stdbool.h>

#define BOARD_SIZE 8
#define MAX_MOVES (BOARD_SIZE * BOARD_SIZE)

typedef char Board[BOARD_SIZE][BOARD_SIZE];

// Utility
char opponentOf(char player);
bool positionInBounds(int row, int col);

// Board operations
void initializeBoard(Board board);
void printBoard(Board board);
bool checkLegalInDirection(Board board, int row, int col, char colour, int deltaRow, int deltaCol);
void applyMove(Board board, int row, int col, char player);
void tryMove(Board src, Board copy, int row, int col, char player);
int countMoves(Board board, char player);
int availableMoves(Board board, char player, int moves[][2]);
char checkWin(Board board);

// AI
void makeMove(Board board, char computer, int *row, int *col);

// Human input
bool AcceptMove(Board board, char player);

#endif

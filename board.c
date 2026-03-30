#include "reversi.h"
#include <stdio.h>
#include <string.h>

char opponentOf(char player)
{
    return (player == 'B') ? 'W' : 'B';
}

bool positionInBounds(int row, int col)
{
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

bool checkLegalInDirection(Board board, int row, int col, char colour, int deltaRow, int deltaCol)
{
    int r = row + deltaRow, c = col + deltaCol;
    if (!positionInBounds(r, c) || board[r][c] == colour || board[r][c] == 'U')
        return false;
    while (positionInBounds(r, c)) {
        if (board[r][c] == 'U')
            return false;
        if (board[r][c] == colour)
            return true;
        r += deltaRow;
        c += deltaCol;
    }
    return false;
}

void applyMove(Board board, int row, int col, char player)
{
    board[row][col] = player;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0)
                continue;
            if (checkLegalInDirection(board, row, col, player, dr, dc)) {
                int r = row + dr, c = col + dc;
                while (board[r][c] != player) {
                    board[r][c] = player;
                    r += dr;
                    c += dc;
                }
            }
        }
    }
}

void tryMove(Board src, Board copy, int row, int col, char player)
{
    memcpy(copy, src, sizeof(Board));
    applyMove(copy, row, col, player);
}

int countMoves(Board board, char player)
{
    int count = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] != 'U')
                continue;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0)
                        continue;
                    if (checkLegalInDirection(board, r, c, player, dr, dc)) {
                        count++;
                        goto next_cell;
                    }
                }
            }
            next_cell:;
        }
    }
    return count;
}

int availableMoves(Board board, char player, int moves[][2])
{
    int total = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] != 'U')
                continue;
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0)
                        continue;
                    if (checkLegalInDirection(board, r, c, player, dr, dc)) {
                        moves[total][0] = r;
                        moves[total][1] = c;
                        total++;
                        goto next_move;
                    }
                }
            }
            next_move:;
        }
    }
    return total;
}

void initializeBoard(Board board)
{
    memset(board, 'U', sizeof(Board));
    int mid = BOARD_SIZE / 2;
    board[mid - 1][mid]     = 'B';
    board[mid][mid - 1]     = 'B';
    board[mid - 1][mid - 1] = 'W';
    board[mid][mid]         = 'W';
}

void printBoard(Board board)
{
    printf("  ");
    for (int c = 0; c < BOARD_SIZE; c++)
        printf("%c", 'a' + c);
    printf("\n");
    for (int r = 0; r < BOARD_SIZE; r++) {
        printf("%c ", 'a' + r);
        for (int c = 0; c < BOARD_SIZE; c++)
            printf("%c", board[r][c]);
        printf("\n");
    }
}

char checkWin(Board board)
{
    int b = 0, w = 0;
    bool hasEmpty = false;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] == 'B') b++;
            else if (board[r][c] == 'W') w++;
            else hasEmpty = true;
        }
    }

    if (hasEmpty && (countMoves(board, 'B') > 0 || countMoves(board, 'W') > 0))
        return 'N';

    if (b > w) return 'B';
    if (w > b) return 'W';
    return 'T';
}

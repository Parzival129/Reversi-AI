#include "reversi.h"
#include <stdio.h>

bool AcceptMove(Board board, char player)
{
    printf("Enter move for colour %c (RowCol): ", player);
    char input[4];
    if (scanf("%2s", input) != 1)
        return false;

    int row = input[0] - 'a';
    int col = input[1] - 'a';

    bool legal = false;
    if (positionInBounds(row, col) && board[row][col] == 'U') {
        for (int dr = -1; dr <= 1 && !legal; dr++)
            for (int dc = -1; dc <= 1 && !legal; dc++) {
                if (dr == 0 && dc == 0)
                    continue;
                if (checkLegalInDirection(board, row, col, player, dr, dc))
                    legal = true;
            }
    }

    if (!legal) {
        printf("Invalid move.\n");
        return false;
    }

    applyMove(board, row, col, player);
    return true;
}

static void printResult(char winner)
{
    if (winner == 'T')
        printf("Tie game.\n");
    else
        printf("%c player wins.\n", winner);
}

int main(void)
{
    char computer;
    printf("Computer plays (B/W): ");
    scanf(" %c", &computer);
    char human = opponentOf(computer);

    Board board;
    initializeBoard(board);
    printBoard(board);

    if (human == 'B') {
        while (!AcceptMove(board, human))
            ;
        printBoard(board);
    }

    while (1) {
        bool computerPassed = false;
        bool humanPassed = false;

        int r, c;
        makeMove(board, computer, &r, &c);
        if (r >= 0) {
            printf("Computer places %c at %c%c.\n", computer, 'a' + r, 'a' + c);
            applyMove(board, r, c, computer);
            printBoard(board);
            char winner = checkWin(board);
            if (winner != 'N') {
                printResult(winner);
                return 0;
            }
        } else {
            printf("%c player has no valid move.\n", computer);
            computerPassed = true;
        }

        int moves[MAX_MOVES][2];
        if (availableMoves(board, human, moves) > 0) {
            while (!AcceptMove(board, human))
                ;
            printBoard(board);
            char winner = checkWin(board);
            if (winner != 'N') {
                printResult(winner);
                return 0;
            }
        } else {
            printf("%c player has no valid move.\n", human);
            humanPassed = true;
        }

        if (computerPassed && humanPassed) {
            printResult(checkWin(board));
            return 0;
        }
    }
}

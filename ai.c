#include "reversi.h"
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

#define DEPTH_LIMIT 50
#define TIME_LIMIT 0.97
#define INF 100000
#define CORNER_PENALTY 60

static double searchStart;
static bool searchTimedOut;

static const int POSITION_WEIGHTS[BOARD_SIZE][BOARD_SIZE] = {
    { 120, -20,  20,   5,   5,  20, -20, 120 },
    { -20, -40,  -5,  -5,  -5,  -5, -40, -20 },
    {  20,  -5,  15,   3,   3,  15,  -5,  20 },
    {   5,  -5,   3,   3,   3,   3,  -5,   5 },
    {   5,  -5,   3,   3,   3,   3,  -5,   5 },
    {  20,  -5,  15,   3,   3,  15,  -5,  20 },
    { -20, -40,  -5,  -5,  -5,  -5, -40, -20 },
    { 120, -20,  20,   5,   5,  20, -20, 120 },
};

static double elapsedTime(double since)
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    struct timeval t = usage.ru_utime;
    return (t.tv_sec + t.tv_usec / 1000000.0) - since;
}

static void sortMovesByWeight(int moves[][2], int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (POSITION_WEIGHTS[moves[j][0]][moves[j][1]] >
                POSITION_WEIGHTS[moves[i][0]][moves[i][1]]) {
                int tmp0 = moves[i][0], tmp1 = moves[i][1];
                moves[i][0] = moves[j][0];
                moves[i][1] = moves[j][1];
                moves[j][0] = tmp0;
                moves[j][1] = tmp1;
            }
        }
    }
}

static int evalBoard(Board board, char player)
{
    char opp = opponentOf(player);
    int score = 0, myPieces = 0, oppPieces = 0, total = 0;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] == player) {
                score += POSITION_WEIGHTS[r][c];
                myPieces++;
            } else if (board[r][c] == opp) {
                score -= POSITION_WEIGHTS[r][c];
                oppPieces++;
            }
            if (board[r][c] != 'U')
                total++;
        }
    }

    score += (countMoves(board, player) - countMoves(board, opp)) * 10;

    if (total > (BOARD_SIZE * BOARD_SIZE * 3) / 4)
        score += (myPieces - oppPieces) * 5;

    return score;
}

static int lookAhead(Board board, char turn, char computer, int depth, int alpha, int beta)
{
    if (!searchTimedOut && elapsedTime(searchStart) > TIME_LIMIT)
        searchTimedOut = true;
    if (searchTimedOut)
        return 0;
    if (depth == 0)
        return evalBoard(board, computer);

    char opp = opponentOf(turn);
    int moves[MAX_MOVES][2];
    int numMoves = availableMoves(board, turn, moves);

    if (numMoves == 0)
        return lookAhead(board, opp, computer, depth - 1, alpha, beta);

    sortMovesByWeight(moves, numMoves);

    bool maximizing = (turn == computer);
    int best = maximizing ? -INF : INF;

    for (int i = 0; i < numMoves; i++) {
        Board tmp;
        tryMove(board, tmp, moves[i][0], moves[i][1], turn);
        int score = lookAhead(tmp, opp, computer, depth - 1, alpha, beta);

        if (maximizing) {
            if (score > best) best = score;
            if (score > alpha) alpha = score;
        } else {
            if (score < best) best = score;
            if (score < beta) beta = score;
        }
        if (alpha >= beta)
            break;
    }
    return best;
}

void makeMove(Board board, char computer, int *outRow, int *outCol)
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    struct timeval t = usage.ru_utime;
    searchStart = t.tv_sec + t.tv_usec / 1000000.0;
    searchTimedOut = false;

    char opp = opponentOf(computer);
    int moves[MAX_MOVES][2];
    int numMoves = availableMoves(board, computer, moves);

    if (numMoves == 0) {
        *outRow = -1;
        *outCol = -1;
        return;
    }

    *outRow = moves[0][0];
    *outCol = moves[0][1];

    for (int depth = 1; depth <= DEPTH_LIMIT; depth++) {
        if (depth > 1 && elapsedTime(searchStart) > TIME_LIMIT)
            break;

        searchTimedOut = false;

        int bestScore = -INF;
        int bestRow = moves[0][0];
        int bestCol = moves[0][1];
        bool completed = true;

        for (int i = 0; i < numMoves; i++) {
            if (searchTimedOut) {
                completed = false;
                break;
            }

            Board tmp;
            tryMove(board, tmp, moves[i][0], moves[i][1], computer);
            int score = lookAhead(tmp, opp, computer, depth, -INF, INF);

            int r = moves[i][0], c = moves[i][1];
            if (r == 1 && c == 1 && board[0][0] == 'U')                         score -= CORNER_PENALTY;
            if (r == 1 && c == BOARD_SIZE - 2 && board[0][BOARD_SIZE - 1] == 'U') score -= CORNER_PENALTY;
            if (r == BOARD_SIZE - 2 && c == 1 && board[BOARD_SIZE - 1][0] == 'U') score -= CORNER_PENALTY;
            if (r == BOARD_SIZE - 2 && c == BOARD_SIZE - 2 && board[BOARD_SIZE - 1][BOARD_SIZE - 1] == 'U')
                score -= CORNER_PENALTY;

            if (score > bestScore) {
                bestScore = score;
                bestRow = r;
                bestCol = c;
            }
        }

        if (completed) {
            *outRow = bestRow;
            *outCol = bestCol;
        } else {
            break;
        }
    }
}


/******************************************************************************

                      _______   _ _          ______
                     |__   __| (_) |        |____  |
                        | |_ __ _| | _____      / /
                        | | '__| | |/ / _ \    / /
                        | | |  | |   <  __/   / /
                        |_|_|  |_|_|\_\___|  /_/


  Content:  A set of functions for generating Trike 7 problems.
  Source:   <https://github.com/CarlosLunaMota/Trike>
  Author:   Carlos Luna-Mota
  Version:  20200815

  =============================================================================

  Trike 7 board: Stored as a 1D array of 29 uint_t (board[0] = pawn index)
                                         
                       1                 Neighborhoods
                     2   3               -------------
                   4   5   6                N6  N1
                 7   8   9  10               \ /
              11  12  13  14  15         N5--N0--N2
            16  17  18  19  20  21          / \
          22  23  24  25  26  27  28      N4  N3

  =============================================================================

  Trike 7 hash: Stored as a single 64 bits unsigned integer

    [63, 62, 61, ... , 38, 37, 36, 35, 34, 33, ... , 10, 09, 08, 07, ..., 00]
    [<----- Player 2 pieces ---->, <----- Player 1 pieces ---->, <- Value ->]

    * Hash[0..7] = value of the position (see definition below)
    * Hash[ 7+i] = 1   <=>   Player 1 has a piece in position i (with 0<i<=28)
    * Hash[35+i] = 1   <=>   Player 2 has a piece in position i (with 0<i<=28)
    * Pawn position is stored by setting its bit to 1 for both players.
      You can deduce the real pawn color by counting the number of pieces.

  =============================================================================

  Trike 7 value: Stored as a single 8 bits unsigned integer

    [07, 06, 05, 04, 03, 02, 01, 00]
    [<-----Height-----> Uniq Winner]

    * Value[0]    = 1   <=>   We know that Player 1 wins this position.
    * Value[1]    = 1   <=>   We know that Player 2 wins this position.
    * Value[2]    = 1   <=>   If there is exactly one winning move.
    * Value[3..7] = The number of remaining turns with perfect play.

 *****************************************************************************/



#include "CLM_LIBS.h"

typedef uint_least8_t uint_t;   /*  8 bits unsigned integer */
typedef uint_fast64_t hash_t;   /* 64 bits unsigned integer */


/*** PARAMETERS & BOARD TOPOLOGY *********************************************/

#define NUM_TRIALS 1000 /* Number of randomly generated boards to solve */
#define SEED_PLIES   10 /* Number of pieces in board before solving it  */

#define MIN_REGION   14 /* Minimum required size of the empty region    */
#define MAX_PLIES    28 /* Maximum allowed  length for the problems     */
#define MIN_PLIES     8 /* Minimum required length for the problems     */
#define MIN_MOVES     8 /* Minimum required number of legal moves       */
#define MIN_REPLIES   4 /* Minimum required number of legal replies     */

/* `N[direction][index]` is the `neighbor` of `index` in that `direction` */
const uint_t N[7][29] = {{ 0,               1,
                                          2,  3,
                                        4,  5,  6,
                                      7,  8,  9, 10,
                                   11, 12, 13, 14, 15,
                                 16, 17, 18, 19, 20, 21,
                               22, 23, 24, 25, 26, 27, 28},
                               
                         { 0,               0,
                                          1,  0,
                                        2,  3,  0,
                                      4,  5,  6,  0,
                                    7,  8,  9, 10,  0,
                                 11, 12, 13, 14, 15,  0,
                               16, 17, 18, 19, 20, 21,  0},
                               
                         { 0,               0,
                                          3,  0,
                                        5,  6,  0,
                                      8,  9, 10,  0,
                                   12, 13, 14, 15,  0,
                                 17, 18, 19, 20, 21,  0,
                               23, 24, 25, 26, 27, 28,  0},
                               
                         { 0,               3,
                                          5,  6,
                                        8,  9, 10,
                                     12, 13, 14, 15,
                                   17, 18, 19, 20, 21,
                                 23, 24, 25, 26, 27, 28,
                                0,  0,  0,  0,  0,  0,  0},
                                
                         { 0,               2,
                                          4,  5,
                                        7,  8,  9,
                                     11, 12, 13, 14,
                                   16, 17, 18, 19, 20,
                                 22, 23, 24, 25, 26, 27,
                                0,  0,  0,  0,  0,  0,  0},
                                
                         { 0,               0,
                                          0,  2,
                                        0,  4,  5,
                                      0,  7,  8,  9,
                                    0, 11, 12, 13, 14,
                                  0, 16, 17, 18, 19, 20,
                                0, 22, 23, 24, 25, 26, 27},
                                
                         { 0,               0,
                                          0,  1,
                                        0,  2,  3,
                                      0,  4,  5,  6,
                                    0,  7,  8,  9, 10,
                                  0, 11, 12, 13, 14, 15,
                                0, 16, 17, 18, 19, 20, 21}};

/* Hashes a `board` and a `value` into a 64 bit unsigned integer */
hash_t hash(uint_t *board, uint_t value) {

    /* Encode the value */
    hash_t h = ((hash_t) value);

    /* Encode the pawn position */
    h |= ((hash_t) 1) << ( 7 + board[0]);
    h |= ((hash_t) 1) << (35 + board[0]);

    /* Encode the rest of the pieces */
    for (uint_t i = 1; i <= 28; i++) {
        if      (board[i] == 1) { h |= ((hash_t) 1) << ( 7 + i); }
        else if (board[i] == 2) { h |= ((hash_t) 1) << (35 + i); }
    }

    /* Return the hash */
    return h;
}

/* Undoes the hash operation and returns the `value` */
uint_t unhash(uint_t *board, hash_t h) {

    hash_t first  = ((hash_t) 1) <<  8;
    hash_t second = ((hash_t) 1) << 36;
    uint_t pieces = 0;

    /* Decode the value */
    uint_t value  = (uint_t) (h & 255);

    /* Decode the pieces */
    board[0] = 0;
    for (uint_t i = 1; i <= 28; i++, first <<= 1, second <<= 1) {
        if ((h & first) && (h & second)) { board[0] = i; pieces++; }
        else if (h &  first)             { board[i] = 1; pieces++; }
        else if (h & second)             { board[i] = 2; pieces++; }
        else                             { board[i] = 0;           }
    }

    /* Decode the pawn position */
    if (board[0]) { board[board[0]] = 2 - (pieces & 1); }

    /* Return the value */
    return value;
}

/* The size of the connected component `cell` */
uint_t component_size(uint_t *board, uint_t cell) {

    uint_t Q[29], next, size;       /* Queue */
    uint_t i, C[29];                /* candidates */

    /* Particular case: */
    if (cell == 0) { return 0; }

    /* Initialize candidates */
    for (i = 1; i <= 28; i++) { C[i] = (board[cell] == board[i]) ? 1 : 0; }
    C[cell] = C[0] = 0;

    /* Breath first search */
    next = size = 0;
    Q[size++] = cell;
    while (next < size) {
        i = Q[next++];  
        if (C[N[1][i]]) { C[N[1][i]] = 0; Q[size++] = N[1][i]; }
        if (C[N[2][i]]) { C[N[2][i]] = 0; Q[size++] = N[2][i]; }
        if (C[N[3][i]]) { C[N[3][i]] = 0; Q[size++] = N[3][i]; }
        if (C[N[4][i]]) { C[N[4][i]] = 0; Q[size++] = N[4][i]; }
        if (C[N[5][i]]) { C[N[5][i]] = 0; Q[size++] = N[5][i]; }
        if (C[N[6][i]]) { C[N[6][i]] = 0; Q[size++] = N[6][i]; }
    }
        
    return size;
}



/*** INTERFACE ***************************************************************/

/* Prints a `(board), pawn, win_move, height` line in the standar output */
void write(uint_t *board, uint_t win_move, uint_t height) {

    int row[29];
    for (int i = 1; i <= 28; i++) { row[i] = board[i]; }
    row[board[0]] |= 4;
    row[win_move] |= 4;
    
    printf("# %02d-", (int) height);
    for (int i = 1; i <= 28; i++) { printf("%d", row[i]); }
    printf("\n");
}

/* Draws the board on the screen, highlighting the pawn and the winning move */
void draw(uint_t *board, uint_t win_move, uint_t height) {

    char C[29] = {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
                  '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
                  '.', '.', '.', '.', '.', '.', '.', '.', '.'};

    C[win_move] = ':';
    for (uint_t i = 1; i <= 28; i++) {
        if (board[i] == 1) { C[i] = 'x'; }
        if (board[i] == 2) { C[i] = 'o'; }
    }
    if (board[board[0]] == 1) { C[board[0]] = 'X'; C[0] = 'O'; }
    if (board[board[0]] == 2) { C[board[0]] = 'O'; C[0] = 'X'; }

    printf("\n Player %c plays and wins in %d:\n\n", C[0], (int)height);
    printf("        %c\n",                         C[ 1]);
    printf("       %c %c\n",                     C[ 2],C[ 3]);
    printf("      %c %c %c\n",                 C[ 4],C[ 5],C[ 6]);
    printf("     %c %c %c %c\n",             C[ 7],C[ 8],C[ 9],C[10]);
    printf("    %c %c %c %c %c\n",         C[11],C[12],C[13],C[14],C[15]);
    printf("   %c %c %c %c %c %c\n",     C[16],C[17],C[18],C[19],C[20],C[21]);
    printf("  %c %c %c %c %c %c %c\n", C[22],C[23],C[24],C[25],C[26],C[27],C[28]);
}



/*** CACHE *******************************************************************/

/* This dictionary is just a thin wrapper aroung my generic splay-tree code */
#define LESS(h1, h2) (((h1)>>8) < ((h2)>>8))
IMPORT_CLM_STREE(hash_t, LESS, )
typedef stree cache_t;

/* Wipes out the whole `cache` and return its `size` */
size_t clear(cache_t *cache) {
    size_t size = 0;
    while (*cache) { stree_pop(cache); size++; }
    return size;
}

/* Puts/overwrites `data` in the `cache` */
bool put(cache_t *cache, hash_t data) {
    return stree_insert(cache, data);
}

/* Recovers the value of `data` stored in the `cache` */
uint_t get(stree *cache, hash_t data) {
    return stree_find(cache, data) ? (uint_t) (stree_root(cache) & 255) : 0;
}



/*** GAME LOGIC **************************************************************/

/* Stores all legal moves in `moves` and return how many of them are */
uint_t get_moves(uint_t *board, uint_t *moves) {

    /* Trivial case: the first move */
    if (board[0] == 0) {
        moves[0] = 1; moves[1] = 2; moves[2] = 4;  moves[3] = 5;
        moves[4] = 7; moves[5] = 8; moves[6] = 13; return 7;
    }

    /* General case: */
    uint_t i, m = 0;
    for (uint_t d = 1; d <= 6; d++) {
        i = N[d][board[0]];
        while (i && !board[i]) { moves[m++] = i; i = N[d][i]; }
    }
    return m;
}

/* Returns the `value` of an end-game position */
uint_t get_winner(uint_t *board) {

    uint_t first  = 0;
    uint_t second = 0;
    uint_t pawn   = board[0];

    if (pawn) {
        for (uint_t d = 0; d <= 6; d++) {
            if      (N[d][pawn] && board[N[d][pawn]] == 1) { first++;  }
            else if (N[d][pawn] && board[N[d][pawn]] == 2) { second++; }
        }
    }

    if      (first > second) { return 1; }
    else if (first < second) { return 2; }
    else                     { return 0; }
}

/* Returns the `value` of a mid-game position */
uint_t solve(uint_t *board, stree *cache) {

    /* Try to recover the value from the cache */
    hash_t h     = hash(board, 0);
    uint_t value =  get(cache, h);
    if (value) { return value; }

    /* Otherwise we need to compute the value */
    uint_t m, moves[12];
    uint_t num_moves = get_moves(board, moves);

    /* End-game position: return the winner */
    if (num_moves == 0) { value = get_winner(board); }

    /* Mid-game position: recourse */
    else {
        uint_t empty_cells = 0;
        uint_t replies[12], num_replies = 0;
        uint_t move_value;
        uint_t win_move   = 0;   /*  Optimal move from this position         */
        uint_t height     = 0;   /*  # of remaining plies with perfect play  */
        uint_t unique     = 0;   /*  1 <=> there is exactly one winning move */
        uint_t pawn = board[0];
        uint_t next = pawn ? 3-board[pawn] : 1;

        for (m = 0; m < num_moves; m++) {

            board[moves[m]] = next;
            board[0]        = moves[m];
            move_value      = solve(board, cache);
            board[0]        = pawn;
            board[moves[m]] = 0;

            /* If it's a winning move... */
            if ((move_value & 3) == next) {

                /* ...and it's the first one */
                if (win_move == 0) {
                    height   = ((move_value >> 3) + 1);
                    win_move = moves[m];
                    unique   = 1;
                }

                /* ...and it isn't the first one */
                else {
                    unique = 0;
                    if (((move_value >> 3) + 1) < height) {
                        height   = ((move_value >> 3) + 1);
                        win_move = moves[m];
                    }
                }
            }

            /* If it's a losing move... */
            else if (win_move == 0 && ((move_value >> 3) + 1) > height) {
                height = ((move_value >> 3) + 1);
            }
        }

        /* Get additional information */
        if (unique) { 
            empty_cells     = component_size(board, win_move);
            board[0]        = win_move;
            board[win_move] = next;
            num_replies     = get_moves(board, replies);
            board[win_move] = 0;
            board[0]        = pawn;            
        }
        
        /* Output selected problems */
        if (unique                 && MIN_REGION  <= empty_cells &&
            MIN_MOVES <= num_moves && MIN_REPLIES <= num_replies &&
            MIN_PLIES <= height    && height <= MAX_PLIES) {
            write(board, win_move, height);
        }

        /* Compute value of current position */
        value  =  win_move ? next : 3-next;   /*  bit structure of `value`  */
        value |= (unique << 2);               /*      8 7 6 5 4 3 2 1       */
        value |= (height << 3);               /*      <-height> u win       */
    }

    /* Store and return the value */
    put(cache, (h | value));
    return value;
}

/* Makes a legal move uniformly at random */
IMPORT_CLM_RAND()
void play_random(uint_t *board) {
    uint_t moves[12];
    uint_t size = get_moves(board, moves);
    uint_t turn = board[0] ? 3-board[board[0]] : 1;
    if (size) { board[0] = moves[rand_size_t(size)]; board[board[0]] = turn; }
}



/*** MAIN FUNCTION ***********************************************************/

int main() {

    cache_t cache = NULL;
    uint_t  i, board[29];
    srand(time(0));

    for (size_t g = 0; g < NUM_TRIALS; g++) {
        for (i = 0; i <= 28; i++)        { board[i] = 0;       }
        for (i = 0; i < SEED_PLIES; i++) { play_random(board); }
        solve(board, &cache);
        clear(&cache);
    }
}

/*****************************************************************************/

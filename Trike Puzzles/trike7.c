
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
  Version:  2020-08-12
  
  =============================================================================
  
  Trike 7 board: Stored as a 1D array of 29 uint_t (board[0] = pawn index)
  
                              | Neighborhoods |        Permutations           |
               1              |---------------|-------------------------------|
             2   3            |               |                               |
           4   5   6          |      N6  N1   |  P0:  1    P2:  2    P4:  3   |
         7   8   9  10        |       \ /     |      2 3       3 1       1 2  |
      11  12  13  14  15      |   N5--N0--N2  |                               |
    16  17  18  19  20  21    |      / \      |  P1:  1    P3:  3    P5:  2   |
  22  23  24  25  26  27  28  |    N4  N3     |      3 2       2 1       1 3  |

  =============================================================================

  Trike 7 hash: Stored as a single 64 bits unsigned integer
  
    [64, 63, 62, ... , 39, 38, 37, 36, 35, 34, ... , 10, 09, 08, 07, ..., 01]      
    [28 <-- Player 2 pieces --> 1, 28 <-- Player 1 pieces --> 1, <- Value ->]

    * Value[0] == 1   <=>   We know that Player 1 wins this position.
    * Value[1] == 1   <=>   We know that Player 2 wins this position.
    * Value[2-7] bits are available to store extra information.
    * Pawn position is stored by setting its bit to 1 for both players.
      You can deduce the real pawn color by counting the number of pieces.

 *****************************************************************************/



#include "CLM_LIBS.h"

typedef uint_least8_t uint_t;   /*  8 bits unsigned integer */
typedef uint_fast64_t hash_t;   /* 64 bits unsigned integer */



/*** BOARD TOPOLOGY **********************************************************/

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

/* `P[symmetry][index]` is the `permutation` of `index` by that `symmetry` */
const uint_t P[6][29] = {{ 0,               1,
                                          2,  3,
                                        4,  5,  6,
                                      7,  8,  9, 10,
                                   11, 12, 13, 14, 15,
                                 16, 17, 18, 19, 20, 21,
                               22, 23, 24, 25, 26, 27, 28},
                         { 0,               1,
                                          3,  2,
                                        6,  5,  4,
                                     10,  9,  8,  7,
                                   15, 14, 13, 12, 11,
                                 21, 20, 19, 18, 17, 16,
                               28, 27, 26, 25, 24, 23, 22},
                         { 0,              22,
                                         23, 16,
                                       24, 17, 11,
                                     25, 18, 12,  7,
                                   26, 19, 13,  8,  4,
                                 27, 20, 14,  9,  5,  2,
                               28, 21, 15, 10,  6,  3,  1},
                         { 0,              28,
                                         27, 21,
                                       26, 20, 15,
                                     25, 19, 14, 10,
                                   24, 18, 13,  9,  6,
                                 23, 17, 12,  8,  5,  3,
                               22, 16, 11,  7,  4,  2,  1},
                         { 0,              28,
                                         21, 27,
                                       15, 20, 26,
                                     10, 14, 19, 25,
                                    6,  9, 13, 18, 24,
                                  3,  5,  8, 12, 17, 23,
                                1,  2,  4,  7, 11, 16, 22},
                         { 0,              22,
                                         16, 23,
                                       11, 17, 24,
                                      7, 12, 18, 25,
                                    4,  8, 13, 19, 26,
                                  2,  5,  9, 14, 20, 27,
                                1,  3,  6, 10, 15, 21, 28}};

/* Hashes a `board` and a `value` into a 64 bit unsigned integer */
hash_t hash_1(uint_t *board, uint_t value) {

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

/* Returns the smallest hash amongst the 6 possible permutations of the board */
hash_t hash_6(uint_t *board, uint_t value) {

    uint_t i, h;
    hash_t H[6] = {0, 0, 0, 0, 0, 0};

    /* Encode value and pawn position */
    for (h = 0; h < 6; h++) {
        H[h] |= ((hash_t) value);
        H[h] |= ((hash_t)     1) << ( 7 + P[h][board[0]]);
        H[h] |= ((hash_t)     1) << (35 + P[h][board[0]]);
    }

    /* Encode the rest of the pieces */
    for (i = 1; i <= 28; i++) {
        for (h = 0; h < 6; h++) {
            if      (board[i] == 1) { H[h] |= ((hash_t) 1) << ( 7 + P[h][i]); }
            else if (board[i] == 2) { H[h] |= ((hash_t) 1) << (35 + P[h][i]); }
        } 
    }

    /* Return the smallest one */
    for (h = 1; h < 6; h++) { if (H[h] < H[0]) { H[0] = H[h]; } }
    return H[0];
}

/* Thin wrapper for testing different hash functions */
hash_t hash(uint_t *board, uint_t value) { return hash_1(board, value); }

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



/*** CACHE *******************************************************************/

/* This dictionary is just a thin wrapper aroung my generic splay-tree code */
#define LESS(h1,h2)  (((h1)>>8) < ((h2)>>8))
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



/*** INTERFACE ***************************************************************/

/* Draws the board on the screen, highlighting the pawn and the winning move */
void draw(uint_t *board, uint_t winning_move) {

    /* Filter positions with more than 12 pieces */
    uint_t i, pieces = 0;
    for (i = 1; i <= 28; i++) { if (board[i]) { pieces++; } }
    if (pieces > 12) { return; }
    
    /* Draw positions with 12 or less pieces */
    
    char C[29] = {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
                  '.', '.', '.', '.', '.', '.', '.', '.', '.', '.',
                  '.', '.', '.', '.', '.', '.', '.', '.', '.'};

    C[winning_move] = ':';
    for (uint_t i = 1; i <= 28; i++) {
        if (board[i] == 1) { C[i] = 'x'; }
        if (board[i] == 2) { C[i] = 'o'; }
    }
    if (board[board[0]] == 1) { C[board[0]] = 'X'; C[0] = 'O'; }
    if (board[board[0]] == 2) { C[board[0]] = 'O'; C[0] = 'X'; }
    
    printf("\nPlayer %c to play and win:\n",     C[ 0]);
    printf("      %c\n",                         C[ 1]);
    printf("     %c %c\n",                     C[ 2],C[ 3]);
    printf("    %c %c %c\n",                 C[ 4],C[ 5],C[ 6]);
    printf("   %c %c %c %c\n",             C[ 7],C[ 8],C[ 9],C[10]);
    printf("  %c %c %c %c %c\n",         C[11],C[12],C[13],C[14],C[15]);
    printf(" %c %c %c %c %c %c\n",     C[16],C[17],C[18],C[19],C[20],C[21]);
    printf("%c %c %c %c %c %c %c\n", C[22],C[23],C[24],C[25],C[26],C[27],C[28]);    
}

/* Prints a `pawn, (board), winning_move` line in the standar output */
void write(uint_t *board, uint_t winning_move) {
    uint_t i, pieces = 0;
    for (i = 1; i <= 28; i++) { if (board[i]) { pieces++; } }
    if (pieces <= 12) {
        printf("%2d, (", ((int) (board[0])));
        for (int i = 1; i < 29; i++) { printf("%d,", ((int) (board[i]))); }  
        printf("%d), %2d\n", ((int) (board[29])), ((int) (winning_move)));
    }
}



/*** GAME LOGIC **************************************************************/

/* Stores all legal moves in `moves` and return how many of them are */
uint_t get_moves(uint_t *board, uint_t *moves) {

    uint_t i, m = 0;
    
    if (board[0]) {
        for (uint_t d = 1; d <= 6; d++) {
            i = N[d][board[0]];
            while (i && !board[i]) { moves[m++] = i; i = N[d][i]; }
        }
    } else {
        moves[0] = 1; moves[1] = 2; moves[2] = 4;  moves[3] = 5;
        moves[4] = 7; moves[5] = 8; moves[6] = 13; m        = 7; 
    }
    return m;
}

/* Returns the winner (1 or 2) of an end-game position */
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
    assert(first != second);
    if      (first > second) { return 1; }
    else if (first < second) { return 2; }
    else                     { return 0; }
}

/* Returns the winner (1 or 2) of a mid-game position */
uint_t solve(uint_t *board, stree *cache) {

    /* Try to recover the value from the cache */
    hash_t h     = hash(board, 0);
    uint_t value =  get(cache, h);
    if (value & 3) { return (value & 3); }

    /* Otherwise we need to compute the value */
    uint_t m, moves[12];
    uint_t num_moves = get_moves(board, moves);
    
    /* End-game position: return the winner */
    if (num_moves == 0) { value = get_winner(board); }

    /* Mid-game position: recourse */
    else {
        uint_t winner, w_move = 0;
        uint_t pawn = board[0];
        uint_t turn = pawn ? 3-board[pawn] : 1;
        for (m = 0; m < num_moves; m++) {
            board[moves[m]] = turn;
            board[0]        = moves[m];
            winner          = solve(board, cache);
            board[moves[m]] = 0;
            board[0]        = pawn;
            if (winner == turn) {
                if (w_move) { break;             } /* 2nd winning move found */
                else        { w_move = moves[m]; } /* 1st winning move found */               
            }
        }
        if (w_move && m == num_moves) { draw(board, w_move); }
        value = w_move ? turn : 3-turn;
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

    cache_t cache     = NULL;
    uint_t  board[29] = {0,     0,
                              0,  0,
                            0,  0,  0,
                          0,  0,  0,  0,
                        0,  0,  0,  0,  0,
                      0,  0,  0,  0,  0,  0,
                    0,  0,  0,  0,  0,  0,  0};

    for (uint_t i = 0; i < 8; i++) { play_random(board); }

    solve(board, &cache);

    printf("\n%zu elements in cache\n", clear(&cache));
}

/*****************************************************************************/

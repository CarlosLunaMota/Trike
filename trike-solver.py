#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    TRIKE SOLVER
    ============
    

    Player "1" plays first, player "2" plays second. An empty cell holds a "0".
    The value of a board postion is either "1" or "2", depending on who
    wins that position (assuming that both players play perfectly).


    
    All the functions encode a board position in a single integer as follows:
        
        PawnPosition Cell[1] Cell[2] Cell[3] ... Cell[N]

    For example, the following position:
    
               1                              1
              0 0                           2   3
             2 0 0                        4   5   6
            1 0(2)0                     7   8   9  10
           1 0 1 0 0                 11  12  13  14  15
          0 0 0 1 0 0              16  17  18  19  20  21

    Is encoded as:

        9100200102010100000100

    And the following position:
    
               1                              1
              0 0                           2   3
             2 0 0                        4   5   6
            1 0 2 0                     7   8   9  10
           1 0 1 0 0                 11  12  13  14  15
          0 0 0 1 1(2)              16  17  18  19  20  21

    Is encoded as:

        21100200102010100000112


    The empty board is encoded as 0.

"""

__author__   = "Carlos Luna-Mota"
__license__  = "The Unlicense"
__version__  = "20200528"


### PARAMETERS (CHANGE THEM!) ################################################## 

SIZE             = 3        # Length of a side of the board
COMPLETE_TREE    = False    # Evaluate ALL reachable positions
BREAK_SYMMETRIES = True     # Break mirror & rotational symmetries

PRINT_COMPACT    = False    # Print compact representation of the memory
PRINT_EXTENDED   = False    # Print graphic representation of the memory 
PRINT_TREE       = True     # Print tree-like representation of the memory


### BOARD TOPOLOGY #############################################################

CELLS = (tuple([None]+[(r,          c)        for r in range(SIZE) for c in range(r+1)]),
         tuple([None]+[(r,          r-c)      for r in range(SIZE) for c in range(r+1)]),
         tuple([None]+[(SIZE+c-r-1, SIZE-r-1) for r in range(SIZE) for c in range(r+1)]),
         tuple([None]+[(SIZE+c-r-1, c)        for r in range(SIZE) for c in range(r+1)]),
         tuple([None]+[(SIZE-c-1,   r-c)      for r in range(SIZE) for c in range(r+1)]),
         tuple([None]+[(SIZE-c-1,   SIZE-1-r) for r in range(SIZE) for c in range(r+1)]))

INDEX = tuple({c:n for n,c in enumerate(cells)} for cells in CELLS)


### AUXILIARY FUNCTIONS ########################################################

def encode(board, pawn):
    """Given a (board, pawn) pair, return s its corresponding code"""

    # Encode this position in a single integer
    if not BREAK_SYMMETRIES:
        code = INDEX[0][pawn]    
        for cell in CELLS[0][1:]: code = (code*10) + board[cell]
        return code

    # Encode this position in a single integer breaking symmetries
    else:
        codes = []
        for i in range(6):
            code = INDEX[i][pawn]    
            for cell in CELLS[i][1:]: code = (code*10) + board[cell]
            codes.append(code)
        return min(codes)

def decode(code):
    """Given a code, returns its corresponding (board, pawn) pair"""

    board = {}
    for i in range(SIZE*(SIZE+1)//2, 0, -1):
        board[CELLS[0][i]] = code%10
        code             //= 10
    pawn = CELLS[0][code]
    return board, pawn

def draw(code):
    board, pawn = decode(code)
    for r in range(SIZE):
        row  = ["\t" + " " * (4+SIZE-r)]
        for c in range(r+1):
            if (r,c) == pawn:  row.append("("+".12"[board[(r,c)]]+")")
            else:              row.append(    ".12"[board[(r,c)]]    )
        row = " ".join(row)
        row = row.replace(" (", "(")
        row = row.replace(") ", ")")
        print(row)

def count_pieces(code):
    """Returns the number of pieces in the encoded board"""
    
    board, pawn = decode(code)
    return sum(1 if board[cell] else 0 for cell in CELLS[0][1:])

def draw_subtree(board, pawn, memory):
    
    code = encode(board, pawn)
    if code in memory:

        # Print current node:
        n = SIZE*(SIZE+1)//2
        t = "\t"+"  ┆" * (sum(1 if board[cell] else 0 for cell in CELLS[0][1:]))
        b = ["".join(str(board[r,c]) for c in range(r+1)) for r in range(SIZE)]
        w = memory[code]
        print(t + "  ╰┄┄{} ({} wins)".format("/".join(b), w))

        # Remove this node from memory:
        del memory[code]

        # Recursively print the descendants:
        turn = 3-board[pawn] if pawn else 1
        for move in legal_moves(board, pawn):
            board[move] = turn
            draw_subtree(board, move, memory)
            board[move] = 0


### GAME LOGIC #################################################################

def legal_moves(board, pawn):
    """Returns a list of legal moves from the current position."""

    # You can play anywhere in the first turn:
    if not pawn: return CELLS[0][1:]

    # After the first turn the neutral pawn can move in any direction...
    moves  = []                         #             (0, 0)
    deltas = (    (-1,-1),(-1, 0),      #         (1, 0), (1, 1)
              ( 0,-1),        ( 0, 1),  #     (2, 0), (2, 1), (2, 2)
                  ( 1, 0),( 1, 1))      # (3, 0), (3, 1), (3, 2), (3, 3)

    # ...but cannot move onto, or jump over occupied points:
    for (dr,dc) in deltas:
        rr,cc = pawn[0]+dr, pawn[1]+dc
        while 0 <= cc <= rr < SIZE and board[(rr,cc)] == 0:
           moves.append((rr,cc))
           rr += dr
           cc += dc

    return moves

def who_wins(board, pawn):
    """If the game is over, returns the winner. Otherwise, returns 0."""

    # If nobody has played yet, the game cannot be over:
    if not pawn: return 0

    # For each cell adjacent to, or underneath, the pawn...
    (r,c) = pawn                            #             (0, 0)
    neighborhood = (  (r-1,c-1),(r-1,c),    #         (1, 0), (1, 1)
                    (r,c-1),(r,c),(r,c+1),  #     (2, 0), (2, 1), (2, 2)
                      (r+1,c),(r+1,c+1))    # (3, 0), (3, 1), (3, 2), (3, 3)

    # ...each player gets a point for each checker of their own color:
    first  = 0
    second = 0
    for (rr,cc) in neighborhood:
        if 0 <= cc <= rr < SIZE:
            if   board[(rr,cc)] == 1: first  += 1
            elif board[(rr,cc)] == 2: second += 1
            else:                     return 0

    # The person with the highest score wins:
    if first > second: return 1
    else:              return 2

def solve(board, pawn, memory):
    """A simple Negamax solver for Trike"""

    # If we don't know yet the value of this position
    code = encode(board, pawn)
    if code not in memory:
        
        value = who_wins(board, pawn)

        # If the game is NOT over
        if not value:

            turn  = 3-board[pawn] if pawn else 1
            value =   board[pawn] if pawn else 2

            # Evaluate all legal moves looking for a winning move
            for move in legal_moves(board, pawn):
                board[move] = turn
                move_value  = solve(board, move, memory)
                board[move] = 0
                if move_value == turn:
                    value = turn
                    if not COMPLETE_TREE: break
                
        # Store the value of this position.
        memory[code] = value

    # Return the value of this position
    return memory[code]


### MAIN FUNCTION ##############################################################

if __name__ == "__main__":


    ### SOLVE THE GAME #########################################################

    board, pawn = decode(0)     # Empty board
    memory      = dict()
    value       = solve(board, pawn, memory)


    ### OUTPUT INFORMATION #####################################################

    if PRINT_COMPACT:
        n = SIZE*(SIZE+1)//2
        print("\n\tSOLVED POSITIONS:\n")
        for code in sorted(memory, key=count_pieces):
            print("\t  Player {} wins: ({}){}".format(memory[code],
                                                      str(code)[:-n],
                                                      str(code)[-n:]))
    if PRINT_TREE:
        print("\n\tGAME TREE:\n")
        draw_subtree(board, pawn, dict(memory))

    if PRINT_EXTENDED:
        print("\n\tSOLVED POSITIONS:\n")
        for code in sorted(memory, key=count_pieces):
            print("\n\t  Player {} wins:\n".format(memory[code]))
            draw(code)



    print("\n\tBOARD SIZE:       {}".format(SIZE))
    print("\tCOMPLETE TREE:    {}".format(COMPLETE_TREE))
    print("\tBREAK SYMMETRIES: {}".format(BREAK_SYMMETRIES))
    print("\n\t{} positions visited below:\n".format(len(memory)))
    draw(encode(board, pawn))
    print("\n\tPlayer {} wins this position".format(value))

################################################################################

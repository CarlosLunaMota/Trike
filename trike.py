#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    Trike is a two-player, combinatorial, abstract strategy game designed
    by Alek Erickson in April 2020.


    Play on an equilateral triangular hexagon-tessellated grid with sides
    between 6-12. Use a neutral pawn and black/white checkers.

    Players take turns moving a neutral pawn around on the board (passing
    is not allowed). The neutral pawn can move any number of empty points,
    in any direction in a straight line, but cannot move onto, or jump over
    occupied points.

    When a player moves the pawn, first they place a checker of their own
    color onto the destination point, then they move the pawn on top of it.
    When the pawn is trapped, the game is over.

    At the end of the game, each player gets a point for each checker
    of their own color adjacent to, or underneath, the pawn. The person
    with the highest score wins.


    More info at: https://boardgamegeek.com/boardgame/307379/trike

"""

__all__      = ["trike"]
__author__   = "Carlos Luna-Mota"
__license__  = "The Unlicense"
__version__  = "20200425"

import random

### CONSTANTS (DO NOT CHANGE THEM!) ############################################

PLAYER   =  1   # Must be 1 or -1
COMPUTER = -1   # Must be -PLAYER
EMPTY    =  0   # Must be 0

WINNING  =  1   # Must be  1
LOSING   = -1   # Must be -1
UNKNOWN  =  0   # Must be  0

### GAME LOGIC #################################################################

def legal_moves(board):
    """Returns a list of legal moves from the current position."""

    size = board["size"]
    pawn = board["pawn"]

    # You can play anywhere in the first turn:
    if not pawn: return [(r,c) for r in range(size) for c in range(r+1)]

    # After the first turn the neutral pawn can move in any direction...
    moves  = []                         #             (0, 0)
    deltas = (    (-1,-1),(-1, 0),      #         (1, 0), (1, 1)
              ( 0,-1),        ( 0, 1),  #     (2, 0), (2, 1), (2, 2)
                  ( 1, 0),( 1, 1))      # (3, 0), (3, 1), (3, 2), (3, 3)

    # ...but cannot move onto, or jump over occupied points:
    for (dr,dc) in deltas:
        rr,cc = pawn[0]+dr, pawn[1]+dc
        while 0 <= cc <= rr < size and board[(rr,cc)] == EMPTY:
           moves.append((rr,cc))
           rr += dr
           cc += dc

    return moves

def who_wins(board):
    """If the game is over, returns the winner. Otherwise, returns EMPTY."""

    # If nobody has played yet, the game cannot be over:
    if not board["pawn"]: return EMPTY

    # For each cell adjacent to, or underneath, the pawn...
    (r,c) = board["pawn"]                   #             (0, 0)
    neighborhood = (  (r-1,c-1),(r-1,c),    #         (1, 0), (1, 1)
                    (r,c-1),(r,c),(r,c+1),  #     (2, 0), (2, 1), (2, 2)
                      (r+1,c),(r+1,c+1))    # (3, 0), (3, 1), (3, 2), (3, 3)

    # ...each player gets a point for each checker of their own color:
    computer = 0
    player   = 0
    size     = board["size"]
    for (rr,cc) in neighborhood:
        if 0 <= cc <= rr < size:
            if   board[(rr,cc)] == PLAYER:   player   += 1
            elif board[(rr,cc)] == COMPUTER: computer += 1
            else:                            return EMPTY

    # The person with the highest score wins:
    if player > computer: return PLAYER
    else:                 return COMPUTER

### AI #########################################################################

def get_value(board, depth):
    """
    A simple recursive negamax solver for Tryke (negamin, indeed).

    Returns the value of the current position for the player that just moved:

        value = WINNING     if it was a winning move
        value = LOSING      if it was a losing move
        value = UNKNOWN     if its value is unknown

    """

    # Check if the game is already over or if we have run out of depth:
    pawn    = board["pawn"]
    turn    = board[pawn]
    is_over = who_wins(board)
    if is_over or depth == 0: return is_over * turn

    # Otherwise, explore the game tree:
    value = WINNING
    for move in legal_moves(board):
        board[move]   = -turn                                   # Place checker
        board["pawn"] =  move                                   # Move pawn
        value         = min(value, -get_value(board, depth-1))  # Get value
        board[move]   = EMPTY                                   # Remove checker
        board["pawn"] = pawn                                    # Move pawn back
        if value == LOSING: break                               # Prunning

    return value

def get_computer_move(board, AI_level):
    """Return the best move the AI is able to find at the current AI_level."""

    winning = []
    unknown = []
    losing  = []
    pawn    = board["pawn"]

    # Classify all legal moves:
    for move in legal_moves(board):

        board[move]   = COMPUTER                    # Place a checker
        board["pawn"] = move                        # Move the pawn
        value         = get_value(board, AI_level)  # Get value
        board[move]   = EMPTY                       # Remove the checker
        board["pawn"] = pawn                        # Move back the pawn

        if   value == WINNING: winning.append(move)
        elif value == UNKNOWN: unknown.append(move)
        else:                  losing.append(move)

    # Report results:
    cells = ((r,c) for r in range(board["size"]) for c in range(r+1))
    name  = {cell:str(i+1) for i,cell in enumerate(cells)}
    print("\n AI("+(str(AI_level) if AI_level > 0 else "infinity")+") found:\n")
    print("  * Winning moves: " + ", ".join(name[cell] for cell in winning))
    print("  * Unknown moves: " + ", ".join(name[cell] for cell in unknown))
    print("  * Losing  moves: " + ", ".join(name[cell] for cell in losing))

    # Choose at random among the best moves you can find:
    if   winning: return random.choice(winning)
    elif unknown: return random.choice(unknown)
    elif losing:  return random.choice(losing)
    else:         return None

### USER INTERFACE #############################################################

try: input = raw_input  # Use Python 2 raw_input function
except NameError: pass  # Use Python 3 input function

def get_player_move(board):
    """Ask the user which move wants to make."""

    legal = dict()
    cells = ((r,c) for r in range(board["size"]) for c in range(r+1))
    for i,cell in enumerate(cells):
        if cell in legal_moves(board): legal[i+1] = cell

    question = "\n What is your next move?\n\n "
    move     = input(question)
    while not move or int(move) not in legal: move = input(question)

    return legal[int(move)]

def show(board):
    """Prints board on the screen, highlighting last move & legal moves."""

    size  = board["size"]
    pawn  = board["pawn"]
    moves = legal_moves(board)

    print(" " + "_" * (6*size + 6) + "\n")

    total = 1
    for r in range(size):
        row  = [" " *   (size-r)]
        info = [" " * 3*(size-r)]
        for c in range(r+1):
            if (r,c) == pawn:  row.append(".XO"[board[(r,c)]])
            else:              row.append(".xo"[board[(r,c)]])
            if (r,c) in moves: info.append("{:4d}".format(total+c))
            else:              info.append("   ·")
        print(" " + " ".join(row) + "".join(info))
        total += r+1

    print(" " + "_" * (6*size + 6))

### MAIN FUNCTION ##############################################################

def trike(size, AI_level):
    """
    A simple Player vs Computer command line implementation of Trike.

     * Parameter `size`     (integer >= 1) controls the boad size.
     * Parameter `AI_level` (integer >= 0) controls the ply-depth of the AI.

    Any negative value for `AI_level` is interpreted as `infinity`.

    """

    # Validate parameter:
    assert(size > 0)

    # Initialize game:
    turn          = random.choice((PLAYER, COMPUTER))
    cells         = tuple((r,c) for r in range(size) for c in range(r+1))
    board         = {cell:EMPTY for cell in cells}
    board["size"] = size
    board["pawn"] = None
    
    if turn == PLAYER: print("\n You play first")
    else:              print("\n The computer plays first")
    
    # Play the game:
    while True:

        show(board)

        if turn == PLAYER: move = get_player_move(board)
        else:              move = get_computer_move(board, AI_level)

        board[move]   =  turn               # Place the checker
        board["pawn"] =  move               # Move the pawn
        turn          = -turn               # Change turn
        is_over       =  who_wins(board)    # Check if it is over

        if is_over:
            show(board)
            if is_over == PLAYER: print("\n You have won!")
            else:                 print("\n You have lost!")
            break

if __name__ == "__main__": trike(size=9, AI_level=5)

################################################################################

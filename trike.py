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
__version__  = "20200424"

import random

### CONSTANTS (DO NOT CHANGE THEM!) ############################################

PLAYER   =  1   # Must be 1 or -1
COMPUTER = -1   # Must be -PLAYER
EMPTY    =  0   # Must be 0

WINNING  =  1   # Must be  1
LOSING   = -1   # Must be -1
UNKNOWN  =  0   # Must be  0

### GAME LOGIC #################################################################

def legal_moves(board, last_move):
    """Returns a list of legal moves from the current position."""

    # You can play anywhere in the first turn:
    size = board["size"]
    if not last_move: return [(r,c) for r in range(size) for c in range(r+1)]

    # After the first turn the neutral pawn can move in any direction...
    (r,c)  = last_move                  #             (0, 0)
    deltas = (    (-1,-1),(-1, 0),      #         (1, 0), (1, 1)
              ( 0,-1),        ( 0, 1),  #     (2, 0), (2, 1), (2, 2)
                  ( 1, 0),( 1, 1))      # (3, 0), (3, 1), (3, 2), (3, 3)

    # ...but cannot move onto, or jump over occupied points:
    moves  = []
    for (dr,dc) in deltas:
        rr,cc = r+dr, c+dc
        while 0 <= cc <= rr < size and board[(rr,cc)] == EMPTY:
           moves.append((rr,cc))
           rr += dr
           cc += dc

    return moves

def who_wins(board, last_move):
    """If the game is over, returns the winner. Otherwise, returns EMPTY."""

    # For each cell adjacent to, or underneath, the pawn...
    (r,c) = last_move                       #             (0, 0)
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

def get_value(board, last_move, depth):
    """
    A simple recursive negamax solver for Tryke.

    Returns the value of the current position for the player that just moved:

        value = WINNING     if it was a winning move
        value = LOSING      if it was a losing move
        value = UNKNOWN     if its value is unknown

    """

    # Check if the game is already over or if we have run out of depth:
    last_turn = board[last_move]
    is_over   = who_wins(board, last_move)
    if is_over or depth == 0: return is_over * last_turn

    # Otherwise, explore the game tree:
    value = LOSING
    for move in legal_moves(board, last_move):
        board[move] = -last_turn                                    # Make move
        value       = max(value, -get_value(board, move, depth-1))  # Get value
        board[move] = EMPTY                                         # Undo move
        if value == WINNING: break                                  # Prunning

    return value

def get_computer_move(board, last_move, AI_level):
    """Return the best move the AI is able to find at the current AI_level"""

    winning = []
    unknown = []
    losing  = []

    # Classify all legal moves:
    for move in legal_moves(board, last_move):

        board[move] = COMPUTER                          # Make move
        value       = get_value(board, move, AI_level)  # Get value
        board[move] = EMPTY                             # Undo move

        if   value == WINNING: winning.append(move)
        elif value == UNKNOWN: unknown.append(move)
        else:                  losing.append(move)

    # Report results:
    cells = ((r,c) for r in range(board["size"]) for c in range(r+1))
    name  = {cell:str(i+1) for i,cell in enumerate(cells)}
    print("\n AI(" + str(AI_level) + ") found...\n")
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

def get_player_move(board, last_move):
    """Ask the user which move wants to make. Shows legal moves."""

    legal = dict()
    cells = ((r,c) for r in range(board["size"]) for c in range(r+1))
    for i,cell in enumerate(cells):
        if cell in legal_moves(board, last_move): legal[i+1] = cell

    question = "\n What is your next move?\n\n "
    move     = input(question)
    while not move or int(move) not in legal: move = input(question)

    return legal[int(move)]

def show(board, last_move):
    """Prints board on the screen, shows last move and legal moves."""

    size  = board["size"]
    moves = legal_moves(board, last_move)

    print(" " + "_" * (6*size + 6) + "\n")

    total = 1
    for r in range(size):
        row  = [" " *   (size-r)]
        info = [" " * 3*(size-r)]
        for c in range(r+1):
            if (r,c) == last_move: row.append(".XO"[board[(r,c)]])
            else:                  row.append(".xo"[board[(r,c)]])
            if (r,c) in moves:     info.append("{:4d}".format(total+c))
            else:                  info.append("   ·")
        print(" " + " ".join(row) + "".join(info))
        total += r+1

    print(" " + "_" * (6*size + 6))

### MAIN FUNCTION ##############################################################

def trike(size, AI_level):
    """
    A simple Player vs Computer command line implementation of Trike.

     * Parameter `size`     (integer >= 1) controls the boad size.
     * Parameter `AI_level` (integer >= 0) controls the ply-depth of the AI.

    """

    # Initialize game:
    turn          = random.choice((PLAYER, COMPUTER))
    cells         = tuple((r,c) for r in range(size) for c in range(r+1))
    board         = {cell:EMPTY for cell in cells}
    board["size"] = size
    move          = None

    if turn == PLAYER: print("\n You play first")
    else:              print("\n The computer plays first")

    # Play the game:
    while True:

        show(board, move)

        if turn == PLAYER: move = get_player_move(board, move)
        else:              move = get_computer_move(board, move, AI_level)

        board[move] =  turn                 # Play the move
        turn        = -turn                 # Change turn
        is_over     = who_wins(board, move) # Check if it is over

        if is_over:
            show(board, move)
            if is_over == PLAYER: print("\n You have won!")
            else:                 print("\n You have lost!")
            break

if __name__ == "__main__": trike(size=6, AI_level=6)

################################################################################

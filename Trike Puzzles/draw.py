#!/usr/bin/python
# -*- coding: utf-8 -*-

################################################################################
#
#  Content:  Figure generator for Trike7.c problems
#  Source:   <https://github.com/CarlosLunaMota/Trike>
#  Author:   Carlos Luna-Mota
#  Version:  20200815
#
################################################################################

from math import sqrt
from pyx import *

### STYLES #####################################################################

CELL_BACK   = color.cmyk.CornflowerBlue
CELL_LINE   = color.cmyk.MidnightBlue
P1_BACK     = color.grey.white
P1_LINE     = color.grey.black
P2_BACK     = color.grey.black
P2_LINE     = color.grey.black
PAWN_LINE   = color.cmyk.Red
ANSWER_LINE = color.cmyk.MidnightBlue

CELL   = [style.linewidth.THick, style.linestyle.solid, style.linecap.round,
          style.linejoin.round, CELL_LINE, deco.filled([CELL_BACK])]
P1     = [style.linewidth.thick, style.linestyle.solid, style.linecap.round,
          style.linejoin.round, P1_LINE, deco.filled([P1_BACK])]
P2     = [style.linewidth.thick, style.linestyle.solid, style.linecap.round,
          style.linejoin.round, P2_LINE, deco.filled([P2_BACK])]
PAWN   = [style.linewidth.THICK, style.linestyle.solid, style.linecap.round,
          style.linejoin.round, PAWN_LINE]
ANSWER = [style.linewidth.THICK, style.linestyle.solid, style.linecap.round,
          style.linejoin.round, ANSWER_LINE]

### AUXILIARY FUNCTIONS ########################################################

def hexagon((x,y), r):
    s = r/2.0
    t = s*sqrt(3.0)
    return [(x,y+r), (x+t,y+s), (x+t,y-s), (x,y-r), (x-t,y-s), (x-t,y+s)]
    
def draw(name, board):

    r, dx, dy = 1.0/sqrt(3.0), 1.0, sqrt(3.0)/2.0
    drawing   = []
    for h in range(7):
        for w in range(7-h):
            i = (7-h)*(6-h)//2 + w
            O = (dx*w+dx*h/2.0, dy*h)
            H = hexagon(O, r)
            drawing.append((path.path(path.moveto(*H[0]),
                                      path.lineto(*H[1]),
                                      path.lineto(*H[2]),
                                      path.lineto(*H[3]),
                                      path.lineto(*H[4]),
                                      path.lineto(*H[5]),
                                      path.closepath()), CELL))
            if board[i] == '1':
                drawing.append((path.circle(O[0], O[1], 1.0/2.7), P1))
            elif board[i] == '2':
                drawing.append((path.circle(O[0], O[1], 1.0/2.7), P2))
            elif board[i] == '5':
                drawing.append((path.circle(O[0], O[1], 1.0/2.7), P1))
                drawing.append((path.path(path.moveto(*O), path.closepath()), PAWN))
            elif board[i] == '6':
                drawing.append((path.circle(O[0], O[1], 1.0/2.7), P2))
                drawing.append((path.path(path.moveto(*O), path.closepath()), PAWN))
                                      
    mycanvas = canvas.canvas()
    for (p, s) in drawing: mycanvas.stroke(p, s)
    mycanvas.writePDFfile("puzzles/"+name+"-p")

    for h in range(7):
        for w in range(7-h):
            i = (7-h)*(6-h)//2 + w
            O = (dx*w+dx*h/2.0, dy*h)
            if board[i] == '4':
                drawing.append((path.path(path.moveto(*O), path.closepath()), ANSWER))

    mycanvas = canvas.canvas()
    for (p, s) in drawing: mycanvas.stroke(p, s)
    mycanvas.writePDFfile("puzzles/"+name+"-s")

### MAIN FUNCTION ##############################################################

if __name__ == "__main__":
    H = [0]*30
    with open("puzzles.txt") as f: content = f.readlines()
    for line in content:
        if line.startswith("#"):
            line   = line.strip()
            name   = line.split(" ")[1]
            board  = name.split("-")[1]
            height = int(name.split("-")[0])
            draw(name, board)
            H[height] += 1
    for i,h in enumerate(H):
        if (h): print i,h

################################################################################

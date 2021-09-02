# Board evaluation

## Overview of board evaluation features across chess engines

One of the primary components of an evaluation function is the value assigned to the different piece types.
Values are generally not assigned to the King, because it is never captured, and the values assigned
are often normalized so that the value of the Pawn is 1.0.
The values of the non-King, non-Pawn pieces in standard chess (a.k.a., orthochess)
are often given as Q:9, R:5, B:3, N:3, but current computer chess programs use more precise values.
As of 2020, [AlphaZero uses the values](https://arxiv.org/pdf/2009.04374.pdf)
Q:9.5, R:5.63, B:3.33, N:3.05. How do these values relate to the reach of these piece types,
such as the number of total moves available, summed over all starting positions on the board?
Such a relationship could inform a choice of values for the piece types in different variants
of hexagonal chess. (Of course, these values can be adjusted in light of games played, but
a well-chosen starting point can allow the program to start at a higher skill level than a naive
choice would have provided.)

  * Queen: Orthochess moves=1456; AlphaZero value=9.5. Millipawns per move = 6.5247
  * Rook: Orthochess moves=896; AlphaZero value=5.63. Millipawns per move = 6.2835
  * Bishop: Orthochess moves=560; AlphaZero value=3.33. Millipawns per move = 5.9464
  * Knight: * Orthochess moves=336; AlphaZero value=3.05. Millipawns per move = 9.0774
    * (A King in the center of the board can move in 8 directions, just like the Knight,
      but since it doesn't jump as far as the Knight, its moves aren't as often blocked
      by the edge of the board, so it has more moves than the Knight.
      Specifically, it has a total of 420 moves.)

The number of millipawns per move are similar for the Queen and Rook, but are significantly higher
for the Knight, perhaps because none of its moves can be blocked by an piece interposed between
the starting point and the destination. If we assign a devaluation factor to a move for each piece
that can block its path (think "exponential decay"), then we can model an evaluation system
that brings the values (in terms of millipawns per devaluation-adjusted move) of the Queen and Rook
closer to that of the Knight. (For more details, see the script
[orthochess_move_counts_and_values.py](../../scratch/orthochess_move_counts_and_values.py).)

Note that the Bishop has a lower value of millipawns per move than the other pieces. This could be
due to Bishops in orthochess having access to only half the squares of the board. Bishops in hexagonal
chess could have values even lower (in terms of millipawns per move), since they only have access to
one-third of the cells of the board.

Lastly, Glinski estimated the strengths of the piece types in his chess variant as
Q:10, R:5, B:3, N:4.

## Overview of board evaluation features across chess engines

Here are the categories of features listed in section 2 of Mark Watkins's survey paper of chess engines, called Quantifying evaluation features.
1. Knights
  1. Knight mobility
  2. Knight outposts
  3. Knight in opposing corner or trapped
2. Bishops
  1. Bishop mobility
  2. Trapped bishops
  3. Opposite Bishop ending
  4. Bishop outposts
  5. Bad bishops
  6. Bishop pair (or triple, for hexagonal chess)
     * Note: Larry Kaufman proposed that the value of half a Pawn for a chess pair.
3. Rooks
  1. Rook mobility
  2. Rooks on open files
  3. Rooks on semi-open files
  4. Blocked rooks
  5. Rooks on the 7th rank
4. Queens
  1. Queen mobility
  2. Queens on the 7th rank
  3. Early Queen development
5. Pawns
  1. Doubled Pawns
  2. Isolated Pawns
  3. Backward and/or weak Pawns
  4. Pawn duos
  5. Pawn guards
  6. Pawn center
  7. Pawn immobility (general)
  8. Blocked central Pawns and/or Bishops
  9. Pawn Outpost
  10. Pawn/piece interaction?
6. Passed and candidate Pawns
  1. Candidate passed Pawns
  2. Passed Pawns, initial bonunses
  3. Passed Pawns, support/attack by Kings
  4. Passed Pawns, freedom to advance
  5. Unstoppable Pawns, races, Pawn ending evaluators
7. King danger
  1. King danger, when to use
  2. King danger from pieces, method
  3. King danger from pieces, relative weightings
  4. King shelter from friendly Pawns
  5. King storm from enemy Pawns
8. Conditions regarding game phase
  1. Game phases and/or interpolation
  2. General development and/or thematic moves
  3. Bonuses/penalties with castling
  4. Bishop/Knight ending advantage
  5. Special endgame code
  6. Draw recognition, insufficient material
9. Other
  1. Lazy evaluation
  2. Material imbalances
  3. Drawishness from Pawn structure
  4. Hung pieces, pins, and multiple attacks
  5. Tempo

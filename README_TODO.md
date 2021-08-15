# Hexchess TODO list

* TOP
  * TODO [pri=Top] ASCII-based gameplay
  * TODO [pri=Top] Detect check & checkmate
  * TODO [pri=Top] Determine legal moves
  * TODO [pri=Top] Determine psuedo-legal moves
  * TODO [pri=Top] HTML developer documentation
  * TODO [pri=Top] Move execution
  * TODO [pri=Top] Publish as Debian/Ubuntu package
  * TODO [pri=Top] UI: Highlight legal moves
  * TODO [pri=Top] UI: Menu item: Help -> About
  * TODO [pri=Top] UI: Menu item: Help -> How to Play
  * TODO [pri=Top] UI: Menu item: Hexchess -> New Game
  * TODO [pri=Top] UI: Render pieces
  * TODO [pri=Top] UI: Render moves in UI
  * TODO [pri=Top] UI: Support move input
* HIGH
  * TODO [pri=High] Add subdirectory "engine"
  * TODO [pri=High] Board evaluation feature: Attacked locations in K-Ring-6
  * TODO [pri=High] Board evaluation feature: Base value of pieces
  * TODO [pri=High] Board evaluation feature: board repetition counter
  * TODO [pri=High] Board evaluation feature: En passant location
  * TODO [pri=High] Board evaluation feature: King is in check
  * TODO [pri=High] Board evaluation feature: Liberties in K-Ring-6
  * TODO [pri=High] Board evaluation feature: Multiple bishop bonus
  * TODO [pri=High] Board evaluation feature: Non-progress counter
  * TODO [pri=High] Board evaluation feature: Pawn configurations (elaborate)
  * TODO [pri=High] Board evaluation feature: Pawn promo-gap
  * TODO [pri=High] Board evaluation feature: Piece distance from the middle of the board
  * TODO [pri=High] Board evaluation feature: Positional value variation: King (row=2 * hex1 - hex0 - 5)
  * TODO [pri=High] Board valuation: Implement basic list of game features
  * TODO [pri=High] Computer play: alpha-beta
  * TODO [pri=High] Detect draw by insufficient resources
  * TODO [pri=High] Document extension to PGN format with game specification
  * TODO [pri=High] Import game format=PGN format (verbose move syntax)
  * TODO [pri=High] Initialize game with FEN
  * TODO [pri=High] Qt thread safety
  * TODO [pri=High] Save game in PGN format (verbose move syntax)
  * TODO [pri=High] Save positions as FEN
  * TODO [pri=High] Support alternate pawn configurations for Glinski.
  * TODO [pri=High] Support highlighting of cells by coloring borders (red=Attacked)
  * TODO [pri=High] Support human vs human play
  * TODO [pri=High] UI: Menu item: Edit -> Copy FEN
  * TODO [pri=High] UI: Menu item: Edit -> Copy PGN
  * TODO [pri=High] UI: Menu item: Edit -> Preferences
  * TODO [pri=High] UI: Menu item: Hexchess -> Load Game (PGN)
  * TODO [pri=High] UI: Menu item: Hexchess -> Save Board Image (JPG)
  * TODO [pri=High] UI: Menu item: Hexchess -> Save Board Image (SVG)
  * TODO [pri=High] UI: Menu item: Hexchess -> Save Game (PGN)
  * TODO [pri=High] Undo / Redo (via QtUndo)
* MED
  * TODO [pri=Med] Elo rating computation
  * TODO [pri=Med] Github Action: Create Docker image
  * TODO [pri=Med] In-game Documentation (at first, link to elsewhere)
  * TODO [pri=Med] Match play
  * TODO [pri=Med] Match play: z-score computation
  * TODO [pri=Med] UI: Menu item: Edit -> Offer Draw
  * TODO [pri=Med] UI: Menu item: Edit -> Pause
  * TODO [pri=Med UI: Menu item: Edit -> Redo
  * TODO [pri=Med] UI: Menu item: Edit -> Resign
  * TODO [pri=Med] UI: Menu item: Edit -> Resume
  * TODO [pri=Med] UI: Menu item: Edit -> Undo
  * TODO [pri=Med] Save board as SVG.
  * TODO [pri=Med] Support chess clocks
  * TODO [pri=Med] Tournament play
  * TODO [pri=Mid] Glinski chess w/ Mathewson's layout (center 5 pawns shifted outward 1 space)
* LOW
  * TODO [pri=Low] Add UCI support
  * TODO [pri=Low] Board evaluation feature: Attacked locations in K-Ring-12
  * TODO [pri=Low] Board evaluation feature: Can Castle-K
  * TODO [pri=Low] Board evaluation feature: Can Castle-Q
  * TODO [pri=Low] Board evaluation feature: Count of pieces moved
  * TODO [pri=Low] Board evaluation feature: Count of pieces remaining on board
  * TODO [pri=Low] Board evaluation feature: Liberties of K-Ring-12
  * TODO [pri=Low] Build on Mac OS
  * TODO [pri=Low] Build on Windows
  * TODO [pri=Low] Build on Windows with static libraries
  * TODO [pri=Low] Compare API against that of python-chess
  * TODO [pri=Low] Github Action: Create Debian/Ubuntu package
  * TODO [pri=Low] Github Action: Run from Docker image
  * TODO [pri=Low] Localization
  * TODO [pri=Low] Move style settings to CSS or QPalette
  * TODO [pri=Low] Support different clock settings
  * TODO [pri=Low] Support editing of user preferences: Colors
  * TODO [pri=Low] Support editing of user preferences: Fonts
  * TODO [pri=Low] Support editing of user preferences: Measures
  * TODO [pri=Low] Support McCooey chess
  * TODO [pri=Low] Support user choice of Icon set
  * TODO [pri=Low] Support variant: McCooey
  * TODO [pri=Low] Support variant: Mini Hexchess
  * TODO [pri=Low] Support variant: Shafran
* BOT
  * TODO [pri=Bot] Board evaluation ML feature: Zones
  * TODO [pri=Bot] Board evaluation weight experimentation [req Matches]
  * TODO [pri=Bot] Chess puzzles
  * TODO [pri=Bot] Computer play: MCMC
  * TODO [pri=Bot] Computer play: NN
  * TODO [pri=Bot] Create endgame tablebase
  * TODO [pri=Bot] Create opening book
  * TODO [pri=Bot] Create syzygy tablebase
  * TODO [pri=Bot] Import game format=Jocly JSON
  * TODO [pri=Bot] Play over network
  * TODO [pri=Bot] Support variant: Brusky
  * TODO [pri=Bot] Support variant: De Vasa
  * TODO [pri=Bot] Support variant: Starchess (w/ piece placement phase)
  * TODO [pri=Bot] Support variants using flat-sided hexagons
  * TODO [pri=Bot] Support variant: Wellisch (3-player)

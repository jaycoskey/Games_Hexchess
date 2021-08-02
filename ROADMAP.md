# Hexchess Roadmap

* Description:
  For v0.1.0, a minimal human vs human, non-networked implementation of Gliński's chess (herein referred to as Glinski chess) on Linux, in C++, using Qt.

---

## Requirements
* Minimal requirements:
    * Functional
      * Determination and highlighting of legal moves, of end of game due to insufficient resources
    * Opponents: Human vs. Human
    * Save Game & Load Game (modified PGN format)
  * Non-functional
    * TBD
  * Methodology
    * Testing

## Minimal Deliverables
* Ubuntu package implementing Glinski chess as desktop app, implemented in C++ & Qt.
* Documentation (in-game documentation for users; HTML documentation for developers)

##  Optional features (nice-to-have)
  * Alternate pawn configurations for Glinski.
  * Board configuration
    * Support of boards with flat-sided hexagons
  * Chess clocks
  * Computer strategy: Alpha-beta pruning, Markov-Chain Monte Carlo (MCMC), etc.
  * Engine:
    * Time-limited search for next move
  * Importing:
    * PGN game format
    * JSON format exported by Jocly
  * Opponents: Human vs Computer, Computer vs Computer (with randomness to prevent the same game being played every time)
  * Methodology
    * Docker
    * Github Actions (e.g., Docker build & run tests)
  * Localization, using Qt's mechanism
  * Undo, using QtUndo

## Out of scope
  * Boards with other shapes
  * Computer move determination features
  * Chess engine rating: Targetting any particular chess engine strength
  * Chess puzzles
  * Databases: Opening book/database, or endgame tablebase
  * Play over network
  * UCI support (unless Fairy-Stockfish is adopted as a back-end engine)
  * Variants of hexagonal chess other than Glinski.

---

## Architecture
* Compatibility:
  * Extend chess standards (AN, FEN, PGN, UCI) as needed, with small changes.
  * Use Qt, which supports multiple platforms (Windows, MacOS, Linux)
  * Adhere generally to python-chess's API, as it is popular and well-designed.
* Configurability:
  * Clock settings
  * Colors
    * Settings for B&W, Standard colors, high contrast, etc.
  * Icon set choice
* Extensibility
  * Avoid premature optimization: The path to support of multiple variants doesn't need to be determined before implementation of a second variant begins. Different variants can have different board shape or orientation, different movement of the Pieces, etc. Some options for extensibility:
    * A class enum (Variant) designating the variant, and switch() statements to modify behavior.
    * Dynamic polymorphism
    * Encapsulation (e.g., in small behavioral class)
    * Feature/rule flags
    * Static polymorphism
    * Use of std::variant
  * Upon adding support for a second variant, add a HOWTO document recommending how to add support for other variants.
* Localizability
  * Qt localization
* Security
  * Not addressed

## Design
* TODO: Evaluate suitability of extending Fairy-Stockfish (https://github.com/ianfab/Fairy-Stockfish, by Fabian Fichter) as a back-end for heagonal chess
  * Can F-S model hexchess board (via bitmask)?
  * Can F-S model hexchess moves?
  * Can F-S model hexchess Pawn promotion (uneven)?
  * Licensing implications of using Fairy-Stockfish?
  * Extension to Fairy-Stockfish
  * Inter-component communication: CECP vs UCI:
    * CECP = Chess Engine Communication Protocol
    * UCI = Universal Chess Interface
    * Create Fairy-Stockfish variant configuration for Glinski chess
    * Notation:
      * Extension to FEN (H-FEN, based on X-FEN, FFEN)
      * Extension to PGN (customize via PGN headers)
  * Game Save/Load
  * Coordinate systems
    * Glinski (used by Wikipedia)
    * Ludii
    * Index
    * How does this vary with flat-sided hexagons?
  * iostream output for logging, indlucing to_string(), to_unicode()
* Documentation of rules
  * Explanation of rules
  * Illustration of legal moves for each piece
  * Image of initial board position
  * Glossary: orthogonal = edge-crossing, diagonal = vertex-crossing
* Directory structure
  * PGNs
  * docs
  * images
  * localization
  * src
    * core    // Standard chess classes: Piece, Board, Game, etc.
    * engine  // Logic to determine future moves
    * test    // All tests
    * ui      // GUI based on Qt, and possibly a text-based interface
    * util
* Namespace structure (compare with python-chess)
* Class structure (compare with python-chess)
  * Color
    * hexchess.Black
    * hexchess.White

    * hexchess.color_name  // Black, White
    * hexchess.color_symbol  // B, W

  * Piece
    * hexchess.PieceType
    * hexchess.King
    * hexchess.Queen
    * hexchess.Rook
    * hexchess.Bishop
    * hexchess.Knight
    * hexchess.Pawn

    * hexchess.Piece(chess.PieceType, chess.Color)
    * hexchess.Piece

    * Methods:
      * piece_name  // King .. Pawn
      * piece_symbol  // K .. P
      * symbol() -> string  // E.g., K, Q, R, B, N, P
      * unicode_symbol(*, invert_color:bool=False) -> string

  * Cell
    * hexchess.Cell = int (index into bitvectors)
    * cell_file() -> int
    * cell_rank() -> int

  * CellSet (cf SquareSet, in python-chess)

  * Board
    * Constructor:
      * Board()
      * Board(string fen)
    * Static data:
      * Member for each cell: A1, A2, A3, ... F11, ... L4, L5, L6
        * Possible implementation: integer index into bitvector in bitboards
      * hexchess.file_names
      * hexchess.rank_names
    * Methods:
      * hexchess.board.attackers(player_color, cell)
      * hexchess.board.can_claim_draw()
      * hexchess.board.can_claim_fifty_move_stalemate()
      * hexchess.board.can_claim_threefold_repetition()
      * hexchess.board.halfmove_clock()
      * hexchess.board.is_attacked_by(player_color)
      * hexchess.board.is_check()
      * hexchess.board.is_checkmate()
      * hexchess.board.is_draw()
      * hexchess.board.is_insufficient_material()
      * hexchess.board.is_into_check()
      * hexchess.board.is_seventyfive_moves()  // Calls is_n_moves()
      * hexchess.board.parse_san(string san)
      * hexchess.board.piece_at(Cell cell)
      * hexchess.board.remove_piece_at()
      * hexchess.board.reset()
      * hexchess.board.san(Move move)
      * hexchess.board.set_piece_at(cell, piece, promoted=False)
      * hexchess.board.was_into_check()
    * Instance Data:
      * ep_squares: CellSet // Has 0 or 1 items in Glinski chess
      * fullmove_number: int
      * halfmove_clock: int
      * move_stack: stack[Move]
      * turn: Color

  * Move
    * Constructor: hexchess.Move(Cell from, Cell to)
    * Static data:
      * class enum CastlingMoveType (Kingside, Queenside)
        // Note: Shafran's Chess has four types of castling per player: {Queen,Bishop}'s-side {short,long} castling.
        // QueenSideShort, QueenSideLong, BishopSideShort, BishopSideLong
        // Q-0-0, Q-0-0-0, B-0-0, B-0-0-0
        // Iterate over all possibilities
    * Members:
      * generate_castling_moves()
      * is_capture()
      * is_check()
      * is_checkmate()
      * is_en_passant()
      * is_legal()
      * is_pawn_promotion()
      * to_pgn()

  * Outcome
    * winner: Optional[Color]
    * termination: enum
    * score(Color c)  // For Glinski
      {
        if is_win() { return c == outcome.winner ? 1 : 0; }
        if is_stalemate() { return c == move_stack.last.mover ? 1 : 0; }
      }

  * Termination (class enum):
    * Checkmate
    * Draw_FivefoldRepetition
    * Draw_InsufficientMaterial
    * Draw_SeventyFiveMoves
    * Draw_Stalemate
    * DrawOptional_FiftyMoves
    * DrawOptional_ThreefoldRepetition
    * Loss
    * Win
    * is_loss(Color c)
    * is_win(Color c)
    * is_draw()

  * Game
    * hexchess.pgn.read_game_file(pgn_file_obj)
    * Methods:
      * game.headers(string header_name)

  * GUI framework: Qt5 or Qt6?
  * Hex cells events:
    * On hover
    * On selected
    * On can-move-to
    * Opponent can-capture
    * King is mated
    * King is check-mated
    * Detection of click events
    * Drag pieces or click to designate destination?

---

## Implementation Timeline:
* TODO: List rought outline of tasks for **August, 2021**, and re-calibrate during implementation.
* Task: Planning; Create Qt Hello world app.
* Task: Design UI; Create wiring of GUI classes with Qt Designer.
* Task: Rendering of Board & Pieces
* Task: Create Move bitboard logic; Logic for determination of {pseudo-,}legal moves
* Task: Create stubs of core classes, with high-level API
* Task: Printing of board (ASCII, Unicode, FEN)
* Task: Board configuration via FEN

## Possible future work (contributions welcome)
* Possible future work
  * Additional hexchess variants:
    * Variants with flat-top hexagon boards:
      * McCooey chess, Shafran chess, and Mini Hexchess
      * Starchess (with a piece-placing setup phase)
    * Variants with flat-side hexagon boards:
      * Brusky Chess and De Vasa Chess
      * Wellisch Chess (3-player variant, with novel Pawn promotion rule)
  * Different bitboard algorithms
    * Faster bitboard implementation, possibly at the cost of increased memory use
    * Allow user to chose speed vs memory trade-offs
  * Create databases:
    * Database of games (either historic, or generated by computer-play)
    * Database of openings
    * Database of game endings ("syzygy tablebase")

---

## Appendix: Hexagonal Chess Glossary (TODO)
* Bitboard:
* Flat-side hexagons:
* Flat-top hexagons:
* Hexagonal chess:
* Hexhex board:
* Knight's move:
* Movement, diagonal:
* Movement, orthogonal:
* Pseudo-legal moves:
* Syzygy tablebase (Higher compression storage than is achieved by Nalimov tablebases):

## Appendix: Related Games (TODO: Expand)
  * Glinski w/ Mathewson's layout (center 5 pawns shifted outward 1 space)
  * 88 Hexagons Chess
  * Hexagonia
  * Three Hexagonal Chess
  * Treyshah, by Matthew C. Seabury @ https://www.chessvariants.com/multiplayer.dir/treyshah.html
  * Van Gennip's Hexagonal Chess
    * https://lichess.org/forum/general-chess-discussion/van-gennips-hexagonal-chess
    * Implemented in Zillions of Games: https://www.zillionsofgames.com/cgi-bin/zilligames/submissions.cgi?do=show;id=489

## Appendix: Related projects
  * Open source
    * Desktop
      * Ludii, in Java, at https://ludii.games. Source code at https://github.com/Ludeme/Ludii.
      * Scatha [Open source]
        * Source code (https://web.archive.org/web/20070704003206/http://www.glaurungchess.com/) was last available on www.glarungchess.com at around 2007-07-04.
  * Non-open source
    * Android:
      * Hexagonal Chess Pass and Play, by hyerpunicorn.com
      * HexChess, by Winston Liu
        Currently terminates immediately upon running on my phone with the message "Unfortunately, HexChess has stopped.".
    * Desktop
      * Hex Chess, by Saár Patrik
        * https://www.microsoft.com/en-us/p/hex-chess/9mzqb2blz5wz?activetab=pivot:overviewtab
      * Hexodus III
        Executable can be downloaded as zip file from https://www.chessvariants.com/link2.dir/hexodus.html. Source not available.
      * [???] HsMDE61
        * Source is downloadable as zip from https://www.chessvariants.com/link2.dir/hexodus.html
  * Web
    * greenchess.net
      * Hosts Gliński's Chess @ https://greenchess.net/rules.php?v=glinski
      * Hosts Three Hexagonal Chess @ https://greenchess.net/rules.php?v=three-hexagonal
    * Jocly . Source code available at https://github.com/mi-g/jocly and https://github.com/mi-g/joclyboard

## Appendix: Resources
* TODO

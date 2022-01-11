# Hexagonal Chess

## Vision: An extensible Hexagonal Chess platform with intelligent computer play.

This extensible Hexagonal Chess platform is a work in progress.

Extensibility:
  * New variants can (once complete) be added by dropping in a new class that
    specifies the details of the new variant (e.g., Board, piece moves and
    placements, etc.).
  * New computer players can be added by dropping in a new Player class.
(In some respects, this extensibility makes the source code more like a
domain-specific language than like the implementation of a single chess
variant.)

The first release will only support Glinski's Hexagonal Chess.
The implementation strives to be cross-platform: written in C++, and using Qt
for its user interface.
  * This is intended to be reasonably fast (using bitboards and various cached
    piece movement info), but not blazing. There has not yet been any performance
    tuning.
  * The first version of the alpha-beta computer player should be fairly free of
    blunders, but not expert. The plan is to tune/improve its skill level through
    self-completition.
  * The UI (once implemented) should be usable, but not polished or flashy.

Those who would like to learn more about hexagonal chess can get a decent
introduction by reading
[the Wikipedia page on Hexagonal chess](https://en.wikipedia.org/wiki/Hexagonal_chess).

For those who would like to play hexagonal chess:
  * For play against a computer player:
    * Jocly's [desktop app](https://github.com/mi-g/joclyboard/releases),
      or the web-based version of their hexagonal chess games, at these URLs:
      * Flat-top hexagon variants:
        * [Glinski chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=glinski-chess)
          (announced by Jocly on 2014-04-07)
        * [McCooey chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=mccooey-chess)
        * [Shafran chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=shafran-chess)
      * Flat-side hexagon variants:
        * [Brusky chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=brusky-chess)
        * [De Vasa chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=devasa-chess)
      * Note: The names of these variants are commonly expressed as possessives.
        For example, Glinski's chess, McCooey's chess, etc. I tend to drop the possessives
        for simplicity.
  * For either play against the computer or against live opponents around the world:
    * The [Ludii General Game System](https://ludii.games/), which supports those
      variants that Jocly supports (listed above), as well as:
      * Flat-top hexagon variants:
        * Mini Hexchess, a variant without Queens on a small (37-cell) board
        * Starchess, a variant that includes an initial phase in which the players
          place their pieces
      * Flat-side hexagon variant:
        * Wellisch chess, a 3-player variant.
          * This implementation differs from the standard, in that the game ends
            upon the capture of a King. (In the standard rules for Wellisch chess,
            the player whose King is captured loses, and play continues between
            the other two, with the King-capturing player taking control over the
            losing player's pieces.)
    * Omnichess at (omnichess.club)[https://www.omnichess.club/], which currently
      (as of August, 2021) supports Glinski chess and Wellisch Chess, but will likely
      soon host several more variants, since it hosts a "variant editor", which
      allows for the easy creation of new chess-like variants.
  * There is also a ["tabletop" version](https://steamcommunity.com/sharedfiles/filedetails/?id=262803646)
    available.

Implementation details:
  * Board instances owned by:
    * Server
    * Player
    * Board
  * Communication channels:
    * Server <--> Player
    * Player <--> GUI
  * Signals passed
    * Player to Server:
      * Action
    * Server to Player:
      * ActionRequest
      * BoardInitialization
      * Check
      * TODO: Checkmate
      * GameOutcome
    * GUI to Player
      * Action
    * Player to GUI
      * ActionRequest
      * BoardInitialiaztion
      * Check
      * GameOutcome

Those who are interested in contributing should see the [CONTIBUTING.md](./CONTRIBUTING.md) file.

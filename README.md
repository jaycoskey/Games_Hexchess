# Hexchess

This is a minimal implementation (in progress) of Glinski's Hexagonal Chess in C++, using Qt for GUI implementation.

The term "minimal" here means:
  * This is intended to be reasonably fast (using bitboards and various cached piece movement info), but not blazing.
  * The computer player (once implemented, using alpha-beta pruning) should be fairly free of blunders, but not expert.
  * The UI (once implemented) should be usable, but not polished or flashy.

Those who would like to learn more about hexagonal chess can get a decent introduction by reading
[the Wikipedia page on Hexagonal chess](https://en.wikipedia.org/wiki/Hexagonal_chess).

For those who would like to play hexagonal chess:
  * For play against a computer player, Jocly's [desktop app](https://github.com/mi-g/joclyboard/releases),
    or the web-based version of their hexagonal chess games, at these URLs:
    * Flat-top hexagon variants:
      * [Glinski chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=glinski-chess)
        (announced by Jocly on 2014-04-07)
      * [McCooey chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=mccooey-chess)
      * [Shafran chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=shafran-chess)
    * Flat-side hexagon variants:
      * [Brusky chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=brusky-chess)
      * [De Vasa chess](https://mi-g.github.io/jocly/examples/browser/control.html?game=devasa-chess)
    * Note: The names of these variants are commonly expressed as possessives. For example,
      Glinski's chess, McCooey's chess, Shafran's chess, etc.
  * For play against other opponents around the world:
    * The [Ludii General Game System](https://ludii.games/), which supports those variants that Jocly
      supports (listed above), as well as:
      * Flat-top hexagon variants:
        * Mini Hexchess, a variant without Queens on a small (37-cell) board
        * Starchess, a variant that includes an initial phase in which the players place their pieces
      * Flat-side hexagon variant:
        * Wellisch chess, a 3-player variant.
          * This implementation differs from the standard, in that the game ends upon the capture of a King.
            (In the standard rules for Wellisch chess, the player whose King is captured loses, and play
            continues between the other two, with the King-capturing player taking control over the losing
            player's pieces.)
  * There is also a ["tabletop" version](https://steamcommunity.com/sharedfiles/filedetails/?id=262803646)
    available.

Those who are interested in contributing should see the [CONTIBUTING.md](./CONTRIBUTING.md) file.

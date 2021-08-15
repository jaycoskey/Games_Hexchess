# Contributing to Hexchess

When you contribute code, you affirm that the contribution is your original work and that you
license the work to the project under the project's open source license. Whether or not you
state this explicitly, by submitting material via pull request, email, or other means,
you agree to license the material---whether it is copyrighted or not---under the project's
open source license and warrant that you have the legal authority to do so.

Categories of contributing, and some developer notes:
  * **Reporting issues.**
    * All are welcome to file issues on Github.
  * **Adding features.**
    * Adding variants.
      * This program was designed with extensibility in mind. It should be fairly
        straightforward to add other variants of hexagonal chess that use flat-top hexagons
        (e.g., McCooey chess, Shafran chess and Mini Hexchess). Variants of hexagonal chess
        that use flat-side hexagona (e.g., Brusky chess and De Vasa chess) would require
        more extensive changes.
      * Adding new variants requires adding a class named after the variant that encodes
        the board geometry, piece layout, piece movement details, etc., to variant.{h,cpp},
        plus several less major changes. At some point, the files variant.{h,cpp} will likely
        be split up into variant-specific files (e.g., variant_glinski.{h,cpp}).
      * The first time a variant is added (after Glinski, the first variant supported), the
        user will need to be offered the ability to choose which game to play.
    * Enabling tournament play among computer players.
        * This includes evaluation of match or tournament play, and ranking (e.g., z-score
          and possibly ELO computation).
    * Providing interoperability.
      * In time, hopefully other hexagon chess front-ends and back-ends will arise. Presumably
        these will communicate through an extended (?) version of the Universal Chess Interface (UCI).
  * **Improving skill level.**
    * One way to improve board value compuataion is to tune feature weights through playing
      computer vs computer games. The improvement should be statistically significant, with a
      z-score of at least 1.65, and hopefully 2.0. As much as possible, the outcomes of those
      games should be checked to ensure that blunders are not introduced (e.g., "sacrifices"
      without corresponding board position improvement).
      * Use of neural networks (NNs) might be added as an additional means of computer move
        determination. At that point, there will be many experiments possible involving the NN
        topology and weights, as well as feature input vectors.
    * This includes the computer player having a better idea of how long to spend on choosing
      a move during timed play, when to resign, and when to offer or accept a draw.
  * **Improving performance.**
      * This document assumes that Although performance is part of the design of Hexchess
        (bitfields, caching of move-related info), there is still much that can be done to improve
        performance. This could also include restricting some of the computations performed,
        which could impact the skill level of computer play.
  * **Improving usability and aesthetics.**
      * The GUI is very primitive. Lots of room for improvement.

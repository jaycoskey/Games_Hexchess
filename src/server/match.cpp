// Copyright (C) 2021, by Jay M. Coskey
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


namespace hexchess::core {

void Match::run() {
    for (Short gameNum = 0; gameNum < gameCount; ++gameNum) {
        Game<Glinski> game{};

        // Set up players
        if (gameNum % 2 == 0) {
            game.setPlayerBlack(player1);
            game.setPlayerWhite(player2);
        } else {
            game.setPlayerBlack(player2);
            game.setPlayerWhite(player1);
        }
        auto isWinnerPlayer1 = [](Short gameNum, const Outcome& outcome)
            {
                return ( (gameNum % 2 == 0 && outcome.winner == Color::Black)
                      || (ameNum % 2 == 1 && outcome.winner == Color::White) );
            };

        const Outcome& outcome = Game{player1, player2}.play();
        assert(outcome.termination != Termination::None);
        game_count += 1;
        if (isWin(outcome) {
            if (isWinnerPlayer1(gameNum, outcome)) {
                player1_wins += 1
                player2_losses += 1;

                player1_score += 1.0
            } else {
                player1_losses += 1;
                player2_wins += 1

                player2_score += 1.0
            }
        } else if (outcome.termination == Termination::Draw_Stalemate) {
            if (isWinnerPlayer1(gameNum, outcome) {
                // \todo Move variant-specific scoring to variant.{h,cpp}
                player1_stalemater += 1;

                player1_score += 0.75;
                player2_score += 0.25;
            } else {
                player1_stalemated += 1;

                player1_score += 0.25;
                player2_score += 0.75;
            }
        } else /* Non-stalemate Draw */ {
            player1_other_draws++;
            player1_score += 0.5;
            player2_score += 0.5;
        }
    }
};

std::string Match::match_summary_string() {
    std::ostringstream oss;
    oss << wPlayer().name() << " vs " << bPlayer().name()
        << " [" << game_count() << " games]: "
        << "Player1 (wins, stalematers #, stalemated #, other draws, losses) = "
        << "(" << player1_draws << ", "
               << player1_stalemater << ", "
               << player1_stalemated << ", "
               << player1_other_draws << ", "
               << player1_losses
        << ")";
    return oss.str();
}

}  // namespace hexchess::core



#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

class Decks {
public:
	Decks(int n) {
		this->num_decks = n;
		
		this->shuffle();
	}
	
	void shuffle() {
		cards.clear();
		for (int i = 0; i < this->num_decks; i++) {
			for (int j = 1; j <= 13; j++) {
				for (int k = 0; k < 4; k++) {
					int c = (j <= 10) ? j : 10;
					cards.push_back(c);
				}
			}
		}
		
		std::random_shuffle(cards.begin(), cards.end());
	}
	
	int getCard() {
		int r;
		if (this->num_decks > 0) {
			r = cards.back();
			cards.pop_back();
		}
		else {
			// TBD
		}
		return r;
	}
	
private:
	unsigned int num_decks;
	std::vector <int> cards;
};


enum PlayResult { gameLost, gameEquals, gameWon, gameBlackjack };

class GameLogic {
public:
	GameLogic(int num_decks) : game_deck(num_decks) {
		game_deck.shuffle();
	}
	
	void shuffle() {
		game_deck.shuffle();
	}
	
	// Play a game
	PlayResult play() {
		// Give a couple of cards to the player and the dealer
		std::vector <int> dealer_cards;
		std::vector <int> player_cards;
		
		player_cards.push_back(game_deck.getCard());
		player_cards.push_back(game_deck.getCard());
		
		dealer_cards.push_back(game_deck.getCard());
		dealer_cards.push_back(game_deck.getCard());
		
		// Call the player logic :)
		this->playerPlay(player_cards, dealer_cards, game_deck);
		
		// Dealer logic (soft)
		while (handMinValue(dealer_cards) < 17 && !isBlackjack(dealer_cards)) {
			dealer_cards.push_back(game_deck.getCard());
		}
		
		// Evaluate who won
		int dealer_value = handMaxValue(dealer_cards);
		int player_value = handMaxValue(player_cards);
		bool dealer_busted = handMaxValue(dealer_cards) > 21;
		bool player_busted   = handMaxValue(player_cards) > 21;
		
		if (player_busted) return gameLost;
		if (dealer_busted) {
			if (isBlackjack(player_cards))
				return gameBlackjack;
			else
				return gameWon;
		}else{
			if (dealer_value > player_value) {
				return gameLost;
			}
			else if (dealer_value == player_value) {
				if (isBlackjack(player_cards) == isBlackjack(dealer_cards))
					return gameEquals;
				else if (isBlackjack(player_cards))
					return gameWon;
				else
					return gameLost;
			}
			else {
				if (isBlackjack(player_cards))
					return gameBlackjack;
				else
					return gameWon;
			}
		}
	}
	
private:
	int handMinValue(const std::vector <int> & hand) {
		int r = 0;
		for (int i = 0; i < hand.size(); i++)
			r += hand[i];
		return r;
	}
	
	int handMaxValue(const std::vector <int> & hand) {
		int r = 0;
		for (int i = 0; i < hand.size(); i++) {
			if (hand[i] == 1) {
				if (r+11 < 21)
					r += 11;
				else
					r++;
			}
			else
				r += hand[i];
		}
		
		if (r > 21)
			return handMinValue(hand);
		return r;
	}

	bool isBlackjack(const std::vector <int> & hand) {
		if (hand.size() != 2)
			return false;
		//std::cout << hand[0] << " " << hand[1] << std::endl;
		return (hand[0] == 1 && hand[1] == 10) || (hand[0] == 10 && hand[1] == 1);
	}

	int handValue(const std::vector <int> & hand) {
		int rmax = 0;
		int rmin = 0;
		for (int i = 0; i < hand.size(); i++) {
			if (hand[i] != 1) {
				rmax += hand[i];
				rmin += hand[i];
			}
			else {
				if (rmax == rmin) {
					rmax += 11;
					rmin += 1;
				}
				else {
					rmax++;
					rmin++;
				}
			}
		}
	}
	
	void playerPlay(std::vector <int> & player_cards, const std::vector <int> & dealer_cards, Decks & game_deck) {
		int pol = 1;
		
		if (pol == 0) {
			while (handMaxValue(player_cards) <= 17 && !isBlackjack(player_cards)) {
				player_cards.push_back(game_deck.getCard());
			}
		}
		else if (pol == 1) {
			while (
					(
						handMaxValue(player_cards) <= 15 || 
						(handMaxValue(player_cards) == 18 && dealer_cards[0] < 9)
					)
					&& !isBlackjack(player_cards)
				) {
				
				player_cards.push_back(game_deck.getCard());
			}
		}
	}
	
	Decks game_deck;
};

int main(int argc, char ** argv) {
	int num_games = atoi(argv[1]);
	GameLogic gl(4);
	int res[4] = {0};
	
	for (int i = 0; i < num_games; i++) {
		// Shuffle every 10 games
		if (i % 10 == 0)
			gl.shuffle();
		
		PlayResult r = gl.play();
		res[(int)r]++;
	}

	const char * strs[] = {"Lost", "Equals", "Won", "BlackJack"};	
	for (int i = 0; i < 4; i++) {
		std::cout << std::string(strs[i]) << " " << 100.0f*res[i]/num_games << std::endl;
	}
}



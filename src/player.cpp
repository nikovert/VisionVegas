#include <player.hpp>
#include <carddetector.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cassert>

std::istream& operator>>(std::istream& in, card_value& cv)
{
	std::string str;
	in >> str;

	cv.type = number;
	cv.val = -1; // Invalidate

	try {
		cv.val = std::stoi(str);
	} catch (std::invalid_argument) {
		size_t pos[8];
		pos[0] = str.find('j');
		pos[1] = str.find('q');
		pos[2] = str.find('k');
		pos[3] = str.find('a');
		pos[4] = str.find('J');
		pos[5] = str.find('Q');
		pos[6] = str.find('K');
		pos[7] = str.find('A');
		if (pos[0] != std::string::npos || pos[1] != std::string::npos || pos[2] != std::string::npos || \
				pos[4] != std::string::npos || pos[5] != std::string::npos || pos[6] != std::string::npos) {
			cv.type = jqk;
			cv.val = 10;
		} else if (pos[3] != std::string::npos || pos[7] != std::string::npos) {
			cv.type = ace;
			cv.val = 1;
		}
	}

	return in;
}

std::ostream& operator<<(std::ostream& out, card_value& cv)
{
	if (cv.type == number)
		out << cv.val << std::endl;
	else if (cv.type == ace)
		out << "Ace\n";
	else out << "JQK\n";

	return out;
}

bool GameState::gotCard(card_value cv, WHOS_TURN wt)
{
	unsigned cv_t;
	if (cv.type == jqk && cv.val == 10) cv_t = 10;
	else if (cv.type == ace && cv.val == 1) cv_t = 0;
	else if (cv.type == number && cv.val >= 2 && cv.val <= 10) cv_t = cv.val;
	else return false;

	if (wt == BANK) {
		if (stack[cv_t]-- != 0) {
			bankScore += cv.val;
			if (cv.type == ace) bankAces++;
		} else return false;
	} else if (wt == PLAYER) {
		if (stack[cv_t]-- != 0) {
			myScore += cv.val;
			if (cv.type == ace) myAces++;
		} else return false;
	} else return false;

	return true;
}

static std::vector<card_value> bank_cards;
static std::vector<card_value> my_cards;

static decisionNode* pub_play(GameState state);
static float play(GameState state);
static float stand(GameState state);
static float hit(GameState state);
static float bank(GameState state);
static void probabilities(const GameState state, float* p);
static float endExpectedValue(const GameState state);

static decisionNode* pub_play(GameState state)
{
	return new decisionNode(state, stand(state), hit(state));
}

static float play(GameState state)
{
	if (state.myScore > 21) return -1.0;
	float s = stand(state);
	float h = hit(state);
	return (s > h) ? s : h;
}

static float stand(GameState state)
{
	if (state.myScore > 21) return -1.0;
	else return bank(state);
}

static float hit(GameState state)
{
	float p[c];
	probabilities(state, p);

	float expectedVal = 0;
	for (unsigned i = 0; i < c; i++) {
		if (state.stack[i] == 0) continue;
		expectedVal += p[i] * play(GameState(state,i,PLAYER));
	}

	return expectedVal;
}

static float bank(GameState state)
{
	if (state.bankScore >= bankStop) return endExpectedValue(state);

	// Else the bank hits
	float p[c];
	probabilities(state, p);

	float expectedVal = 0;
	for (unsigned i = 0; i < c; i++) {
		if (state.stack[i] == 0) continue;
		expectedVal += p[i] * bank(GameState(state,i,BANK));
	}

	return expectedVal;
}

static void probabilities(const GameState state, float* p)
{
	unsigned amountOfCards = 0;
	for (unsigned i = 0; i < c; i++) {
		amountOfCards += state.stack[i];
		p[i] = float(state.stack[i]);
	}
	for (unsigned i = 0; i < c; i++) {
		p[i] /= float(amountOfCards);
	}
}

static float endExpectedValue(const GameState state)
{
	if (state.myScore > 21) return -1.0;
	if (state.bankScore > 21) return 1.0;

	unsigned myScore = state.myScore;
	unsigned bankScore = state.bankScore;

	for (unsigned i = 0; i < state.myAces; i++) {
		if (myScore + 10 <= 21) myScore += 10;
		else break;
	}
	for (unsigned i = 0; i < state.bankAces; i++) {
		if (bankScore + 10 <= 21) bankScore += 10;
		else break;
	}

	if (myScore > bankScore) return 1.0;
	else if (myScore < bankScore) return -1.0;
	else return 0.0;
}

// Return true if we want a hit
bool _hit()
{
	int sum = 0;
	for (card_value cv : my_cards) {
		if (cv.type == ace) {
			if (sum > 10) sum++;
			else sum+=11;
		}
		else if (cv.type == jqk) sum+=10;
		else if (cv.val > 1 && cv.val < 11) sum+=cv.val;
	}

	if (sum <= 16) return true;
	else return false;
}

card_value getCard_notalk() {
	card_value cv;
	std::cin >> cv;
	return cv;
}

card_value get_card()
{
	std::cout << "Enter a card\n";
	return getCard_notalk();
}

// Return true if we want to play again
bool new_game()
{
	return false;
}

void player()
{
	card_value bankCard;
	GameState state;
	bool hitMe = true;
	std::cout << "Starting game. Please enter bank card value\n";
	std::cin >> bankCard;
	state.gotCard(bankCard, BANK);

	std::cout << "Please press enter to get player card\n";

	while (hitMe) {
		hitMe = false;
		std::cin.ignore(); // Continue on enter

		card_value myCard = getCard_notalk();
		state.gotCard(myCard, PLAYER);
		if (stand(state) < hit(state)) {
			hitMe = true;
		}

		std::cout << "current card value: " << myCard;
		std::cout << "accumulated card value: " << state.myScore << '\n';
		std::cout << "action: ";
		if (hitMe) std::cout << "hit\n";
		else std::cout << "stand\n";

	}

}

void simple_player()
{
	// Start a new game
	do {
		// Init in testing with get_card
		bank_cards.clear();
		my_cards.clear();

		bank_cards.push_back(get_card());
		my_cards.push_back(get_card());

		while(_hit()) {
			my_cards.push_back(get_card());
		}

		// Game is finished, bank shows what it has
		// Maybe implement, it isn't that important

		std::cout << "You got:\n";
		for (card_value cv : my_cards) {
			std::cout << cv;
		}

	} while(new_game());


}

void player_test()
{
	GameState state;

	std::cout << "Initial bank card,expected value on hit,expected value on stand\n";

	for (unsigned i = 0; i < c; i++) {
		GameState state1(state, i, BANK);
		decisionNode* d = pub_play(state1);
		std::cout << i << ',' << d->eh << ',' << d->es << std::endl;
		delete d;
	}

	/*std::cout << "playerScore, bankScore \n";
	std::cin >> state.myScore >> state.bankScore;
	std::cout << bank(state);*/
}

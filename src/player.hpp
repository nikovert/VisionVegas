#include <cassert>
#include <carddetector.hpp>

#ifndef __PLAYER__
#define __PLAYER__

const unsigned c = 11; // Amount of types of cards we destinct: A-10 seperate and J-K as one
const unsigned bankStop = 17;
const unsigned nrOfDecks = 1;

void player();

struct decisionNode;
struct branchNode;

enum WHOS_TURN {PLAYER,BANK};

struct GameState {
	unsigned bankScore; // Aces counted as 1
	unsigned myScore;
	unsigned stack[c]; // How many aces [0], 2s [1], 10 [9] J-K [10] are left
	unsigned myAces; // Amount of aces
	unsigned bankAces;
	unsigned unknownCardsPlayer;
	unsigned playerCardsAmount;
	bool standafter;

	GameState(const GameState& s) : bankScore(s.bankScore), myScore(s.myScore), myAces(s.myAces), bankAces(s.bankAces), unknownCardsPlayer(s.unknownCardsPlayer), standafter(s.standafter), playerCardsAmount(s.playerCardsAmount) {
		for (unsigned i = 0; i < c; i++) {
			stack[i] = s.stack[i];
		}
	}

	GameState(const GameState& s, unsigned j, WHOS_TURN w) : bankScore(s.bankScore), myScore(s.myScore), myAces(s.myAces), bankAces(s.bankAces), unknownCardsPlayer(s.unknownCardsPlayer), standafter(s.standafter), playerCardsAmount(s.playerCardsAmount) {
		for (unsigned i = 0; i < c; i++) {
			stack[i] = s.stack[i];
		}
		assert(stack[j] > 0 && j < c);
		stack[j]--;

		// j < c-1 means card is not jqk
		unsigned increase = (j < c-1) ? j+1 : j;
		if (w == PLAYER) {
			myScore += increase;
			if (j == 0) myAces++;
		} else if (w == BANK) {
			bankScore += increase;
			if (j == 0) bankAces++;
		} else assert(false);
	}

	GameState() : bankScore(0), myScore(0), myAces(0), bankAces(0), unknownCardsPlayer(0), standafter(false), playerCardsAmount(0) {
		for (unsigned i = 0; i < c; i++) {
			stack[i] = 4 * nrOfDecks; // A to 10
		}
		stack[c-1] = 12 * nrOfDecks; // J to K
	}

	bool gotCard(card_value cv, WHOS_TURN wt);
};

struct decisionNode {
	GameState state;
	// branchNode* b;
	float es; // Expected value at stand btw -1 and 1
	float eh; // Expected value at hit

	decisionNode(GameState _s, float _es, float _eh) :
		state(_s), es(_es), eh(_eh) {}
};

struct branchNode {
	float p[c]; // Probabilities of getting
	decisionNode d[c];
	float e;
};

#endif

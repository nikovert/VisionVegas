#include <player.hpp>
#include <carddetector.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

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
				pos[4] != std::string::npos || pos[5] != std::string::npos || pos[6] != std::string::npos)
			cv.type = jqk;
		else if (pos[3] != std::string::npos || pos[7] != std::string::npos)
			cv.type = ace;
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

static std::vector<card_value> bank_cards;
static std::vector<card_value> my_cards;

// Return true if we want a hit
bool hit()
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

card_value get_card()
{
	// Test function
	card_value cv;

	std::cout << "Enter a card\n";
	std::cin >> cv;
	return cv;
}

// Return true if we want to play again
bool new_game()
{
	return false;
}

void player()
{
	// Start a new game
	do {
		// Init in testing with get_card
		bank_cards.clear();
		my_cards.clear();

		bank_cards.push_back(get_card());
		my_cards.push_back(get_card());

		while(hit()) {
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

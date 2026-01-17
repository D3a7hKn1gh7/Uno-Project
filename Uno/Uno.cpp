/*
* Angel Pisarev group 2
* 3MI0600689
* GCC
* Main file for UNO card game implementation
*/

#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

// Constants
const int MAX_PLAYERS = 4;
const int MAX_DECK_SIZE = 108;
const int MAX_HAND_SIZE = 50;
const int STARTING_HAND_SIZE = 7;

// Card colors
const int COLOR_RED = 0;
const int COLOR_GREEN = 1;
const int COLOR_BLUE = 2;
const int COLOR_YELLOW = 3;
const int COLOR_WILD = 4;

// Card values
const int VALUE_SKIP = 10;
const int VALUE_REVERSE = 11;
const int VALUE_DRAW_TWO = 12;
const int VALUE_WILD = 13;
const int VALUE_WILD_DRAW_FOUR = 14;

// Card structure
struct Card {
    int color;
    int value;
};

// Player structure
struct Player {
    Card hand[MAX_HAND_SIZE];
    int cardCount;
    bool saidUno;
};

// GameState structure
struct GameState {
    Card deck[MAX_DECK_SIZE];
    int deckSize;
    Card discardPile[MAX_DECK_SIZE];
    int discardSize;
    Player players[MAX_PLAYERS];
    int numPlayers;
    int currentPlayer;
    bool clockwise;
};

// Convert color integer to character
char getColorChar(int color) {
    if (color == COLOR_RED) {
        return 'R';
    }
    if (color == COLOR_GREEN) {
        return 'G';
    }
    if (color == COLOR_BLUE) {
        return 'B';
    }
    if (color == COLOR_YELLOW) {
        return 'Y';
    }
    return 'W';
}

// Convert character to color integer
int getColorFromChar(char c) {
    if (c == 'R' || c == 'r') {
        return COLOR_RED;
    }
    if (c == 'G' || c == 'g') {
        return COLOR_GREEN;
    }
    if (c == 'B' || c == 'b') {
        return COLOR_BLUE;
    }
    if (c == 'Y' || c == 'y') {
        return COLOR_YELLOW;
    }
    return COLOR_WILD;
}

// Print a single card
void printCard(const Card& card) {
    cout << getColorChar(card.color);

    if (card.value <= 9) {
        cout << card.value;
    }
    else if (card.value == VALUE_SKIP) {
        cout << "Skip";
    }
    else if (card.value == VALUE_REVERSE) {
        cout << "Rev";
    }
    else if (card.value == VALUE_DRAW_TWO) {
        cout << "+2";
    }
    else if (card.value == VALUE_WILD) {
        cout << "ild";
    }
    else if (card.value == VALUE_WILD_DRAW_FOUR) {
        cout << "+4";
    }
}

// Initialize deck with all 108 UNO cards
void initializeDeck(GameState& game) {
    game.deckSize = 0;

    for (int color = COLOR_RED; color <= COLOR_YELLOW; color++) {
        game.deck[game.deckSize].color = color;
        game.deck[game.deckSize].value = 0;
        game.deckSize = game.deckSize + 1;

        for (int value = 1; value <= 9; value++) {
            for (int copy = 0; copy < 2; copy++) {
                game.deck[game.deckSize].color = color;
                game.deck[game.deckSize].value = value;
                game.deckSize = game.deckSize + 1;
            }
        }

        for (int copy = 0; copy < 2; copy++) {
            game.deck[game.deckSize].color = color;
            game.deck[game.deckSize].value = VALUE_SKIP;
            game.deckSize = game.deckSize + 1;
        }

        for (int copy = 0; copy < 2; copy++) {
            game.deck[game.deckSize].color = color;
            game.deck[game.deckSize].value = VALUE_REVERSE;
            game.deckSize = game.deckSize + 1;
        }

        for (int copy = 0; copy < 2; copy++) {
            game.deck[game.deckSize].color = color;
            game.deck[game.deckSize].value = VALUE_DRAW_TWO;
            game.deckSize = game.deckSize + 1;
        }
    }

    for (int i = 0; i < 4; i++) {
        game.deck[game.deckSize].color = COLOR_WILD;
        game.deck[game.deckSize].value = VALUE_WILD;
        game.deckSize = game.deckSize + 1;
    }

    for (int i = 0; i < 4; i++) {
        game.deck[game.deckSize].color = COLOR_WILD;
        game.deck[game.deckSize].value = VALUE_WILD_DRAW_FOUR;
        game.deckSize = game.deckSize + 1;
    }
}

// Shuffle the deck using random algorithm
void shuffleDeck(GameState& game) {
    random_device rd;
    mt19937 generator(rd());

    // Use standard library shuffle algorithm
    shuffle(game.deck, game.deck + game.deckSize, generator);
}

int main() {
    srand(time(0));

    GameState game;
    game.deckSize = 0;

    initializeDeck(game);

    cout << "Before shuffle - First 10 cards:\n";
    for (int i = 0; i < 10; i++) {
        printCard(game.deck[i]);
        cout << " ";
    }
    cout << "\n\n";

    shuffleDeck(game);

    cout << "After shuffle - First 10 cards:\n";
    for (int i = 0; i < 10; i++) {
        printCard(game.deck[i]);
        cout << " ";
    }
    cout << "\n";

    return 0;
}
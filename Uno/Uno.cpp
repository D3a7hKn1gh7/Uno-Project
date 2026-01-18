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

// Shuffle the deck
void shuffleDeck(GameState& game) {
    random_device rd;
    mt19937 generator(rd());
    shuffle(game.deck, game.deck + game.deckSize, generator);
}

// Deal starting cards to all players and set first discard card
void dealCards(GameState& game) {
    for (int p = 0; p < game.numPlayers; p++) {
        game.players[p].cardCount = 0;
        game.players[p].saidUno = false;

        for (int i = 0; i < STARTING_HAND_SIZE; i++) {
            if (game.deckSize > 0) {
                game.deckSize = game.deckSize - 1;
                game.players[p].hand[game.players[p].cardCount] = game.deck[game.deckSize];
                game.players[p].cardCount = game.players[p].cardCount + 1;
            }
        }
    }

    game.discardSize = 0;
    if (game.deckSize > 0) {
        game.deckSize = game.deckSize - 1;
        game.discardPile[game.discardSize] = game.deck[game.deckSize];
        game.discardSize = game.discardSize + 1;
    }
}

// Print all cards in a player's hand
void printPlayerHand(const GameState& game, int playerIndex) {
    cout << "Player " << (playerIndex + 1) << " - Your cards:\n";

    for (int i = 0; i < game.players[playerIndex].cardCount; i++) {
        cout << "[" << i << "] ";
        printCard(game.players[playerIndex].hand[i]);
        cout << " ";
    }
    cout << "\n";
}

// Check if a card can be played on top of another card
bool isValidPlay(const Card& card, const Card& topCard) {
    if (card.color == COLOR_WILD) {
        return true;
    }

    if (card.color == topCard.color) {
        return true;
    }

    if (card.value == topCard.value) {
        return true;
    }

    return false;
}

// Draw one card from deck for a player
void drawCard(GameState& game, int playerIndex) {
    // Check if deck is empty - reshuffle discard pile
    if (game.deckSize == 0) {
        cout << "Deck empty! Reshuffling discard pile...\n";

        // Move all cards except top card from discard to deck
        for (int i = 0; i < game.discardSize - 1; i++) {
            game.deck[game.deckSize] = game.discardPile[i];
            game.deckSize = game.deckSize + 1;
        }

        // Keep only top card in discard pile
        game.discardPile[0] = game.discardPile[game.discardSize - 1];
        game.discardSize = 1;

        // Shuffle the deck
        shuffleDeck(game);
    }

    // Draw card if deck has cards and player has space
    if (game.deckSize > 0 && game.players[playerIndex].cardCount < MAX_HAND_SIZE) {
        game.deckSize = game.deckSize - 1;
        game.players[playerIndex].hand[game.players[playerIndex].cardCount] = game.deck[game.deckSize];
        game.players[playerIndex].cardCount = game.players[playerIndex].cardCount + 1;

        cout << "Drew: ";
        printCard(game.players[playerIndex].hand[game.players[playerIndex].cardCount - 1]);
        cout << "\n";
    }
}

int main() {
    srand(time(0));

    GameState game;
    game.deckSize = 0;
    game.discardSize = 0;
    game.numPlayers = 2;

    initializeDeck(game);
    shuffleDeck(game);
    dealCards(game);

    cout << "=== Draw Card Test ===\n\n";
    cout << "Player 1 initial hand:\n";
    printPlayerHand(game, 0);

    cout << "\nDrawing 3 cards...\n";
    drawCard(game, 0);
    drawCard(game, 0);
    drawCard(game, 0);

    cout << "\nPlayer 1 hand after drawing:\n";
    printPlayerHand(game, 0);

    cout << "\nCards remaining in deck: " << game.deckSize << "\n";

    return 0;
}
/*
* Angel Pisarev group 2
* 3MI0600689
* GCC
* Main file for UNO card game implementation
*/

#include <iostream>

using namespace std;

// Constants
const int MAX_PLAYERS = 4;
const int MAX_DECK_SIZE = 108;
const int MAX_HAND_SIZE = 50;
const int STARTING_HAND_SIZE = 7;
const int CARDS_PER_COLOR = 25;
const int NUM_COLORS = 4;
const int NUM_WILD_CARDS = 4;

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

// Card structure - stores color and value of each card
struct Card {
    int color;  // 0=Red, 1=Green, 2=Blue, 3=Yellow, 4=Wild
    int value;  // 0-9 for numbers, 10=Skip, 11=Reverse, 12=+2, 13=Wild, 14=Wild+4
};

// Player structure - stores each player's hand and state
struct Player {
    Card hand[MAX_HAND_SIZE];  // Cards in player's hand
    int cardCount;              // Number of cards currently held
    bool saidUno;               // Whether player declared UNO
};

// GameState structure - stores all game data
struct GameState {
    Card deck[MAX_DECK_SIZE];           // Draw pile
    int deckSize;                        // Number of cards in deck
    Card discardPile[MAX_DECK_SIZE];    // Played cards pile
    int discardSize;                     // Number of cards in discard pile
    Player players[MAX_PLAYERS];         // All players
    int numPlayers;                      // Number of players in game
    int currentPlayer;                   // Index of current player
    bool clockwise;                      // Direction of play
};

int main() {
    GameState game;
    game.numPlayers = 0;
    game.deckSize = 0;
    game.discardSize = 0;
    game.currentPlayer = 0;
    game.clockwise = true;

    cout << "UNO Game - Game State Initialized\n";
    return 0;
}
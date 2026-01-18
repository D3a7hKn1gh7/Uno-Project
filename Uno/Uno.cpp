/*
* Angel Pisarev group 2
* 3MI0600689
* GCC
* Main file for UNO card game implementation
*/

#include <iostream>
#include <fstream>
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
    if (game.deckSize == 0) {
        cout << "Deck empty! Reshuffling discard pile...\n";

        for (int i = 0; i < game.discardSize - 1; i++) {
            game.deck[game.deckSize] = game.discardPile[i];
            game.deckSize = game.deckSize + 1;
        }

        game.discardPile[0] = game.discardPile[game.discardSize - 1];
        game.discardSize = 1;

        shuffleDeck(game);
    }

    if (game.deckSize > 0 && game.players[playerIndex].cardCount < MAX_HAND_SIZE) {
        game.deckSize = game.deckSize - 1;
        game.players[playerIndex].hand[game.players[playerIndex].cardCount] = game.deck[game.deckSize];
        game.players[playerIndex].cardCount = game.players[playerIndex].cardCount + 1;

        cout << "Drew: ";
        printCard(game.players[playerIndex].hand[game.players[playerIndex].cardCount - 1]);
        cout << "\n";
    }
}

// Get the index of the next player based on direction
int getNextPlayer(const GameState& game) {
    if (game.clockwise) {
        return (game.currentPlayer + 1) % game.numPlayers;
    }
    else {
        return (game.currentPlayer - 1 + game.numPlayers) % game.numPlayers;
    }
}

// Apply special card effects (Skip, Reverse, +2, Wild+4)
void applyCardEffect(GameState& game, const Card& card) {
    if (card.value == VALUE_SKIP) {
        cout << "Skip! Player " << (getNextPlayer(game) + 1) << " is skipped!\n";
        game.currentPlayer = getNextPlayer(game);
    }
    else if (card.value == VALUE_REVERSE) {
        game.clockwise = !game.clockwise;
        cout << "Reverse! Direction changed!\n";

        if (game.numPlayers == 2) {
            game.currentPlayer = getNextPlayer(game);
        }
    }
    else if (card.value == VALUE_DRAW_TWO) {
        int nextPlayer = getNextPlayer(game);
        cout << "Player " << (nextPlayer + 1) << " draws 2 cards!\n";

        drawCard(game, nextPlayer);
        drawCard(game, nextPlayer);
    }
    else if (card.value == VALUE_WILD_DRAW_FOUR) {
        int nextPlayer = getNextPlayer(game);
        cout << "Player " << (nextPlayer + 1) << " draws 4 cards!\n";

        for (int i = 0; i < 4; i++) {
            drawCard(game, nextPlayer);
        }
    }
}

// Play a card from player's hand
void playCard(GameState& game, int playerIndex, int cardIndex) {
    Card playedCard = game.players[playerIndex].hand[cardIndex];

    game.discardPile[game.discardSize] = playedCard;
    game.discardSize = game.discardSize + 1;

    for (int i = cardIndex; i < game.players[playerIndex].cardCount - 1; i++) {
        game.players[playerIndex].hand[i] = game.players[playerIndex].hand[i + 1];
    }
    game.players[playerIndex].cardCount = game.players[playerIndex].cardCount - 1;

    cout << "You played: ";
    printCard(playedCard);
    cout << "\n";

    if (playedCard.color == COLOR_WILD) {
        cout << "Choose new color (R/G/B/Y): ";
        char colorChoice;
        cin >> colorChoice;

        game.discardPile[game.discardSize - 1].color = getColorFromChar(colorChoice);
    }

    applyCardEffect(game, playedCard);

    game.players[playerIndex].saidUno = false;
}

// Safely read integer input with validation
bool readIntegerInput(int& result) {
    if (cin >> result) {
        return true;
    }
    else {
        cin.clear(); //clear the error state of cin
        cin.ignore(10000, '\n'); //ignore the invalid input in the buffer
        return false;
    }
}

void playerTurn(GameState& game) {
    int currentPlayerIdx = game.currentPlayer;
    Card topCard = game.discardPile[game.discardSize - 1];

    cout << "\n--- Player " << (currentPlayerIdx + 1) << "'s turn ---\n";
    cout << "Current card: ";
    printCard(topCard);
    cout << "\n\n";

    printPlayerHand(game, currentPlayerIdx);

    // Check if player has any valid cards
    bool hasValidCard = false;
    for (int i = 0; i < game.players[currentPlayerIdx].cardCount; i++) {
        if (isValidPlay(game.players[currentPlayerIdx].hand[i], topCard)) {
            hasValidCard = true;
            break;
        }
    }

    // No valid cards - must draw
    if (!hasValidCard) {
        cout << "No valid cards. Drawing from deck...\n";
        int oldCount = game.players[currentPlayerIdx].cardCount;
        drawCard(game, currentPlayerIdx);

        // Check if drawn card can be played
        if (game.players[currentPlayerIdx].cardCount > oldCount) {
            Card drawnCard = game.players[currentPlayerIdx].hand[game.players[currentPlayerIdx].cardCount - 1];

            if (isValidPlay(drawnCard, topCard)) {
                cout << "You can play this card. Play it? (1=Yes, 0=No): ";
                int choice;

                if (readIntegerInput(choice) && choice == 1) {
                    playCard(game, currentPlayerIdx, game.players[currentPlayerIdx].cardCount - 1);
                    return;
                }
            }
        }

        game.currentPlayer = getNextPlayer(game);
        return;
    }

    // Check if player has exactly 1 card - must declare UNO
    if (game.players[currentPlayerIdx].cardCount == 1) {
        cout << "You have 1 card left! Type 'uno' to declare UNO: ";
        char unoInput[10];
        cin >> unoInput;

        // Check if input is exactly "uno"
        bool isUno = (unoInput[0] == 'u' && unoInput[1] == 'n' && unoInput[2] == 'o' && unoInput[3] == '\0');

        if (isUno) {
            game.players[currentPlayerIdx].saidUno = true;
            cout << "UNO declared!\n";
        }
        else {
            cout << "You forgot to say UNO! Drawing penalty card...\n";
            drawCard(game, currentPlayerIdx);
        }
    }

    // Player has valid cards - let them choose
    cout << "Choose card index (or -1 to save game): ";
    int choice;

    // Save game option
    if (choice == -1) {
        saveGame(game);
        return;
    }

    if (!readIntegerInput(choice)) {
        cout << "Invalid input! Please enter a number.\n";
        return;
    }

    if (choice < 0 || choice >= game.players[currentPlayerIdx].cardCount) {
        cout << "Invalid choice!\n";
        return;
    }

    if (!isValidPlay(game.players[currentPlayerIdx].hand[choice], topCard)) {
        cout << "Invalid card! Choose again.\n";
        return;
    }

    playCard(game, currentPlayerIdx, choice);
    game.currentPlayer = getNextPlayer(game);
}

// Save current game state to file
void saveGame(const GameState& game) {
    ofstream file("uno_save.txt");
    
    if (!file.is_open()) {
        cout << "Error: Could not save game!\n";
        return;
    }
    
    // Save game state
    file << game.numPlayers << " " << game.currentPlayer << " " << game.clockwise << "\n";
    
    // Save deck
    file << game.deckSize << "\n";
    for (int i = 0; i < game.deckSize; i++) {
        file << game.deck[i].color << " " << game.deck[i].value << "\n";
    }
    
    // Save discard pile
    file << game.discardSize << "\n";
    for (int i = 0; i < game.discardSize; i++) {
        file << game.discardPile[i].color << " " << game.discardPile[i].value << "\n";
    }
    
    // Save players
    for (int p = 0; p < game.numPlayers; p++) {
        file << game.players[p].cardCount << " " << game.players[p].saidUno << "\n";
        
        for (int i = 0; i < game.players[p].cardCount; i++) {
            file << game.players[p].hand[i].color << " " << game.players[p].hand[i].value << "\n";
        }
    }
    
    file.close();
    cout << "Game saved successfully!\n";
}

int main() {
    srand(time(0));

    cout << "=== Input Validation Test ===\n\n";

    cout << "Enter an integer: ";
    int num;

    if (readIntegerInput(num)) {
        cout << "You entered: " << num << "\n";
    }
    else {
        cout << "Invalid input! Not an integer.\n";
    }

    cout << "\nEnter another integer: ";
    if (readIntegerInput(num)) {
        cout << "You entered: " << num << "\n";
    }
    else {
        cout << "Invalid input! Not an integer.\n";
    }

    return 0;
}
/*
* Angel Pisarev group 2
* 3MI0600689
* GCC
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

// Card structure - represents a single UNO card
struct Card {
    int color;  // 0=Red, 1=Green, 2=Blue, 3=Yellow, 4=Wild
    int value;  // 0-9 for numbers, 10=Skip, 11=Reverse, 12=+2, 13=Wild, 14=Wild+4
};

// Player structure - represents a single player's state
struct Player {
    Card hand[MAX_HAND_SIZE];  // Cards in player's hand
    int cardCount;              // Number of cards currently held
    bool saidUno;               // Whether player declared UNO
};

// GameState structure - represents the complete game state
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

// Function declarations
void initializeDeck(GameState& game);
void shuffleDeck(GameState& game);
void dealCards(GameState& game);
void printCard(const Card& card);
void printPlayerHand(const GameState& game, int playerIndex);
bool isValidPlay(const Card& card, const Card& topCard);
void drawCard(GameState& game, int playerIndex);
int getNextPlayer(const GameState& game);
void applyCardEffect(GameState& game, const Card& card);
void playCard(GameState& game, int playerIndex, int cardIndex);
bool readIntegerInput(int& result);
void playerTurn(GameState& game);
void saveGame(const GameState& game);
bool loadGame(GameState& game);
void playGame(GameState& game);
void displayMenu();
char getColorChar(int color);
int getColorFromChar(char c);

// Convert color integer to character representation
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

// Convert character to color integer (case insensitive)
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

// Print a single card to console
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

    // For each of the 4 colors (Red, Green, Blue, Yellow)
    for (int color = COLOR_RED; color <= COLOR_YELLOW; color++) {
        // Add one 0 card per color
        game.deck[game.deckSize].color = color;
        game.deck[game.deckSize].value = 0;
        game.deckSize = game.deckSize + 1;

        // Add two cards of each value 1-9
        for (int value = 1; value <= 9; value++) {
            for (int copy = 0; copy < 2; copy++) {
                game.deck[game.deckSize].color = color;
                game.deck[game.deckSize].value = value;
                game.deckSize = game.deckSize + 1;
            }
        }

        // Add two Skip cards per color
        for (int copy = 0; copy < 2; copy++) {
            game.deck[game.deckSize].color = color;
            game.deck[game.deckSize].value = VALUE_SKIP;
            game.deckSize = game.deckSize + 1;
        }

        // Add two Reverse cards per color
        for (int copy = 0; copy < 2; copy++) {
            game.deck[game.deckSize].color = color;
            game.deck[game.deckSize].value = VALUE_REVERSE;
            game.deckSize = game.deckSize + 1;
        }

        // Add two Draw Two cards per color
        for (int copy = 0; copy < 2; copy++) {
            game.deck[game.deckSize].color = color;
            game.deck[game.deckSize].value = VALUE_DRAW_TWO;
            game.deckSize = game.deckSize + 1;
        }
    }

    // Add 4 Wild cards
    for (int i = 0; i < 4; i++) {
        game.deck[game.deckSize].color = COLOR_WILD;
        game.deck[game.deckSize].value = VALUE_WILD;
        game.deckSize = game.deckSize + 1;
    }

    // Add 4 Wild Draw Four cards
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

// Deal starting cards to all players and set first discard card
void dealCards(GameState& game) {
    // Initialize all players
    for (int p = 0; p < game.numPlayers; p++) {
        game.players[p].cardCount = 0;
        game.players[p].saidUno = false;

        // Deal 7 cards to each player
        for (int i = 0; i < STARTING_HAND_SIZE; i++) {
            if (game.deckSize > 0) {
                game.deckSize = game.deckSize - 1;
                game.players[p].hand[game.players[p].cardCount] = game.deck[game.deckSize];
                game.players[p].cardCount = game.players[p].cardCount + 1;
            }
        }
    }

    // Place first card on discard pile
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
    // Wild cards can always be played
    if (card.color == COLOR_WILD) {
        return true;
    }

    // Card matches by color
    if (card.color == topCard.color) {
        return true;
    }

    // Card matches by value
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

        // In 2-player game, Reverse acts like Skip
        if (game.numPlayers == 2) {
            game.currentPlayer = getNextPlayer(game);
        }
    }
    else if (card.value == VALUE_DRAW_TWO) {
        int nextPlayer = getNextPlayer(game);
        cout << "Player " << (nextPlayer + 1) << " draws 2 cards!\n";

        drawCard(game, nextPlayer);
        drawCard(game, nextPlayer);

        //Player does NOT skip turn after drawing
    }
    else if (card.value == VALUE_WILD_DRAW_FOUR) {
        int nextPlayer = getNextPlayer(game);
        cout << "Player " << (nextPlayer + 1) << " draws 4 cards!\n";

        for (int i = 0; i < 4; i++) {
            drawCard(game, nextPlayer);
        }

        //Player does NOT skip turn after drawing
    }
}

// Play a card from player's hand
void playCard(GameState& game, int playerIndex, int cardIndex) {
    Card playedCard = game.players[playerIndex].hand[cardIndex];

    // Add card to discard pile
    game.discardPile[game.discardSize] = playedCard;
    game.discardSize = game.discardSize + 1;

    // Remove card from player's hand by shifting remaining cards
    for (int i = cardIndex; i < game.players[playerIndex].cardCount - 1; i++) {
        game.players[playerIndex].hand[i] = game.players[playerIndex].hand[i + 1];
    }
    game.players[playerIndex].cardCount = game.players[playerIndex].cardCount - 1;

    cout << "You played: ";
    printCard(playedCard);
    cout << "\n";

    // Handle Wild cards - player chooses new color
    if (playedCard.color == COLOR_WILD) {
        cout << "Choose new color (R/G/B/Y): ";
        char colorChoice;
        cin >> colorChoice;

        // Convert to color integer (case insensitive)
        game.discardPile[game.discardSize - 1].color = getColorFromChar(colorChoice);
    }

    // Apply special card effects
    applyCardEffect(game, playedCard);

    // Reset UNO declaration
    game.players[playerIndex].saidUno = false;
}

// Safely read integer input with validation
bool readIntegerInput(int& result) {
    if (cin >> result) {
        return true;
    }
    else {
        cin.clear(); // Clear the error state of cin
        cin.ignore(10000, '\n'); // Ignore the invalid input in the buffer
        return false;
    }
}

// Execute one player's turn
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

    if (!readIntegerInput(choice)) {
        cout << "Invalid input! Please enter a number.\n";
        return;
    }

    // Save game option
    if (choice == -1) {
        saveGame(game);
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

// Load game state from file
bool loadGame(GameState& game) {
    ifstream file("uno_save.txt");

    if (!file.is_open()) {
        cout << "No saved game found!\n";
        return false;
    }

    // Load game state
    file >> game.numPlayers >> game.currentPlayer >> game.clockwise;

    // Load deck
    file >> game.deckSize;
    for (int i = 0; i < game.deckSize; i++) {
        file >> game.deck[i].color >> game.deck[i].value;
    }

    // Load discard pile
    file >> game.discardSize;
    for (int i = 0; i < game.discardSize; i++) {
        file >> game.discardPile[i].color >> game.discardPile[i].value;
    }

    // Load players
    for (int p = 0; p < game.numPlayers; p++) {
        file >> game.players[p].cardCount >> game.players[p].saidUno;

        for (int i = 0; i < game.players[p].cardCount; i++) {
            file >> game.players[p].hand[i].color >> game.players[p].hand[i].value;
        }
    }

    file.close();
    cout << "Game loaded successfully!\n";
    return true;
}

// Main game loop
void playGame(GameState& game) {
    while (true) {
        // Check for winner
        for (int i = 0; i < game.numPlayers; i++) {
            if (game.players[i].cardCount == 0) {
                cout << "\n****************************************\n";
                cout << "*** Player " << (i + 1) << " wins the game! ***\n";
                cout << "****************************************\n";
                return;
            }
        }

        // Execute current player's turn
        playerTurn(game);
    }
}

// Display main menu
void displayMenu() {
    cout << "\n====================================\n";
    cout << "           UNO GAME\n";
    cout << "====================================\n";
    cout << "1. New Game\n";
    cout << "2. Continue Game\n";
    cout << "3. Exit\n";
    cout << "====================================\n";
    cout << "Choose option: ";
}

int main() {
    srand(time(0));

    while (true) {
        displayMenu();

        int choice;
        if (!readIntegerInput(choice)) {
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            // New game
            GameState game;
            game.deckSize = 0;
            game.discardSize = 0;
            game.currentPlayer = 0;
            game.clockwise = true;

            cout << "Enter number of players (2-4): ";
            if (!readIntegerInput(game.numPlayers)) {
                cout << "Invalid input!\n";
                continue;
            }

            if (game.numPlayers < 2 || game.numPlayers > MAX_PLAYERS) {
                cout << "Invalid number of players! Must be 2-4.\n";
                continue;
            }

            initializeDeck(game);
            shuffleDeck(game);
            dealCards(game);

            playGame(game);
        }
        else if (choice == 2) {
            // Continue game
            GameState game;
            if (loadGame(game)) {
                playGame(game);
            }
        }
        else if (choice == 3) {
            cout << "Thanks for playing UNO!\n";
            break;
        }
        else {
            cout << "Invalid option! Please choose 1, 2, or 3.\n";
        }
    }

    return 0;
}
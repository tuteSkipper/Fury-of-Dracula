// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
     
struct gameView {
    int roundNumber;                      // total number of rounds
    int turnNumber;                       // current turn number
    int score;                            // current score
    int HP[NUM_PLAYERS];                  // players' health points
    int trail[NUM_PLAYERS][TRAIL_SIZE];   // players' trails
    Map m;
};
     

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView view = malloc(sizeof(struct gameView));
    view->roundNumber = -1;
    view->turnNumber = -1;
    view->score = GAME_START_SCORE;

    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (i < PLAYER_DRACULA) {
            view->HP[i] = GAME_START_HUNTER_LIFE_POINTS;
        } else {
            view->HP[i] = GAME_START_BLOOD_POINTS;    
        }
    }

    for (i = 0; i < NUM_PLAYERS; i++) {
        for (int j = 0; j < TRAIL_SIZE; j++) {
            view->trail[i][j] = NOWHERE;
        }
    }
    
    view->m = newMap();

    int curr = 0;
    while (pastPlays[curr] != '\0') {
        if (curr > 0) {
            curr++; // to bring it to the correct character
        }
        view->turnNumber++;
        if (pastPlays[curr] == 'G') { // update round number
            view->roundNumber++;
            if (view->roundNumber > 0) {
                view->score--; // decrease score after Dracula's turn
            }
        }

        for (i = 0; i < TRAIL_SIZE; i++) { // put the move from the turn in the trail
            if (i < (TRAIL_SIZE-1)) {
                view->trail[(view->turnNumber)%5][i] = view->trail[(view->turnNumber)%5][i+1];
            } else {
                char place[2];
                place[0] = pastPlays[curr+1];
                place[1] = pastPlays[curr+2];
                view->trail[(view->turnNumber)%5][i] = abbrevToID(place);
            }
        }

        if ((pastPlays[curr] == 'G')||(pastPlays[curr] == 'S')||
            (pastPlays[curr] == 'H')||(pastPlays[curr] == 'M')) {
            for (i = (curr+3); i < (curr+7); i++) {
                if (pastPlays[i] == 'T') {
                    HP[(view->turnNumber)%5] -= LIFE_LOSS_TRAP_ENCOUNTER;
                } else if (pastPlays[i] == 'V') {
                    // no life lost, vampire vanquished
                } else if (pastPlays[i] == 'D') {
                    HP[(view->turnNumber)%5] -= LIFE_LOSS_DRACULA_ENCOUNTER;
                }
            }

            for (i = 0; i < TRAIL_SIZE; i++) {
                if (i < (TRAIL_SIZE-1)) { // check if hunter's stumbled on Drac's trail
                    if (view->trail[(view->turnNumber)%5][TRAIL_SIZE] == view->trail[PLAYER_DRACULA][i]) {

                    }
                } else {

                }
            }

        } else  { // Dracula's turn
            
        }





        curr += 7;     // gets to the space or NULL if no more plays
    }
    return view;
}
     
     
// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current score
int getScore(GameView currentView)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return 0;
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player){
    return currentView->location[player];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return NULL;
}

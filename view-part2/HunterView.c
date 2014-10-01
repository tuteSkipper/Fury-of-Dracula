// HunterView.c ... HunterView ADT implementation

#include <stdlib.h>
#include <assert.h>

#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Places.h"
// #include "Map.h" ... if you decide to use the Map ADT
     
struct hunterView {
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    
    int currentRound; // current round of the game
    int currentPlayer; //current player of the game
    int currentScore; // current score of the game
    int currentHP[NUM_PLAYERS]; // current hp of each player
    int currentPlayerLocation[NUM_PLAYERS]; //current player location
    int currentTrailHistory[NUM_PLAYERS][TRAIL_SIZE]; //current player trail history
    
};
     

// Creates a new HunterView to summarise the current state of the game
HunterView newHunterView(char *pastPlays, PlayerMessage messages[]) {
    HunterView hunterView = malloc(sizeof(struct hunterView));
    
    hunterView->currentRound = 0;
    hunterView->currentPlayer = 0;
    hunterView->currentScore = GAME_START_SCORE;
    
    hunterView->currentHP[0] = GAME_START_HUNTER_LIFE_POINTS;
    hunterView->currentHP[1] = GAME_START_HUNTER_LIFE_POINTS;
    hunterView->currentHP[2] = GAME_START_HUNTER_LIFE_POINTS;
    hunterView->currentHP[3] = GAME_START_HUNTER_LIFE_POINTS;
    hunterView->currentHP[4] = GAME_START_BLOOD_POINTS;
    
    int i;
    for ( i = 0; i < NUM_PLAYERS; i++) {
        hunterView->currentPlayerLocation[i] = UNKNOWN_LOCATION;
    }
    
    int j, k;
    for ( j = 0; j < NUM_PLAYERS ; j++){
        for ( k = 0; k < TRAIL_SIZE; k++) {
            hunterView->currentTrailHistory[j][k] = UNKNOWN_LOCATION;
        }
    }

    
    return hunterView;
}
     
     
// Frees all memory previously allocated for the HunterView toBeDeleted
void disposeHunterView(HunterView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(HunterView currentView) {
    return currentView->currentRound; //done
}

// Get the id of current player
PlayerID whoAmI(HunterView currentView) {
    return currentView->currentPlayer; //done
}

// Get the current score
int giveMeTheScore(HunterView currentView) {
    return currentView->totalScore; //done
}

// Get the current health points for a given player
int howHealthyIs(HunterView currentView, PlayerID player) {
    return currentView->currentHP[player]; //done
}

// Get the current location id of a given player
LocationID whereIs(HunterView currentView, PlayerID player) {
    return currentView->currentPlayerLocation[player]; //done
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(HunterView currentView, PlayerID player, LocationID trail[TRAIL_SIZE]) {
    int i;
    for ( i = 0; i < TRAIL_SIZE; i++){
        trail[i] = currentView->currentTrailHistory[player][i];
    }
} //done

//// Functions that query the map to find information about connectivity

// What are my possible next moves (locations)
LocationID *whereCanIgo(HunterView currentView, int *numLocations, int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return NULL;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(HunterView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return NULL;
}

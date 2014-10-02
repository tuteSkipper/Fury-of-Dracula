// HunterView.c ... HunterView ADT implementation

#include <stdlib.h>
#include <assert.h>

#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Places.h"
#include "Map.h"
     
struct hunterView {
    int roundNumber;                      // total number of rounds
    int turnNumber;                       // current turn number
    int score;                            // current score
    int HP[NUM_PLAYERS];                  // players' health points
    int trail[NUM_PLAYERS][TRAIL_SIZE];   // players' trails
    Map m;
};
     

// Creates a new HunterView to summarise the current state of the game
HunterView newHunterView(char *pastPlays, PlayerMessage messages[]) {
    HunterView hunterView = malloc(sizeof(struct hunterView));
    
    hunterView->roundNumber = -1;
    hunterView->turnNumber = -1;
    hunterView->score = GAME_START_SCORE;
    
    PlayerID playerID;
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (i < PLAYER_DRACULA) {
            hunterView->HP[i] = GAME_START_HUNTER_LIFE_POINTS;
        } else {
            hunterView->HP[i] = GAME_START_BLOOD_POINTS;    
        }
    }    
    
    // can't reveal Dracula trail to hunters ?
    for (i = 0; i < NUM_PLAYERS; i++) {
        for (int j = 0; j < TRAIL_SIZE; j++) {
            hunterView->trail[i][j] = NOWHERE;
        }
    }
    
    hunterView->m = newMap();
    
    int researchRecord[NUM_PLAYERS-1] = {0};
    int curr = 0;
    while (pastPlays[curr] != '\0') {
        if (curr > 0) {
            curr++; // to bring it to the correct character
        }
        hunterView->turnNumber++;
        if (pastPlays[curr] == 'G') { // update round number
            hunterView->roundNumber++;
            if (hunterView->roundNumber > 0) {
                hunterView->score--; // decrease score after Dracula's turn
            }
        }
        playerID = getCurrentPlayer(view);
        
        for (i = 0; i < TRAIL_SIZE; i++) { // put the move from the turn in the trail
            if (i < (TRAIL_SIZE-1)) {
                hunterView->trail[playerID][i] = hunterView->trail[playerID][i+1];
            } else {
                char place[2];
                place[0] = pastPlays[curr+1];
                place[1] = pastPlays[curr+2];
                hunterView->trail[playerID][i] = abbrevToID(place);
            }
        }
        
        if ((pastPlays[curr] == 'G')||(pastPlays[curr] == 'S')||
            (pastPlays[curr] == 'H')||(pastPlays[curr] == 'M')) {
            if (hunterView->HP[playerID] <= 0) {
                hunterView->score -= SCORE_LOSS_HUNTER_HOSPITAL;
                hunterView->HP[playerID] = 9;
            }
            
            for (i = (curr+3); i < (curr+7); i++) {
                if (pastPlays[i] == 'T') {
                    hunterView->HP[playerID] -= LIFE_LOSS_TRAP_ENCOUNTER;
                } else if (pastPlays[i] == 'V') {
                    // no life lost, vampire vanquished
                } else if (pastPlays[i] == 'D') {
                    hunterView->HP[playerID] -= LIFE_LOSS_DRACULA_ENCOUNTER;
                    hunterView->HP[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
                }
                    // yes??
                if (hunterView->HP[playerID] <= 0) {
                    hunterView->trail[playerID][TRAIL_SIZE-1] = ST_JOSEPH_AND_ST_MARYS;
                }
            }
            
            // for (i = 0; i < TRAIL_SIZE; i++) { //THIS PROBABLY GOES IN HUNTERhunterView
            // if (i < (TRAIL_SIZE-1)) { // check if hunter's stumbled on Drac's trail
            // if (hunterView->trail[playerID][TRAIL_SIZE-1] == hunterView->trail[PLAYER_DRACULA][i]) {
            
            // }
            // } else {
            
            // }
            // }
            
            if (hunterView->trail[playerID][TRAIL_SIZE-1] == hunterView->trail[playerID][TRAIL_SIZE-2]) { // same place as last round
                if (researchRecord[playerID] == 0) {
                    researchRecord[playerID]++;
                }
                hunterView->HP[playerID] += 3;
                if (hunterView->HP[playerID] > 9) {
                    hunterView->HP[playerID] = 9;
                }
            } else if (researchRecord[playerID] == 1) { // resting on more than one turn can't count towards research
                researchRecord[playerID]--;
            }
            
            if ((researchRecord[PLAYER_LORD_GODALMING] == 1)&&(researchRecord[PLAYER_DR_SEWARD] == 1)&&
                (researchRecord[PLAYER_VAN_HELSING] == 1)&&(researchRecord[PLAYER_MINA_HARKER] == 1)) {
                // all hunters have not moved for one go and researches
                int research = hunterView->trail[PLAYER_DRACULA][0]; // should this be an aspect of the hunterView struct or a static function????
            }
            
        } else  { // Dracula's turn
            if (pastPlays[curr+6] == 'V') {
                hunterView->score -= SCORE_LOSS_VAMPIRE_MATURES;
            }
        } // Not needed in HunterView
          // ^ Needed for score, though???
        
        curr += 7;     // gets to the space or NULL if no more plays
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
    return currentView->roundNumber;
}

// Get the id of current player
PlayerID whoAmI(HunterView currentView) {
    return (currentView->turnNumber % NUM_PLAYERS)-1;
}

// Get the current score
int giveMeTheScore(HunterView currentView) {
    return currentView->score;
}

// Get the current health points for a given player
int howHealthyIs(HunterView currentView, PlayerID player) {
    return currentView->HP[player];
}

// Get the current location id of a given player
LocationID whereIs(HunterView currentView, PlayerID player) {
    // need to adjust for trail array
    
    return currentView->currentPlayerLocation[player];
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

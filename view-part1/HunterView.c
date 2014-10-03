// HunterView.c ... HunterView ADT implementation

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

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
    GameView g;
};
     

// Creates a new HunterView to summarise the current state of the game
HunterView newHunterView(char *pastPlays, PlayerMessage messages[]) {
    HunterView view = malloc(sizeof(struct hunterView));
    
    view->roundNumber = 0;
    view->turnNumber = 0;
    view->score = GAME_START_SCORE;
    
    PlayerID playerID;

    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i < PLAYER_DRACULA) {
            view->HP[i] = GAME_START_HUNTER_LIFE_POINTS;
        } else {
            view->HP[i] = GAME_START_BLOOD_POINTS;    
        }
    }    
    
    int j;
    // can't reveal Dracula trail to hunters ?
    for (i = 0; i < NUM_PLAYERS; i++) {
        for (j = 0; j < TRAIL_SIZE; j++) {
            view->trail[i][j] = NOWHERE;
        }
    }
    
    view->m = newMap();
    view->g = newGameView(pastPlays, messages);

    int researchRecord[NUM_PLAYERS-1] = {0};
    int curr = 0;
    while (pastPlays[curr] != '\0') {
        if (curr > 0) {
            curr++; // to bring it to the correct character
        }
        view->turnNumber++;
        if (view->turnNumber % NUM_PLAYERS == PLAYER_DRACULA) {
            view->turnNumber++;
        }
        
        if (pastPlays[curr] == 'G') { // update round number
            view->roundNumber++;
            //printf ("roundNumber is %d\n", view->roundNumber);
            if (view->roundNumber > 0) {
                view->score--; // decrease score after Dracula's turn
            }
        }
        playerID = whoAmI(view);
        //printf ("playerID is %d\n", playerID);
        
        for (i = 0; i < TRAIL_SIZE; i++) { // put the move from the turn in the trail
            if (i < (TRAIL_SIZE-1)) {
                view->trail[playerID][i] = view->trail[playerID][i+1];
            } else {
                char place[3];
                place[0] = pastPlays[curr+1];
                place[1] = pastPlays[curr+2];
                place[2] = '\0';
                if (strcmp(place, "C?") == 0) {
                    view->trail[playerID][i] = CITY_UNKNOWN;
                } else if (strcmp(place, "S?") == 0) {
                    view->trail[playerID][i] = SEA_UNKNOWN;
                } else if (strcmp(place, "HI") == 0) {
                    view->trail[playerID][i] = HIDE;
                } else if (strcmp(place, "D1") == 0) {
                    view->trail[playerID][i] = DOUBLE_BACK_1;
                } else if (strcmp(place, "D2") == 0) {
                    view->trail[playerID][i] = DOUBLE_BACK_2;
                } else if (strcmp(place, "D3") == 0) {
                    view->trail[playerID][i] = DOUBLE_BACK_3;
                } else if (strcmp(place, "D4") == 0) {
                    view->trail[playerID][i] = DOUBLE_BACK_4;
                } else if (strcmp(place, "D5") == 0) {
                    view->trail[playerID][i] = DOUBLE_BACK_5;
                } else if (strcmp(place, "TP") == 0) {
                    view->trail[playerID][i] = CITY_UNKNOWN;
                } else {
                    view->trail[playerID][i] = abbrevToID(place);
                }
            }
        }
        
        if ((pastPlays[curr] == 'G')||(pastPlays[curr] == 'S')||
            (pastPlays[curr] == 'H')||(pastPlays[curr] == 'M')) {
            if (view->HP[playerID] <= 0) {
                view->score -= SCORE_LOSS_HUNTER_HOSPITAL;
                view->HP[playerID] = 9;
            }
            
            for (i = (curr+3); i < (curr+7); i++) {
                if (pastPlays[i] == 'T') {
                    view->HP[playerID] -= LIFE_LOSS_TRAP_ENCOUNTER;
                } else if (pastPlays[i] == 'V') {
                    // no life lost, vampire vanquished
                } else if (pastPlays[i] == 'D') {
                    view->HP[playerID] -= LIFE_LOSS_DRACULA_ENCOUNTER;
                    view->HP[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
                }
                    // yes??
                if (view->HP[playerID] <= 0) {
                    view->trail[playerID][TRAIL_SIZE-1] = ST_JOSEPH_AND_ST_MARYS;
                }
            }
            
            // for (i = 0; i < TRAIL_SIZE; i++) { //THIS PROBABLY GOES IN HUNTERview
            // if (i < (TRAIL_SIZE-1)) { // check if hunter's stumbled on Drac's trail
            // if (view->trail[playerID][TRAIL_SIZE-1] == view->trail[PLAYER_DRACULA][i]) {
            
            // }
            // } else {
            
            // }
            // }
            
            if (view->trail[playerID][TRAIL_SIZE-1] == view->trail[playerID][TRAIL_SIZE-2]) { // same place as last round
                if (researchRecord[playerID] == 0) {
                    researchRecord[playerID]++;
                }
                view->HP[playerID] += 3;
                if (view->HP[playerID] > 9) {
                    view->HP[playerID] = 9;
                }
            } else if (researchRecord[playerID] == 1) { // resting on more than one turn can't count towards research
                researchRecord[playerID]--;
            }
            
            // if ((researchRecord[PLAYER_LORD_GODALMING] == 1)&&(researchRecord[PLAYER_DR_SEWARD] == 1)&&
                // (researchRecord[PLAYER_VAN_HELSING] == 1)&&(researchRecord[PLAYER_MINA_HARKER] == 1)) {
                // all hunters have not moved for one go and researches
                // int research = view->trail[PLAYER_DRACULA][0]; // should this be an aspect of the view struct or a static function????
                // research = research; // to make it compile
            // }
            
        } else  { // Dracula's turn
            if (pastPlays[curr+6] == 'V') {
                view->score -= SCORE_LOSS_VAMPIRE_MATURES;
            }

            if (view->trail[playerID][0] != NOWHERE) {
               if (view->trail[playerID][0] < NUM_MAP_LOCATIONS) {
                  if (idToType(view->trail[playerID][0]) == 2) {
                     view->HP[playerID] -= LIFE_LOSS_SEA;
                  }
               } else if (view->trail[playerID][0] == SEA_UNKNOWN) {
                  view->HP[playerID] -= LIFE_LOSS_SEA;
               }
            }
        } // Not needed in HunterView
          // ^ Needed for score, though???
        
        curr += 7;     // gets to the space or NULL if no more plays
    }
    
    return view;
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
   PlayerID player = currentView->turnNumber % NUM_PLAYERS;
   if (player == 0) {
      player = PLAYER_DRACULA;
   } else {
      player -= 1;
   }
    
   if (currentView->turnNumber == 0) {
      player = PLAYER_LORD_GODALMING;
   }
   return (player);
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
    return currentView->trail[player][0];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(HunterView currentView, PlayerID player, LocationID trail[TRAIL_SIZE]) {
    int i;
    int j;
    for (i =0, j = TRAIL_SIZE-1 ; i < TRAIL_SIZE; i++, j--) {
        trail[i] = currentView->trail[player][j];
    }
} //done

//// Functions that query the map to find information about connectivity

// What are my possible next moves (locations)
LocationID *whereCanIgo(HunterView currentView, int *numLocations, int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    LocationID *locations;
    PlayerID me = whoAmI(currentView);
    LocationID currLocation = whereIs(currentView, me);
    locations = connectedLocations(currentView->g, numLocations, currLocation,
                                   me, currentView->roundNumber, road, rail, sea);
    return locations;
}

// What are the specified player's next possible moves
LocationID *whereCanTheyGo(HunterView currentView, int *numLocations,
                           PlayerID player, int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    LocationID *locations;
    LocationID currLocation = whereIs(currentView, player);
    locations = connectedLocations(currentView->g, numLocations, currLocation,
                                   player, currentView->roundNumber, road, rail, sea);
    return locations;
}

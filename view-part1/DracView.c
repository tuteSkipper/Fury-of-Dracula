// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "DracView.h"
#include "Map.h"
    
struct dracView {
   int roundNumber;                      // total number of rounds
   int turnNumber;                       // current turn number
   int score;                            // current score
   int HP[NUM_PLAYERS];                  // players' health points
   int trail[NUM_PLAYERS][TRAIL_SIZE];   // players' trails
   Map m;
   GameView g;
};


// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
   DracView view = malloc(sizeof(struct dracView));
   view->roundNumber = -1;
   view->turnNumber = 0;
   view->score = GAME_START_SCORE;

   int i;
   for (i = 0; i < NUM_PLAYERS; i++) {
      if (i < PLAYER_DRACULA) {
         view->HP[i] = GAME_START_HUNTER_LIFE_POINTS;
      } else {
         view->HP[i] = GAME_START_BLOOD_POINTS;    
      }
   }

   int j;
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
      if (pastPlays[curr] == 'G') { // update round number
         view->roundNumber++;
         if (view->roundNumber > 0) {
            view->score--; // decrease score after Dracula's turn
         }
      }

      PlayerID currPlayer = view->turnNumber % NUM_PLAYERS;
      //printf("->turn num: %d\n",currentView->turnNumber);
      if(view->turnNumber == 0 || view->turnNumber == 1){
         currPlayer = PLAYER_LORD_GODALMING;
      } else if(currPlayer == 0){
         currPlayer = PLAYER_DRACULA;
      } else {
         currPlayer -= 1;
      }

      for (i = 0; i < TRAIL_SIZE; i++) { // put the move from the turn in the trail
         if (i < (TRAIL_SIZE-1)) {
            view->trail[currPlayer][i] = view->trail[currPlayer][i+1];
         } else {
            char place[3];
             place[0] = pastPlays[curr+1];
             place[1] = pastPlays[curr+2];
             place[2] = '\0';
             if (strcmp(place, "C?") == 0) {
                 view->trail[currPlayer][i] = CITY_UNKNOWN;
             } else if (strcmp(place, "S?") == 0) {
                 view->trail[currPlayer][i] = SEA_UNKNOWN;
             } else if (strcmp(place, "HI") == 0) {
                 view->trail[currPlayer][i] = HIDE;
             } else if (strcmp(place, "D1") == 0) {
                 view->trail[currPlayer][i] = DOUBLE_BACK_1;
             } else if (strcmp(place, "D2") == 0) {
                 view->trail[currPlayer][i] = DOUBLE_BACK_2;
             } else if (strcmp(place, "D3") == 0) {
                 view->trail[currPlayer][i] = DOUBLE_BACK_3;
             } else if (strcmp(place, "D4") == 0) {
                 view->trail[currPlayer][i] = DOUBLE_BACK_4;
             } else if (strcmp(place, "D5") == 0) {
                 view->trail[currPlayer][i] = DOUBLE_BACK_5;
             } else if (strcmp(place, "TP") == 0) {
                 view->trail[currPlayer][i] = CITY_UNKNOWN;
             } else {
                 view->trail[currPlayer][i] = abbrevToID(place);
             }
         }
      }

      if ((pastPlays[curr] == 'G')||(pastPlays[curr] == 'S')||
         (pastPlays[curr] == 'H')||(pastPlays[curr] == 'M')) {
         if (view->HP[currPlayer] <= 0) {
            view->score -= SCORE_LOSS_HUNTER_HOSPITAL;
            view->HP[currPlayer] = 9;
         }

         for (i = (curr+3); i < (curr+7); i++) {
            if (pastPlays[i] == 'T') {
               view->HP[currPlayer] -= LIFE_LOSS_TRAP_ENCOUNTER;
            } else if (pastPlays[i] == 'V') {
               // no life lost, vampire vanquished
            } else if (pastPlays[i] == 'D') {
               view->HP[currPlayer] -= LIFE_LOSS_DRACULA_ENCOUNTER;
               view->HP[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
            }
         }

         if (view->trail[currPlayer][TRAIL_SIZE-1] == view->trail[currPlayer][TRAIL_SIZE-2]) { // same place as last round
            if (researchRecord[currPlayer] == 0) {
               researchRecord[currPlayer]++;
            }
            view->HP[currPlayer] += 3;
            while (view->HP[currPlayer] > 9) {
               view->HP[currPlayer]--;
            }
         } else if (researchRecord[currPlayer] == 1) {
            researchRecord[currPlayer]--;
         }

      } else  { // Dracula's turn
         if (pastPlays[curr+6] == 'V') {
            view->score -= SCORE_LOSS_VAMPIRE_MATURES;
         }            
      }

      curr += 7;     // gets to the space or NULL if no more plays
   }
   return view;
}
    
    
// Frees all memory previously allocated for the DracView toBeDeleted
void disposeDracView(DracView toBeDeleted)
{
   //COMPLETE THIS IMPLEMENTATION
   free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round giveMeTheRound(DracView currentView)
{
   return currentView->roundNumber;
}

// Get the current score
int giveMeTheScore(DracView currentView)
{
   return currentView->score;
}

// Get the current health points for a given player
int howHealthyIs(DracView currentView, PlayerID player)
{
   return currentView->HP[player];
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player)
{
   //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
   return 0;
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
             LocationID *start, LocationID *end)
{
   //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
   return;
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where,
                   int *numTraps, int *numVamps)
{
   //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
   return;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player,
                     LocationID trail[TRAIL_SIZE])
{
   //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
}

//// Functions that query the map to find information about connectivity

// What are my (Dracula's) possible next moves (locations)
LocationID *whereCanIgo(DracView currentView, int *numLocations, int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    LocationID *locations = malloc(NUM_MAP_LOCATIONS*sizeof(int));
    LocationID currLocation = whereIs(currentView, PLAYER_DRACULA);
    locations = connectedLocations(currentView->g, numLocations, currLocation,
                                    PLAYER_DRACULA, currentView->roundNumber, road, rail, sea);
    return locations;
}
                               
// What are the specified player's next possible moves
LocationID *whereCanTheyGo(DracView currentView, int *numLocations,
                            PlayerID player, int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    LocationID *locations = malloc(NUM_MAP_LOCATIONS*sizeof(int));
    LocationID currLocation = whereIs(currentView, player);
    locations = connectedLocations(currentView->g, numLocations, currLocation,
                                    player, currentView->roundNumber, road, rail, sea);
    return locations;
}


// DracView.c ... DracView ADT implementation

#include <stdlib.h>
#include <stdio.h>
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
   int whatIsThere[NUM_MAP_LOCATIONS][2];
   Map m;
   GameView g;
};


// Creates a new DracView to summarise the current state of the game
DracView newDracView(char *pastPlays, PlayerMessage messages[])
{
   DracView view = malloc(sizeof(struct dracView));
   view->roundNumber = 0;
   view->turnNumber = 1;
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

      PlayerID currPlayer = view->turnNumber % NUM_PLAYERS;
      printf("->turn num: %d\n", view->turnNumber);
      if(view->turnNumber == 0 || view->turnNumber == 1){
         currPlayer = PLAYER_LORD_GODALMING;
      } else if(currPlayer == 0){
         currPlayer = PLAYER_DRACULA;
      } else {
         currPlayer -= 1;
      }

      for (i = TRAIL_SIZE-1; i >= 0; i--) { // put the move from the turn in the trail
         // printf("%d\n", currPlayer);
         if (i > 0) {
            view->trail[currPlayer][i] = view->trail[currPlayer][i-1];
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
               view->trail[currPlayer][i] = TELEPORT;
            } else {
               printf("%d %s %d\n", i, place, currPlayer);
               view->trail[currPlayer][i] = abbrevToID(place);
               printf("%d\n", view->trail[currPlayer][i]);
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
               printf("%d %d\n", currPlayer, view->HP[currPlayer]);
               view->HP[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
            }
         }

         if (view->trail[currPlayer][0] == view->trail[currPlayer][1]) { // same place as last round
            if (researchRecord[currPlayer] == 0) {
               researchRecord[currPlayer]++;
            }
            view->HP[currPlayer] += 3;
            printf("dsjaod: %d %d\n", currPlayer, view->HP[currPlayer]);
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

         if (view->trail[currPlayer][0] == TELEPORT) {
            view->HP[currPlayer] += LIFE_GAIN_CASTLE_DRACULA;
         }

         if (view->trail[currPlayer][0] != NOWHERE) {
            if (view->trail[currPlayer][0] < NUM_MAP_LOCATIONS) {
               if (idToType(view->trail[currPlayer][0]) == 2) {
                  view->HP[currPlayer] -= LIFE_LOSS_SEA;
               }
            } else if (view->trail[currPlayer][0] == SEA_UNKNOWN) {
               view->HP[currPlayer] -= LIFE_LOSS_SEA;
            } else if (view->trail[currPlayer][0] == DOUBLE_BACK_1) {
               if (view->trail[currPlayer][1] != NOWHERE) {
                  if (view->trail[currPlayer][1] < NUM_MAP_LOCATIONS) {
                     if (idToType(view->trail[currPlayer][1]) == 2) {
                        view->HP[currPlayer] -= LIFE_LOSS_SEA;
                     }
                  } else if (view->trail[currPlayer][1] == SEA_UNKNOWN) {
                     view->HP[currPlayer] -= LIFE_LOSS_SEA;
                  }
               }
            } else if (view->trail[currPlayer][0] == DOUBLE_BACK_2) {
               if (view->trail[currPlayer][1] != NOWHERE) {
                  if (view->trail[currPlayer][2] < NUM_MAP_LOCATIONS) {
                     if (idToType(view->trail[currPlayer][2]) == 2) {
                        view->HP[currPlayer] -= LIFE_LOSS_SEA;
                     }
                  } else if (view->trail[currPlayer][2] == SEA_UNKNOWN) {
                     view->HP[currPlayer] -= LIFE_LOSS_SEA;
                  }
               }
            } else if (view->trail[currPlayer][0] == DOUBLE_BACK_3) {
               if (view->trail[currPlayer][1] != NOWHERE) {
                  if (view->trail[currPlayer][3] < NUM_MAP_LOCATIONS) {
                     if (idToType(view->trail[currPlayer][3]) == 2) {
                        view->HP[currPlayer] -= LIFE_LOSS_SEA;
                     }
                  } else if (view->trail[currPlayer][3] == SEA_UNKNOWN) {
                     view->HP[currPlayer] -= LIFE_LOSS_SEA;
                  }
               }
            } else if (view->trail[currPlayer][0] == DOUBLE_BACK_4) {
               if (view->trail[currPlayer][1] != NOWHERE) {
                  if (view->trail[currPlayer][4] < NUM_MAP_LOCATIONS) {
                     if (idToType(view->trail[currPlayer][4]) == 2) {
                        view->HP[currPlayer] -= LIFE_LOSS_SEA;
                     }
                  } else if (view->trail[currPlayer][4] == SEA_UNKNOWN) {
                     view->HP[currPlayer] -= LIFE_LOSS_SEA;
                  }
               }
            } else if (view->trail[currPlayer][0] == DOUBLE_BACK_5) {
               if (view->trail[currPlayer][1] != NOWHERE) {
                  if (view->trail[currPlayer][5] < NUM_MAP_LOCATIONS) {
                     if (idToType(view->trail[currPlayer][5]) == 2) {
                        view->HP[currPlayer] -= LIFE_LOSS_SEA;
                     }
                  } else if (view->trail[currPlayer][5] == SEA_UNKNOWN) {
                     view->HP[currPlayer] -= LIFE_LOSS_SEA;
                  }
               }
            }
         }
      }

      view->turnNumber++;
      if (pastPlays[curr] == 'D') { // update round number
         view->roundNumber++;
         view->score--; // decrease score after Dracula's turn
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
   printf("%d %d\n", player, currentView->HP[player]);
   return currentView->HP[player];
}

// Get the current location id of a given player
LocationID whereIs(DracView currentView, PlayerID player) {
   if (currentView->trail[player][0] == TELEPORT) {
      return CASTLE_DRACULA;
    }
   return currentView->trail[player][0];
}

// Get the most recent move of a given player
void lastMove(DracView currentView, PlayerID player,
             LocationID *start, LocationID *end)
{
   (*start) = currentView->trail[player][1];
   (*end) = currentView->trail[player][0];
   
   printf ("Location ID  start is %d\n", (*start));
   printf ("Location ID end is %d\n", (*end));
    
    
   return;
}

// Find out what minions are placed at the specified location
void whatsThere(DracView currentView, LocationID where, int *numTraps, int *numVamps){

   //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    if (idToType(where) == SEA || idToType(where) == NOWHERE){
        numTraps = 0;
        numVamps = 0;
    } else {
        numTraps = currentView->whatIsThere[where][0];
        numVamps = currentView->whatIsThere[where][1];
    }
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void giveMeTheTrail(DracView currentView, PlayerID player, LocationID trail[TRAIL_SIZE]){
    int i;
    int j;
    for (i =0, j = 0 ; i < TRAIL_SIZE; i++, j++) {
        trail[i] = currentView->trail[player][j];
    }
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


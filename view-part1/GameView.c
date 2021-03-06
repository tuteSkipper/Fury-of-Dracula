// GameView.c ... GameView ADT implementation

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
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
    //int playerLocation[NUM_PLAYERS];      // players' locations
    Map m;
};


// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    GameView view = malloc(sizeof(struct gameView));
    view->roundNumber = 0;
    view->turnNumber = 1;
    view->score = GAME_START_SCORE;
    
    PlayerID currPlayer;
    
    int i;
    for (i = 0; i < NUM_PLAYERS; i++) {
        if (i < PLAYER_DRACULA) {
            view->HP[i] = GAME_START_HUNTER_LIFE_POINTS;
            //view->playerLocation[i] = UNKNOWN_LOCATION;
        } else {
            view->HP[i] = GAME_START_BLOOD_POINTS;
            //view->playerLocation[i] = UNKNOWN_LOCATION;
        }
    }
    
    int j;
    for (i = 0; i < NUM_PLAYERS; i++) {
        for (j = 0; j < TRAIL_SIZE; j++) {
            view->trail[i][j] = NOWHERE;
        }
    }
    
    view->m = newMap();
    
    int researchRecord[NUM_PLAYERS-1] = {0};
    int curr = 0;
    while (pastPlays[curr] != '\0') {
        if (curr > 0) {
            curr++; // to bring it to the correct character
        }
        
        
        currPlayer = getCurrentPlayer(view);
        //printf("out-> turn %d\n",(view->turnNumber));
        //printf("curr player %d\n",getCurrentPlayer(view));
        
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
                    view->trail[currPlayer][i] = abbrevToID(place);
                }
            }
        }
        
        if ((pastPlays[curr] == 'G')||(pastPlays[curr] == 'S')||
            (pastPlays[curr] == 'H')||(pastPlays[curr] == 'M')) {
            if (view->HP[currPlayer] <= 0) {
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
                
                if (view->HP[currPlayer] <= 0) {
                    view->score -= SCORE_LOSS_HUNTER_HOSPITAL;
                }
                
                // if (view->HP[(view->turnNumber)%5] <= 0) { //HUNTERVIEW?
                // view->trail
                // }
            }
            
            // for (i = 0; i < TRAIL_SIZE; i++) { //THIS PROBABLY GOES IN HUNTERVIEW
            // if (i < (TRAIL_SIZE-1)) { // check if hunter's stumbled on Drac's trail
            // if (view->trail[(view->turnNumber)%5][TRAIL_SIZE-1] == view->trail[PLAYER_DRACULA][i]) {
            
            // }
            // } else {
            
            // }
            // }
            
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
            
            // if ((researchRecord[PLAYER_LORD_GODALMING] == 1)&&
            // (researchRecord[PLAYER_DR_SEWARD] == 1)&&
            // (researchRecord[PLAYER_VAN_HELSING] == 1)&&
            // (researchRecord[PLAYER_MINA_HARKER] == 1)) { // all hunters have not moved for one turn
            // should probably go in hunterview
            // SHOW THE LAST MOVE IN DRAC TRAIL
            // }
            
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
    return currentView->roundNumber;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView) {
    PlayerID player = currentView->turnNumber % NUM_PLAYERS;
    //printf("->turn num: %d\n",currentView->turnNumber);
    if(currentView->turnNumber == 0 || currentView->turnNumber == 1){
        player = PLAYER_LORD_GODALMING;
    } else if(player == 0){
        player = PLAYER_DRACULA;
    } else {
        player -= 1;
    }
    return (player);
}

// Get the current score
int getScore(GameView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    return currentView->HP[player];
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player) {
    // int i=0,j=0;
    // for (i=0; i < NUM_PLAYERS; i++) {
    //printf("player%d:",i);
    // for (j = 0 ; j < TRAIL_SIZE ; j++) {
    //printf(" %d ",currentView->trail[i][j]);
    // }
    //printf("\n");
    // }
    // int x=0;
    // while (x < TRAIL_SIZE-1){
    // x++;
    // }
    //   printf("%d , %d\n",player, x);
    if (currentView->trail[player][0] == TELEPORT) {
        return CASTLE_DRACULA;
    }
    return currentView->trail[player][0]; //playerLocation[player];
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player, LocationID trail[TRAIL_SIZE]) {
    int i;
    int j;
    for (i =0, j = 0 ; i < TRAIL_SIZE; i++, j++) {
        trail[i] = currentView->trail[player][j];
    }
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    LocationID *locations = malloc(NUM_MAP_LOCATIONS*sizeof(int));
    int *visited = calloc (currentView->m->nV, sizeof(int));
    int i = 0;
    int counter = 1;
    locations[i] = from;
    visited[from] = 1;
    i++;
    counter++;
    VList curr;
    if (player != PLAYER_DRACULA) {
        if (rail == TRUE) {
            int railLength = round+player;
            if (railLength%4 == 1) {
                for (curr = currentView->m->connections[from]; curr != NULL; curr = curr->next) {
                    if (curr->type == RAIL  && visited[curr->v] == 0) {
                        locations[i] = curr->v;
                        visited[curr->v] = counter;
                        i++;
                        counter++;
                    }
                }
            } else if (railLength%4 == 2) {
                VList second;
                for (curr = currentView->m->connections[from]; curr != NULL; curr = curr->next) {
                    if (curr->type == RAIL && visited[curr->v] == 0) {
                        locations[i] = curr->v;
                        i++;
                        visited[curr->v] = counter;
                        counter++;
                        for (second = currentView->m->connections[curr->v]; second != NULL; second = second->next) {
                            if (second->type == RAIL && visited[second->v] == 0) {
                                locations[i] = second->v;
                                visited[second->v] = counter;
                                i++;
                                counter++;
                            }
                        }
                    }
                }
            } else if (railLength%4 == 3) {
                VList second;
                VList third;
                for (curr = currentView->m->connections[from]; curr != NULL; curr = curr->next) {
                    if (curr->type == RAIL && visited[curr->v] == 0) {
                        locations[i] = curr->v;
                        visited[curr->v] = counter;
                        i++;
                        counter++;
                        for (second = currentView->m->connections[curr->v]; second != NULL; second = second->next) {
                            if (second->type == RAIL && visited[second->v] == 0) {
                                locations[i] = second->v;
                                visited[second->v] = counter;
                                i++;
                                counter++;
                                for (third = currentView->m->connections[second->v]; third != NULL; third = third->next) {
                                    if (third->type == RAIL && visited[third->v] == 0) {
                                        locations[i] = third->v;
                                        visited[third->v] = counter;
                                        i++;
                                        counter++;
                                    }
                                }
                                
                            }
                        }
                    }
                }
            } else {
                rail = FALSE;
            }
        }
        if (road == TRUE) {
            for (curr = currentView->m->connections[from]; curr != NULL; curr = curr->next) {
                if (curr->type == ROAD && visited[curr->v] == 0) {
                    locations[i] = curr->v;
                    visited[curr->v] = counter;
                    i++;
                    counter++;
                }
            }
        }
        if (sea == TRUE) {
            for (curr = currentView->m->connections[from]; curr != NULL; curr = curr->next) {
                if (curr->type == BOAT && visited[curr->v] == 0) {
                    locations[i] = curr->v;
                    visited[curr->v] = counter;
                    i++;
                    counter++;
                }
            }
        }
    } else {
        if (road == TRUE) {
            for (curr = currentView->m->connections[from]; curr != NULL; curr = curr->next) {
                if (curr->type == ROAD && curr->v != 60 && visited[curr->v] == 0) {
                    locations[i] = curr->v;
                    visited[curr->v] = counter;
                    i++;
                    counter++;
                }
            }
        }
        if (sea == TRUE) {
            for (curr = currentView->m->connections[from]; curr != NULL; curr = curr->next) {
                if (curr->type == BOAT && curr->v != 60 && visited[curr->v] == 0) {
                    locations[i] = curr->v;
                    visited[curr->v] = counter;
                    i++;
                    counter++;
                }
            }
        }
    }
    *numLocations = i;
    return locations;
}
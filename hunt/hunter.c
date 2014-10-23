// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "HunterView.h"

static char *randomDest (HunterView currentView, LocationID current, PlayerID hunter);
static LocationID destination (HunterView gameState, LocationID encounter, int order, PlayerID hunter);
static char *sedwardMove(HunterView currentView);

void decideHunterMove(HunterView gameState) {
    char *bestPlay = "";
    char *message = "";
    
    char *healthCheck = "Our lives are not our own"; //final byte for prevPlayer's initial health
    char *dracEncounter = "All hands on deck"; //
    
    PlayerID id = whoAmI(gameState);
    int health = howHealthyIs(gameState, id);
    int round = giveMeTheRound(gameState);
    
    if (round > 0) {
        PlayerID prevPlayer;
        if (id == PLAYER_LORD_GODALMING) {
            prevPlayer = PLAYER_MINA_HARKER;
        } else {
            prevPlayer = id - 1;
        }

        char *prevMessage = PlayerMessage[(round*NUM_PLAYERS)+prevPlayer];
        int newHealth = howHealthyIs(gameState, prevPlayer);
        if (strncmp(prevMessage, dracEncounter, 17) == 0) {
            
            /* ALL HANDS ON DECK TO BE IMPLEMENT */
            
            
        
        } else if (strncmp(prevMessage, healthCheck, 25) == 0) {
            int prevHealth = (int)prevMessage[27];
            if (newHealth < prevHealth) {
                LocationID encounter = whereIs(gameState, prevPlayer);
                char *abbrevEncounter = idToAbbrev(encounter);
                if (encounter == ST_JOSEPH_AND_ST_MARYS) {
                    // transported to hospital so need encounter location
                    abbrevEncounter = strndup(prevMessage+25,2);
                    encounter = abbrevToID(abbrevEncounter);
                }
                LocationID best = destination(gameState, encounter, 0, prevPlayer);
                bestPlay = idToAbbrev(best);
                char playerLetter;
                if (prevPlayer == PLAYER_LORD_GODALMING) {
                    playerLetter = 'G';
                } else if (prevPlayer == PLAYER_DR_SEWARD) {
                    playerLetter = 'S';
                } else if (prevPlayer == PLAYER_VAN_HELSING) {
                    playerLetter = 'V';
                } else if (prevPlayer == PLAYER_MINA_HARKER) {
                    playerLetter = 'M';
                }

                message = dracEncounter;
                
                
                
                
                /* MUST WORK OUT HOW TO OVERCOME ISSUE IF
                 HUNTER THAT ENCOUNTERED TRAP/DRACULA BUT
                 IS INSTEAD AT HOSPITAL*/
                
                
                
                
                //strcat(message, goal);
            } else {
                if (id == PLAYER_DR_SEWARD) {
                    bestPlay = sedwardMove(gameState);
                } else if (id == PLAYER_LORD_GODALMING) {
                    if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
                        bestPlay = "SR";
                    } else {
                        bestPlay = randomDest(gameState, whereIs(gameState,id), id);
                    }
                } else if (id == PLAYER_VAN_HELSING) {
                    if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
                        bestPlay = "HA";
                    } else {
                        bestPlay = randomDest(gameState, whereIs(gameState,id), id);
                    }
                } else if (id == PLAYER_MINA_HARKER) {
                    if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
                        bestPlay = "VE";
                    } else {
                        bestPlay = randomDest(gameState, whereIs(gameState,id), id);
                    }
                }
                message = healthCheck;
                strcat(message, (char)(health));
            }
        }
    } else {
        if (id == PLAYER_DR_SEWARD) {
            bestPlay = "GA";
        } else if (id == PLAYER_LORD_GODALMING) {
            bestPlay = "SR";
        } else if (id == PLAYER_VAN_HELSING) {
            bestPlay = "HA";
        } else if (id == PLAYER_MINA_HARKER) {
            bestPlay = "VE";
        }
        message = healthCheck;
        strcat(message, bestPlay); // better way of passing location that is more secure??
        strcat(message, (char)(health));
    }

    registerBestPlay(bestPlay,message);
}









static char *randomDest (HunterView gameState, LocationID current, PlayerID hunter) {
    int *numLocations = malloc(sizeof(int));
    char *dest = "";
    LocationID *connections = whereCanTheyGo(gameState, numLocations, hunter, TRUE, FALSE, TRUE);
    int counter = 0;
    if ((hunter == PLAYER_MINA_HARKER)||(hunter == PLAYER_VAN_HELSING)) {
        LocationID next = UNKNOWN_LOCATION;
        LocationID highest = current;
        while (counter < numLocations[0]) {
            LocationID city = connections[counter];
            if (city < current && city > next) {
                next = city;
            } else if (city > highest) {
                highest = city;
            }
            counter++;
        }
        if (next != UNKNOWN_LOCATION) {
            dest = idToAbbrev(next);
        } else {
            dest = idToAbbrev(highest);
        }
    } else if (hunter == PLAYER_LORD_GODALMING) {
        LocationID next = NUM_MAP_LOCATIONS;
        LocationID lowest = current;
        while (counter < numLocations[0]) {
            LocationID city = connections[counter];
            if (city > current && city < next) {
                next = city;
            } else if (city < lowest) {
                lowest = city;
            }
            counter++;
        }
        if (next != NUM_MAP_LOCATIONS) {
            dest = idToAbbrev(next);
        } else {
            dest = idToAbbrev(lowest);
        }
    }
    free(numLocations);
    
    return dest;
}


static char *sedwardMove (HunterView gameState) {
    char *bestPlay;
    PlayerID id = whoAmI(gameState);
    
    if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
        bestPlay = "GA";
    } else if (whereIs(gameState,id) == ST_JOSEPH_AND_ST_MARYS) {
        bestPlay = "SZ";
    } else if (whereIs(gameState,id) == CASTLE_DRACULA) {
        bestPlay = "KL";
    } else if (whereIs(gameState,id) == KLAUSENBURG) {
        bestPlay = "SZ";
    } else if (whereIs(gameState,id) == SZEGED) {
        bestPlay = "BE";
    } else if (whereIs(gameState,id) == BELGRADE) {
        bestPlay = "BC";
    } else if (whereIs(gameState,id) == BUCHAREST) {
        bestPlay = "GA";
    } else if (whereIs(gameState,id) == GALATZ) {
        bestPlay = "CD";
    }
    
    return bestPlay;
}



static LocationID destination (HunterView gameState, LocationID encounter, int order, PlayerID hunter) {
    int *numLocations = malloc(sizeof(int));
    // char *dest = "";
    LocationID *connections = whereCanTheyGo(gameState, numLocations, hunter, TRUE, FALSE, TRUE);
    if (order >= numLocations[0]) {
        order = numLocations[0] - 1;
    }
    
    // dest = connections[order];
    free(numLocations);
    
    return connections[order];
}

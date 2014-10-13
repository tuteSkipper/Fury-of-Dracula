// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"

void decideHunterMove(HunterView gameState) {
    PlayerID id = whoAmI(gameState);
    char *bestPlay = "";
    
    if (id == PLAYER_DR_SEWARD) {
        if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
            bestPlay = "CD";
        } else if (whereIs(gameState,id) == ST_JOSEPH_AND_ST_MARYS) {
            bestPlay = "SZ";
        } else if (whereIs(gameState,id) == CASTLE_DRACULA) {
            bestPlay = "KL";
        } else if (whereIs(gameState,id) == KLAUSENBURG) {
            bestPlay = "SZ";
        } else if (whereIs(gameState,id) == SZEGED) {
            bestPlay = "BC";
        } else if (whereIs(gameState,id) == BUCHAREST) {
            bestPlay = "GA";
        } else if (whereIs(gameState,id) == GALATZ) {
            bestPlay = "CD";
        }
    } else if (id == PLAYER_LORD_GOLDAMING) {
        if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
            bestPlay = "SR";
        } else {
            LocationID current = whereIs(gameState, id);
            int *numLocations;
            LocationID *connections = whereCanTheyGo(gameState, numLocations, id, TRUE, TRUE, TRUE);
            int counter = 0;
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
                bestPlay = idToAbbrev(next);
                // IMPORTANT: THERE IS NO idToAbbrev() FUNCTION, SHOULD WE ALTER PLACES.C TO MAKE ONE???
            } else {
                bestPlay = idToAbbrev(lowest);
            }
        }
    } else if (id == PLAYER_VAN_HELSING) {
        if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
            bestPlay = "HA";
        } else {
            LocationID current = whereIs(gameState, id);
            int *numLocations;
            LocationID *connections = whereCanTheyGo(gameState, numLocations, id, TRUE, TRUE, TRUE);
            int counter = 0;
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
                bestPlay = idToAbbrev(next);
                // IMPORTANT: THERE IS NO idToAbbrev() FUNCTION, SHOULD WE ALTER PLACES.C TO MAKE ONE???
            } else {
                bestPlay = idToAbbrev(highest);
            }
        }
    } else if (id == PLAYER_MINA_HARKER) {
        if (whereIs(gameState,id) == UNKNOWN_LOCATION) {
            bestPlay = "VE";
        } else {
            LocationID current = whereIs(gameState, id);
            int *numLocations;
            LocationID *connections = whereCanTheyGo(gameState, numLocations, id, TRUE, TRUE, TRUE);
            int counter = 0;
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
                bestPlay = idToAbbrev(next);
                // IMPORTANT: THERE IS NO idToAbbrev() FUNCTION, SHOULD WE ALTER PLACES.C TO MAKE ONE???
            } else {
                bestPlay = idToAbbrev(highest);
            }
        }
    }
    
    registerBestPlay(bestPlay,"A semi-randomly moving AI");
}

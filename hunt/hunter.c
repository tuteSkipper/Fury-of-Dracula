// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"

#define NUM_HUNTERS 4
#define MAX_MESSAGE_LENGTH (MESSAGE_SIZE+10)
#define MAX_PLAYS (366*5+5+10)

static char *randomDest (HunterView currentView, LocationID current, PlayerID hunter);
static char *sedwardMove(HunterView currentView);
static char *dracEncounter (HunterView currentView, LocationID encounter, PlayerID hitHunter, int order);

typedef struct _player {
    int health;
    LocationID position;
} player;

struct gameView {
    player players[NUM_PLAYERS];

    // dracula's trail; most recent last
    LocationID trail[TRAIL_SIZE];
    char *messages[MAX_PLAYS];
    char *pastPlays;
    int score;
    int turns;
};

struct hunterView {
    GameView g;
    
    // Dracula's last TRAIL_SIZE _locations_; NOT moves in REVERSE
    // chronological order [as best as we know]
    LocationID trailLocs[TRAIL_SIZE];
};
void decideHunterMove(HunterView gameState) {
    char *bestPlay = malloc(2*sizeof(char)+1);
    char *message = malloc(MAX_MESSAGE_LENGTH*sizeof(char)+1);
    
    PlayerID id = whoAmI(gameState);
    int health = howHealthyIs(gameState, id);
    int roundNum = giveMeTheRound(gameState);
    
    // Each play is allocated 3 bytes for their memory (normal or 'H' Move):
    // 0: First letter of their locations abbrev
    // 1: Health points in the beginning of their turn
    // 2: Second letter of their location abbrev
    // UNLESS trap/drac was encountered ('D' move), then message is 4 bytes long
    // 3: Who was attacked (e.g. if Helsing was attacked, Harker is n = 1, Goldalming, n = 2
    // Seward, n = 3 and Helsing is n = 4).

    
    // After each hunter has gone through a 'D' turn, it changes in to an 'H' turn next round
    
    if ((roundNum > 0)&&(id != PLAYER_DR_SEWARD)) {
        char *lastRoundMessage = malloc(sizeof(char)*MAX_MESSAGE_LENGTH);
        strncpy(lastRoundMessage, (char *) &(gameState->g->messages[((roundNum-1)*NUM_PLAYERS)+id]), MAX_MESSAGE_LENGTH);
        //^gotta strncpy this later
        // if (strcmp(lastRoundMessage, "") == 0) {
        //     bestPlay = randomDest(gameState, whereIs(gameState, id), id);
        //     message = &bestPlay[0];
        //     strcat(message, (char *) &health);
        //     strcat(message, &bestPlay[1]);
        //     registerBestPlay(bestPlay,message);
        //     return;
        // }
        
        if (lastRoundMessage[1] == 'H' || lastRoundMessage[1] == 'D') { // hunt is ON - go towards place in string
            /* When hunt is on, our destination should be stored via messages
             and retrieved by looking at prev round message.
             Only when destination is reached should random moves return and
             1st/3rd byte of player's message return to their bestPlay abbrev.
             */
            
            char *destination = &lastRoundMessage[0];
            strcat(destination, &lastRoundMessage[2]);
            lastRoundMessage[1] = 'H';
            lastRoundMessage[3] = '\0';
            LocationID encounterLoc = abbrevToID(destination);
            LocationID current = whereIs(gameState, id);
            if (current == encounterLoc) {
                // if reached dest, return to random moves - shouldn't enter but extra precaution
                bestPlay = randomDest(gameState, current, id);
                message = &bestPlay[0];
                strcat(message, (char *) &health);
                strcat(message, &bestPlay[1]);
            } else {
                // still on path towards dest
                Map g = newMap();
                LocationID *path = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                TransportID *trans = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                shortestPath(g, current, encounterLoc, path, trans);
                int railDist = getDist(g, RAIL, current, encounterLoc); // length of any direct rail connections to dest
                int railMove = (roundNum + id) % NUM_HUNTERS; // max. no of steps by rail
                
                if (trans[1] == RAIL && railMove == 0) {
                    // if next dest is illegal RAIL move, rest until next turn
                    bestPlay = idToAbbrev(current);
                    message = lastRoundMessage;
                } else if ((railDist == 2 && railMove >= 2) || (railDist == 3 && railMove == 3)) {
                    // take two or three rail moves to get to dest if legal
                    message = &destination[0];
                    strcat(message, (char *) &health);
                    strcat(message, &destination[1]);
                    bestPlay = destination;
                } else {
                    bestPlay = idToAbbrev(path[1]);
                    /*message = destination[0];
                    strcat(message, "H");
                    strcat(message, destination[1]);*/
                    if (bestPlay == destination) {
                        message = &destination[0];
                        strcat(message, (char *) &health);
                        strcat(message, &destination[1]);
                    } else {
                        message = lastRoundMessage;
                    }
                }
                //disposeMap(g);
            }
        } else {
            PlayerID prevPlayer;
            if (id == PLAYER_LORD_GODALMING) {
                prevPlayer = PLAYER_MINA_HARKER;
            } else {
                prevPlayer = id - 1;
            }
            char *prevHuntMessage = malloc(sizeof(char)*MAX_MESSAGE_LENGTH);
            strncpy(prevHuntMessage, (char *) &(gameState->g->messages[(roundNum*NUM_PLAYERS)+prevPlayer]), MAX_MESSAGE_LENGTH);
            
            // if (strcmp(prevHuntMessage, "") == 0) {
            //     bestPlay = randomDest(gameState, whereIs(gameState, id), id);
            //     message = &bestPlay[0];
            //     strcat(message, (char *) &health);
            //     strcat(message, &bestPlay[1]);
            //     registerBestPlay(bestPlay,message);
            //     return;
            // }
            
            if (prevHuntMessage[1] == 'D') {
                // 1. Check if curr. player was the one who intially lost health - no need to hunt
                // far unless at hospital
                int order = (int)prevHuntMessage[3] + 1;
                if (order > 3) {
                    LocationID currLoc = whereIs(gameState, id);
                    if (currLoc == ST_JOSEPH_AND_ST_MARYS) {
                        // teleported to hospital so travel back to encounter location
                        char *prevLocation = &lastRoundMessage[0];
                        strcat(prevLocation, &lastRoundMessage[2]);
                        LocationID encounterLoc = abbrevToID(prevLocation);
                        Map g = newMap();
                        LocationID *path = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                        TransportID *trans = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                        shortestPath(g, currLoc, encounterLoc, path, trans);
                        int railDist = getDist(g, RAIL, currLoc, encounterLoc); // length of any direct rail connections to dest
                        int railMove = (roundNum + id) % NUM_HUNTERS; // max. no of steps by rail
                        
                        if (trans[1] == RAIL && railMove == 0) {
                            // if next dest is illegal RAIL move, rest until next turn
                            bestPlay = idToAbbrev(currLoc);
                            message = lastRoundMessage;
                        } else if ((railDist == 2 && railMove >= 2) || (railDist == 3 && railMove == 3)) {
                            // take two or three rail moves to get to dest if legal
                            message = &prevLocation[0];
                            strcat(message, (char *) &health);
                            strcat(message, &prevLocation[1]);
                            bestPlay = prevLocation;
                        } else {
                            bestPlay = idToAbbrev(path[1]);
                            message = &prevLocation[0];
                            if (bestPlay == prevLocation) {
                                strcat(message, (char *) &health);
                            } else {
                                strcat(message, "H");
                            }
                            strcat(message, &prevLocation[1]);
                        }
                        //disposeMap(g);
                    } else {
                        // continue on with random moves
                        bestPlay = randomDest(gameState, currLoc, id);
                        message[0] = bestPlay[0];
                        message[1] = health;
                        message[2] = bestPlay[1];
                    }
                } else {
                    // 2. Find out info about player who lost health
                    int tempOrder = order;
                    PlayerID hitHunter = id;
                    while (tempOrder > 0) {
                        if (hitHunter == PLAYER_LORD_GODALMING) {
                            hitHunter = PLAYER_MINA_HARKER; // loop back around
                        }
                        tempOrder--;
                    }
                    LocationID hitCurrLoc = whereIs(gameState, hitHunter);
                    if (hitCurrLoc == ST_JOSEPH_AND_ST_MARYS) {
                        // 2.a) In hospital? - Access location of encounter from string
                        char *hitString = gameState->g->messages[((roundNum-1)*NUM_PLAYERS)+id-order];
                        char *encountName = &hitString[0];
                        strcat(encountName, &hitString[2]);
                        LocationID encounterLoc = abbrevToID(encountName);
                        LocationID currLoc = whereIs(gameState, id);
                        Map g = newMap();
                        LocationID *path = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                        TransportID *trans = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                        shortestPath(g, currLoc, encounterLoc, path, trans);
                        
                        if (trans[1] == RAIL && (roundNum + id) % NUM_HUNTERS == 0) {
                            // if next dest is illegal RAIL move, rest until next turn
                            bestPlay = idToAbbrev(currLoc);
                        } else {
                            bestPlay = idToAbbrev(path[1]);
                        }
                        message = &encountName[0];
                        strcat(message, "H");
                        strcat(message, &encountName[1]);
                    } else {
                        // 2. b) Access from hurt Hunter's location
                        bestPlay = dracEncounter(gameState, hitCurrLoc, hitHunter, order);
                        message = &bestPlay[0];
                        strcat(message, "H");
                        strcat(message, &bestPlay[1]);
                    }
                }
            } else {
                /* After the first round, generic checking of prev player's current health
                 against their health at the beginning of their turn by looking at the middle
                 letter of string - if less than when started, 'D' turn is activated */
                int newHealth = howHealthyIs(gameState, prevPlayer);
                int oldHealth = (int)prevHuntMessage[1];
                LocationID currLoc = whereIs(gameState, id);
                if (newHealth < oldHealth) { // start 'D' move
                    LocationID prevLoc = whereIs(gameState, prevPlayer);
                    if (prevLoc == ST_JOSEPH_AND_ST_MARYS) {
                        // 2.a) In hospital? - Access location of encounter from string
                        char *location = &prevHuntMessage[0];
                        strcat(location, &prevHuntMessage[2]);
                        LocationID encounterLoc = abbrevToID(location);
                        Map g = newMap();
                        LocationID *path = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                        TransportID *trans = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
                        shortestPath(g, currLoc, encounterLoc, path, trans);
                        
                        if (trans[1] == RAIL && (roundNum + id) % NUM_HUNTERS == 0) {
                            // if next dest is illegal RAIL move, rest until next turn
                            bestPlay = idToAbbrev(currLoc);
                        } else {
                            bestPlay = idToAbbrev(path[1]);
                        }
                        message = &location[0];
                        strcat(message, "D");
                        strcat(message, &location[1]);
                    } else {
                    printf("Here\n");
                        // 2. b) Access from hurt Hunter's location
                        bestPlay = dracEncounter(gameState, prevLoc, prevPlayer, 1);
                        strcpy(message, &bestPlay[0]);
                        strcat(message, "D");
                        strcat(message, &bestPlay[1]);
                    }
                    strcat(message, "1");
                    // NOT POSITIVE HOW TO GO ABOUT PUTTING INTS IN STRINGS
                } else {
                    // Nothing of interest - continue on with generic random move turn
                    bestPlay = randomDest(gameState, currLoc, id);
                    strcpy(message, &bestPlay[0]);
                    // int *healthPtr = &health;
                    // strcat(message, (char *)healthPtr);
                    strcat(message, (char *)&health);
                    strcat(message, &bestPlay[1]);
                }
            }
        }
    } else if (roundNum > 0 && id == PLAYER_DR_SEWARD) {
        bestPlay = sedwardMove(gameState);
        char *GDmessage = gameState->g->messages[((roundNum-1)*NUM_PLAYERS)];
        
        if (strcmp(GDmessage, "") == 0) {
            message = &bestPlay[0];
            strcat(message, (char *) &health);
            strcat(message, &bestPlay[1]);
        } else {
            int GDhealth = howHealthyIs(gameState, PLAYER_LORD_GODALMING);
            int order = (int)GDmessage[3] + 1;
            if (GDmessage[1] == 'D') { // During a 'D' round (a hunter lost health in that round)
                message = GDmessage;
                message[3] = order;
            } else if (GDmessage[1] != 'H' && GDhealth < (int)GDmessage[1]) { // GOLDALMING lost health
                if (order > 3) { // Sedward was one who lost health initially
                    message[0] = bestPlay[0];
                    message[1] = health;
                    message[2] = bestPlay[1];
                } else { // Pass on 'D' message
                    message[0] = bestPlay[0];
                    message[1] = 'D';
                    message[2] = bestPlay[1];
                    message[3] = order;
                }
            }
        }
    } else {
        if (id == PLAYER_LORD_GODALMING) {
            bestPlay = "SR";
        } else if (id == PLAYER_DR_SEWARD) {
            bestPlay = "GA";
        } else if (id == PLAYER_VAN_HELSING) {
            bestPlay = "HA";
        } else if (id == PLAYER_MINA_HARKER) {
            bestPlay = "VE";
        }
        message[0] = bestPlay[0];
        message[1] = health;
        message[2] = bestPlay[1];
    }


    registerBestPlay(bestPlay,message);
}









static char *randomDest (HunterView currentView, LocationID current, PlayerID hunter) {
    int *numLocations = malloc(sizeof(int));
    char *dest = malloc(sizeof(char)*2);
    LocationID *connections = whereCanTheyGo(currentView, numLocations, hunter, TRUE, FALSE, TRUE);
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
            strcpy(dest, idToAbbrev(next));
        } else {
            strcpy(dest, idToAbbrev(highest));
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
            strcpy(dest, idToAbbrev(next));
        } else {
            strcpy(dest, idToAbbrev(lowest));
        }
    }
    free(numLocations);
    
    return dest;
}


static char *sedwardMove (HunterView currentView) {
    char *bestPlay = malloc(sizeof(char)*2);
    PlayerID id = whoAmI(currentView);
    if (whereIs(currentView,id) == UNKNOWN_LOCATION) {
        strcpy(bestPlay, "GA");
    } else if (whereIs(currentView,id) == ST_JOSEPH_AND_ST_MARYS) {
        strcpy(bestPlay, "SZ");
    } else if (whereIs(currentView,id) == CASTLE_DRACULA) {
        strcpy(bestPlay, "KL");
    } else if (whereIs(currentView,id) == KLAUSENBURG) {
        strcpy(bestPlay, "SZ");
    } else if (whereIs(currentView,id) == SZEGED) {
        strcpy(bestPlay, "BE");
    } else if (whereIs(currentView,id) == BELGRADE) {
        strcpy(bestPlay, "BC");
    } else if (whereIs(currentView,id) == BUCHAREST) {
        strcpy(bestPlay, "GA");
    } else if (whereIs(currentView,id) == GALATZ) {
        strcpy(bestPlay, "CD");
    }
    return bestPlay;
}


static char *dracEncounter (HunterView currentView, LocationID encounter, PlayerID hitHunter, int order) {
    int *numLocations = malloc(sizeof(int));
    char *dest = malloc(sizeof(char)*2);
    order -= 1;
    
    LocationID *connections = whereCanTheyGo(currentView, numLocations, hitHunter, TRUE, FALSE, TRUE);
    if (order >= numLocations[0]) {
        order = numLocations[0] - 1;
    }
    
    Map g = newMap();
    LocationID *path = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
    TransportID *trans = malloc(sizeof(char)*NUM_MAP_LOCATIONS);
    LocationID destination = connections[order];
    PlayerID player = whoAmI(currentView);
    LocationID playerLoc = whereIs(currentView, player);
    shortestPath(g, playerLoc, destination, path, trans);
        
    if (trans[1] == RAIL && (giveMeTheRound(currentView) + player) % NUM_HUNTERS == 0) {
        // if next dest is illegal RAIL move, rest until next turn
        dest = idToAbbrev(playerLoc);
    } else {
        dest = idToAbbrev(path[1]);
    }

    //strcpy(dest, idToAbbrev(destination));

    free(numLocations);

    return dest;
}

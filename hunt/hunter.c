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
    // 1: First letter of their locations abbrev
    // 2: Health points in the beginning of their turn
    // 3: Second letter of their location abbrev
    // UNLESS trap/drac was encountered ('D' move), then message is 6 bytes long
    // 4: First letter of encounter abbrev
    // 5: Second letter of encounter abbrev
    // 6: Who was attacked (e.g. if Helsing was attacked, Harker is n = 1, Goldalming, n = 2
    // Seward, n = 3 and Helsing is n = 4).

    
    // After each hunter has gone through a 'D' turn, it changes in to an 'H' turn next round
    
    if ((roundNum > 0)&&(id != PLAYER_DR_SEWARD)) {        
        char *lastRoundMessage = malloc(sizeof(char)*MAX_MESSAGE_LENGTH);
        strncpy(lastRoundMessage, (char *) &(gameState->g->messages[((roundNum-1)*NUM_PLAYERS)+id]), MAX_MESSAGE_LENGTH);
        //^gotta strncpy this later
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
                LocationID path[NUM_MAP_LOCATIONS];
                TransportID trans[NUM_MAP_LOCATIONS];
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
            char *prevHuntMessage = gameState->g->messages[(roundNum*NUM_PLAYERS)+prevPlayer];
            //int newHealth = howHealthyIs(gameState, prevPlayer);
            if (prevHuntMessage[1] == 'D') {
                // 1. Check if curr. player was the one who intially lost health - no need to hunt
                // far unless at hospital
                int order = (int)prevHuntMessage[5] + 1;
                if (order > 3) {
                    LocationID currLoc = whereIs(gameState, id);
                    if (currLoc == ST_JOSEPH_AND_ST_MARYS) {
                        // teleported to hospital so travel back to encounter location
                        char *prevLocation = &lastRoundMessage[0];
                        strcat(prevLocation, &lastRoundMessage[2]);
                        LocationID encounterLoc = abbrevToID(prevLocation);
                        Map g = newMap();
                        LocationID path[NUM_MAP_LOCATIONS];
                        TransportID trans[NUM_MAP_LOCATIONS];
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
                    // 2. Check if player who lost health is now at hospital
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
                        char *hitString = gameState->g->messages[((roundNum-1)*NUM_PLAYERS)+id-order];
                        char *encountName = &hitString[0];
                        strcat(encountName, &hitString[2]);
                        LocationID encounterLoc = abbrevToID(encountName);
                        LocationID currLoc = whereIs(gameState, id);
                        Map g = newMap();
                        LocationID path[NUM_MAP_LOCATIONS];
                        TransportID trans[NUM_MAP_LOCATIONS];
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
                    }
                    
                }
                
                
                
                
                
                
                char *encounter = &prevHuntMessage[3];
                strcat(encounter, &prevHuntMessage[4]);
                LocationID encounterID = abbrevToID(encounter);
                bestPlay = dracEncounter(gameState, encounterID, id, ((int)prevHuntMessage[5] + 1));
                
                
            }
            // segfault due to not having an else here?????
            
            
            
            
            
            
            
            
            
            
        }
        
    
    } else if (roundNum > 0 && id == PLAYER_DR_SEWARD) {
        bestPlay = sedwardMove(gameState);
        char *GDmessage = gameState->g->messages[((roundNum-1)*NUM_PLAYERS)];
        int GDhealth = howHealthyIs(gameState, PLAYER_LORD_GODALMING);
        int order = (int)GDmessage[5] + 1;
        if (GDmessage[1] == 'D') { // During a 'D' round (a hunter lost health in that round)
            message = GDmessage;
            message[5] = order;
        } else if (GDmessage[1] != 'H' && GDhealth < (int)GDmessage[1]) { // GOLDALMING lost health
            if (order > 3) { // Sedward was one who lost health initially
                message[0] = bestPlay[0];
                message[1] = health;
                message[2] = bestPlay[1];
            } else { // Pass on 'D' message
                message = GDmessage;
                message[0] = bestPlay[0];
                message[2] = bestPlay[1];
                message[5] = order;
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
    char *dest = "";
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


static char *sedwardMove (HunterView currentView) {
    char *bestPlay = NULL;
    PlayerID id = whoAmI(currentView);
    
    if (whereIs(currentView,id) == UNKNOWN_LOCATION) {
        bestPlay = "GA";
    } else if (whereIs(currentView,id) == ST_JOSEPH_AND_ST_MARYS) {
        bestPlay = "SZ";
    } else if (whereIs(currentView,id) == CASTLE_DRACULA) {
        bestPlay = "KL";
    } else if (whereIs(currentView,id) == KLAUSENBURG) {
        bestPlay = "SZ";
    } else if (whereIs(currentView,id) == SZEGED) {
        bestPlay = "BE";
    } else if (whereIs(currentView,id) == BELGRADE) {
        bestPlay = "BC";
    } else if (whereIs(currentView,id) == BUCHAREST) {
        bestPlay = "GA";
    } else if (whereIs(currentView,id) == GALATZ) {
        bestPlay = "CD";
    }
    
    return bestPlay;
}


static char *dracEncounter (HunterView currentView, LocationID encounter, PlayerID hitHunter, int order) {
    int *numLocations = malloc(sizeof(int));
    char *dest = "";
    order -= 1;
    
    LocationID *connections = whereCanTheyGo(currentView, numLocations, hitHunter, TRUE, FALSE, TRUE);
    if (order >= numLocations[0]) {
        order = numLocations[0] - 1;
    }
    
    LocationID destination = connections[order];
    dest = idToAbbrev(destination);
    
    free(numLocations);
    
    
    return dest;
}

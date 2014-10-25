// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <time.h> //random number
#include "Game.h"
#include "DracView.h"
#include "Map.h"

static int isIllegal (DracView gameState, LocationID dest);

void decideDraculaMove(DracView gameState)
{
    // TODO ...
    // Replace the line below by something better
    
    // Random going to random place
    srand(time(NULL));
    
    //Get all location
    // LocationID allPlaces[NUM_MAP_LOCATIONS];
    // int i;
    // for (i = 0; i < NUM_MAP_LOCATIONS; i++) {
    // allPlaces[i] = i;
    // }
    
    //going to a random place at the start
    Round currRound = giveMeTheRound(gameState);
    if ( currRound == 0 ){
        int r = rand() % NUM_MAP_LOCATIONS;
        if ( idToType(r) == SEA ){
            while ( idToType(r) == SEA ){
                r = rand() % NUM_MAP_LOCATIONS;
            }
        }
        char *goToPlace = idToAbbrev(r);
        registerBestPlay(goToPlace,"MwuhahahahaHEREICOME");
    } else if (howHealthyIs(gameState, PLAYER_DRACULA) <= 20){
            Map theMap;
            theMap = newMap();
            
            int i, n;
            LocationID path[NUM_MAP_LOCATIONS];
            TransportID trans[NUM_MAP_LOCATIONS];
            LocationID myLoc = whereIs(gameState, PLAYER_DRACULA);
            int currLocationfForDrac = whereIs(gameState, PLAYER_DRACULA);
            n = shortestPath(theMap, currLocationfForDrac, CASTLE_DRACULA, path, trans);
        
            int size, dest;
            LocationID *whereToGo = malloc(NUM_MAP_LOCATIONS);
            whereToGo = whereCanIgo(gameState, &size ,TRUE, TRUE);
        
            if (n == 0) {
                //cant reach
            } else {
                i = 1;
                int illegal = 0;
                if (isIllegal(gameState, path[i]) == 0) {
                    while (illegal == 0){
                        int a = rand()%size;
                        if ((a == 0) && (whereToGo[a] == myLoc)){
                            a++;
                        }
                        dest = whereToGo[a];
                        illegal = isIllegal(gameState, dest);
                    }
                    char *goToPlace = idToAbbrev(dest);
                    registerBestPlay(goToPlace,"MwuhahahahaRUN");
                } else {
                    registerBestPlay(idToAbbrev(path[i]),"MwuhahahahaRUN1");
                }
                
                /*
                 for (i = 1; i < n ; i++){
                 if (i > 1 && n > 2) printf("then ");
                 printf("go to %s by ", idToName(path[i]));
                 switch (trans[i]) {
                 case ROAD: printf("road\n"); break;
                 case RAIL: printf("rail\n"); break;
                 case BOAT: printf("boat\n"); break;
                 default:   printf("????\n"); break;
                 }
                 */
            }
    } else {
        int size;
        int illegal = 0, dest;
        LocationID myLoc = whereIs(gameState, PLAYER_DRACULA);
        //printf ("A\n");
        LocationID *whereToGo = malloc(NUM_MAP_LOCATIONS);
        whereToGo = whereCanIgo(gameState, &size ,TRUE, FALSE);
        
        
        if (size < 1) {
            whereToGo = whereCanIgo(gameState, &size ,FALSE, TRUE);
            
            
            if (size < 1) {
                registerBestPlay("HI","MwuhahahahaCANNOTFINDME");
                return;
            }
        }
        
        while (illegal == 0){
            int i = rand()%size;
            if ((i == 0) && (whereToGo[i] == myLoc)){
                i++;
            }
            dest = whereToGo[i];
            illegal = isIllegal(gameState, dest);
        }
        
        char *goToPlace = idToAbbrev(dest);
        registerBestPlay(goToPlace,"Mwuhahahaha");
    
        }
    
}

static int isIllegal (DracView gameState, LocationID dest)
{
    int j;
    //LocationID trail[TRAIL_SIZE];
    LocationID *trailLocs = malloc(sizeof(TRAIL_SIZE));
    giveMeTheTrail(gameState, PLAYER_DRACULA, trailLocs);
    for (j = 0; j < TRAIL_SIZE; j++) {
        //printf("[%d] = %d\n", j, trailLocs[j]);
        if (trailLocs[j] == dest) {
            return 0;
        }
    }
    return 1;
}
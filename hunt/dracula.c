// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include <time.h> //random number
#include "Game.h"
#include "DracView.h"



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
        registerBestPlay(goToPlace,"Mwuhahahaha");
    } else {
        int size;
        LocationID *whereToGo = malloc(NUM_MAP_LOCATIONS);
        whereToGo = whereCanIgo(gameState, &size ,TRUE, FALSE);
        if (size <= 1)
        {
            whereToGo = whereCanIgo(gameState, &size ,FALSE, TRUE);
            if (size <= 1) {
                registerBestPlay("HI","Mwuhahahaha");
                return;
            }
        }
        int i = rand()%size;
        if (i == 0)
            i++;
        int dest = whereToGo[i];
        char *goToPlace = idToAbbrev(dest);
        registerBestPlay(goToPlace,"Mwuhahahaha");
    }
    
    if (howHealthyIs(gameState, PLAYER_DRACULA) <= 10){
        //registerBestPlay("CD","Mwuhahahaha");
    }
    
	//registerBestPlay("CD","Mwuhahahaha");
}
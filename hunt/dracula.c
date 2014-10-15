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
        //Location *whereToGo = whereCanIgo(gameState, ,TRUE, FALSE);
    }
    
    if (howHealthyIs(gameState, PLAYER_DRACULA) <= 10){
        //registerBestPlay("CD","Mwuhahahaha");
    }
    
	//registerBestPlay("CD","Mwuhahahaha");
}
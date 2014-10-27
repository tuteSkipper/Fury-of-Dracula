// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "DracView.h"
#include "Places.h"
#include "Map.h"

static int *getHuntersLocations(DracView gameState, LocationID huntersTrail[]);

void decideDraculaMove(DracView gameState)
{
    //Round currRound = giveMeTheRound(gameState);
    LocationID myLocation = whereIs(gameState, PLAYER_DRACULA);
    if (myLocation == NOWHERE) {
        LocationID startingPoint[] = {KLAUSENBURG, ATHENS, BERLIN, VENICE, TOULOUSE};
        int i, huntersTrail[4];
        getHuntersLocations(gameState, huntersTrail);
        for (i = 0; i < 5; i++) {
            if (startingPoint[i] == huntersTrail[0] || startingPoint[i] == huntersTrail[1] ||
                startingPoint[i] == huntersTrail[2] || startingPoint[i] == huntersTrail[3])
                startingPoint[i] = -1;
        }
        i = 0;
        if (startingPoint[i] == -1) {
            while (startingPoint[i] == -1)
                i++;
        }
        registerBestPlay(idToAbbrev(startingPoint[i]),"hahaha");
    } else {
        int i, j, size = 0;
        LocationID *dest = whereCanIgo(gameState, &size ,TRUE, FALSE);
        LocationID trails[TRAIL_SIZE];
        giveMeTheTrail(gameState, PLAYER_DRACULA, trails);
        for(i = 0; i < NUM_MAP_LOCATIONS; i++) {
            for(j = 0; j <TRAIL_SIZE; j++)
                if (dest[i] == trails[j])
                    dest[i] = -1;
        }
        
        for (i=0; i<TRAIL_SIZE; i++) {
            printf("trails[%d] = %d\n", i, trails[i]);
            printf("dest[%d] = %d\n", i, dest[i]);
        }
        
        for(i=0; i<TRAIL_SIZE; i++) {
            if (dest[i] != -1)
                break;
        }
        if (i == TRAIL_SIZE){
            printf("test\n");
            registerBestPlay("TP","Mwuha");
            return;
        }
            
        registerBestPlay(idToAbbrev(dest[i]),"Mwuhahahaha");
    }
}

static int *getHuntersLocations(DracView gameState, LocationID huntersTrail[])
{
    int i;
    for(i=0;i<4;i++)
        huntersTrail[i] = whereIs(gameState, i);

    return huntersTrail;
}


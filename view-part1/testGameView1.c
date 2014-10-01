GameView gv;

printf("Test for Dracula die in the first round\n");
PlayerMessage messages001[] = {"p1","p2","p3","p4","p5"};
gv = newGameView("GLO.... SLO.... HLO.... MLO.... DLO....", messages001);

assert(getCurrentPlayer(gv) == PLAYER_LORD_GODALMING);

assert(getLocation(gv,PLAYER_LORD_GODALMING) == LONDON);
assert(getHealth(gv,PLAYER_LORD_GODALMING) == 9);
assert(getLocation(gv,PLAYER_DR_SEWARD) == LONDON);
assert(getHealth(gv,PLAYER_DR_SEWARD) == 9);
assert(getLocation(gv,PLAYER_VAN_HELSING) == LONDON);
assert(getHealth(gv,PLAYER_VAN_HELSING) == 9);
assert(getLocation(gv,PLAYER_MINA_HARKER) == LONDON);
assert(getHealth(gv,PLAYER_MINA_HARKER) == 9);
assert(getLocation(gv,PLAYER_DRACULA) == LONDON);

assert(getHealth(gv,PLAYER_LORD_GODALMING) == 5);
assert(getHealth(gv,PLAYER_DR_SEWARD) == 5);
assert(getHealth(gv,PLAYER_VAN_HELSING) == 5);
assert(getHealth(gv,PLAYER_MINA_HARKER) == 5);
assert(getHealth(gv,PLAYER_DRACULA) == 0);
printf("passed\n");
disposeGameView(gv);

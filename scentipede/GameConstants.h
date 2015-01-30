#ifndef _GAMECONSTANTS_H_

#define _GAMECONSTANTS_H_

// IDs for the game objects

#define IID_SCENTIPEDE_SEGMENT				0
#define IID_SPIDER				1
#define IID_FLEA				2
#define IID_SCORPION			3
#define IID_PLAYER				100
#define IID_MUSHROOM			200
#define IID_POISON_MUSHROOM		201
#define IID_WATER_DROPLET		300

// keys the user can hit

#define KEY_PRESS_LEFT_ARROW		1000
#define KEY_PRESS_RIGHT_ARROW		1001
#define KEY_PRESS_UP_ARROW			1002
#define KEY_PRESS_DOWN_ARROW		1003
#define KEY_PRESS_SPACE				' '

// board dimensions 

#define GARDEN_WIDTH						30
#define GARDEN_HEIGHT					20

// status of each tick (did the player die?)

#define GWSTATUS_PLAYER_DIED					0
#define GWSTATUS_CONTINUE_GAME					1

// test parameter constants

#define NUM_TEST_PARAMS						5

#define TEST_PARAM_CENT_CHANCE_INDEX		0
#define TEST_PARAM_SPIDER_CHANCE_INDEX		1
#define TEST_PARAM_FLEA_CHANCE_INDEX		2
#define TEST_PARAM_SCORPION_CHANCE_INDEX	3
#define TEST_PARAM_STARTING_MUSHROOMS		4


#endif // #ifndef _GAMECONSTANTS_H_

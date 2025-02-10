#ifndef TYPES_H
#define TYPES_H

// handles the gameplay options
typedef enum
{
    INIT,
    SIMONS_TURN,
    PLAYERS_TURN, 
    RECORD_RESULT, 
    RESULT,
} State;

// Handles the players turn
typedef enum
{
    PLAYER_PAUSE, 
    PLAYER_PLAY,
} Players_Turn_State;

// Handles simons turn
typedef enum
{
    SIMON_START, 
    SIMON_PLAY, 
    SIMON_SILENT, 
} Simons_Turn_State;

// Hanldes input from uart
typedef enum
{
    AWAITING_COMMAND,
    AWAITING_PAYLOAD,
    AWAITING_NAME,
    AWAITING_NEWLINE,
} Serial_State;

// handles the users result state
typedef enum
{
    RANK, 
    SHOW_RANK,
    SHOW_LEVEL, 
} Level_State; 

#endif // TYPES_H

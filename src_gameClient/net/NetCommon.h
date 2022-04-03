#pragma once


#define USER_ID_LEN 32
#define CMD_MIN_LEN (USER_ID_LEN + sizeof(int32_t))


#define GAME_WORLD_WIDTH 2000

// Server command names...
#define CMD_FetchServerMessage	0x1
#define CMD_CreateFaction		0x2
#define CMD_FetchWorldState		0x3


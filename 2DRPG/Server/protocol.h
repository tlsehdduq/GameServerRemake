#pragma once
// ÆÐÅ¶, 
constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 1000;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;
constexpr int MAX_USER = 15;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE_PLAYER = 1;
constexpr char CS_ATTACK = 2;
constexpr char CS_CHAT = 3;
constexpr char CS_MOVE_NPC = 4;

constexpr char SC_LOGIN = 0;
constexpr char SC_ADD_OBJECT = 1;
constexpr char SC_MOVE_PLAYER = 2;
constexpr char SC_MOVE_OBJECT = 3;
constexpr char SC_CHAT = 4;

struct CS_LOGIN_PACKET
{
	unsigned char size;
	char type;
	char name[NAME_SIZE];
	int id;

};
struct CS_MOVE_PLAYER_PACKET
{
	unsigned char size;
	char type;
	char dir;
	// move_time? 

};
struct CS_ATTACK_PACKET
{
	unsigned char size;
	char type;

};
struct CS_CHAT_PACKET
{
	unsigned char size;
	char type;
	char message[CHAT_SIZE];
};
struct CS_MOVE_NPC_PACKET
{
	unsigned char size;
	char type;
	char dir; 
};


struct SC_LOGIN_PACKET
{
	unsigned char size;
	char type;
	int id; 
	int hp;
	int max_hp;
	int exp;
	int level;
	short x, y;
};
struct SC_ADD_PACKET
{
	unsigned char size;
	char type;
	int id;
	short x, y;
	char name[NAME_SIZE];
};
struct SC_MOVE_PLAYER_PACKET
{
	unsigned char size;
	char type;
	int id; 
	short x, y;
};
struct SC_MOVE_NPC_PACKET
{
	unsigned char size;
	char type;
	int n_id;
	short x, y;
};
struct SC_CHAT_PACKET
{
	unsigned char size;
	char type;
	int id;
	char message[CHAT_SIZE];
};
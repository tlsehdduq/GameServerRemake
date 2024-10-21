#pragma once
// ��Ŷ, 
constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 1000;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;
constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 200000;

constexpr int W_WIDTH = 1000;
constexpr int W_HEIGHT = 1000;

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
constexpr char SC_REMOVE = 5;
constexpr char SC_MONTSER_INIT = 6;
constexpr char SC_MONSTER_MOVE = 7;
constexpr char SC_MONSTER_REMOVE = 8;


struct CS_LOGIN_PACKET
{
	unsigned char size;
	char type;
	char name[NAME_SIZE];
	//int id = -1;

};
struct CS_MOVE_PLAYER_PACKET
{
	unsigned char size;
	char type;
	char dir;
	unsigned int move_time;
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
	unsigned int move_time;
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
struct SC_REMOVE_PACKET {
	unsigned char size;
	char type;
	int id;
	int sessiontype;
};
struct SC_MONTSER_INIT_PACKET {
	unsigned char size;
	char type;
	int id;
	int hp;
	short x, y;
	int max_hp;
	int att;
};
struct SC_MONSTER_MOVE_PACKET {
	unsigned char size;
	char type;
	int id;
	short x, y;
};
struct SC_MONSTER_REMOVE_PACKET {
	unsigned char size;
	char type;
	int id;
};
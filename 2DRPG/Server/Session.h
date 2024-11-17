#pragma once
#pragma once
#pragma once
#include"protocol.h"
#include"Astar.h"
enum class STATE : unsigned int { Free, Alloc, Ingame, Start };
class Session
{
public:
	short getPosx();
	short getPosy();
	int getId();
	SOCKET getSocket();
	char* getName();
	int getMoveTime();
	int getHp();

	void setPosx(short x);
	void setPosy(short y);
	void setId(int id);
	void setSocket(SOCKET socket);
	void setName(char* name);
	void setMoveTime(int time);
	void setHp(int hp);

	void doRecv();
	void doSend(void* packet);

	bool can_see(int from, int to, int type);


public:
	//virtual void attack() = 0;
	void sendLoginPacket();
	void sendAddPacket(const Session& client);
	void sendMoverPlayerPacket(const Session& client);
	void sendRemovePacket(int id,int type);

protected:
	OVERLAPPED_EX _s_over;
	short _x;
	short _y;
	int _id = -1;
	int _hp = 0;
	int _maxhp = 0;
	int _att = 0;
	int _last_move_time;

	SOCKET _socket;
	char _name[NAME_SIZE];


public:
	STATE _state;
	mutex _s_lock;
	int _prevremain;
	int _viewrange = 5;
	int _range = 10;
	int _dir = 0;
	bool _leftright = false;
	unordered_set <int> player_view_list;
	unordered_set <int> monster_view_list;
	mutex _vl;

	

};
class Monster;
class Player : public Session
{
public:
	void move(int dir);
	void attack();
	bool canatt(int from, int to);
	//void attack() override;

public:
	void sendMonsterInit(int id);
	void sendMonsterMove(Monster& monster);
	void sendMonsterRemove(int id);
	void sendAttack(int id,bool onoff);
private:
	int _attrange = 1;

};

class Monster : public Session
{
public:
	Monster();
	void move();
	void randommove();
	void moveTowardsPlayer(short playerx, short playery);

	atomic_bool isalive = false;
	mutex _lock;

private:
	// 길찾기 알고리즘 
	static int _map[MAPSIZE][MAPSIZE];
	static bool _closedList[MAPSIZE][MAPSIZE];
	static int _cost[MAPSIZE][MAPSIZE];

};


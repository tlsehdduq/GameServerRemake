#pragma once
#include"protocol.h"
enum class STATE : unsigned int { Free, Alloc, Ingame, Start };
class Session
{
public:
	short getPosx();
	short getPosy();
	int getId();
	SOCKET getSocket();
	char* getName();

	void setPosx(short x);
	void setPosy(short y);
	void setId(int id);
	void setSocket(SOCKET socket);
	void setName(char* name);

	void doRecv();
	void doSend(void* packet); 

public:
	virtual void move() = 0;
	virtual void attack() = 0;
	void sendLoginPacket();
	void sendAddPacket(const Session& client);
	void sendMoverPlayerPacket(const Session& client);

private:
	OVERLAPPED_EX _s_over;
	short _x;
	short _y;
	int _id = -1;
	SOCKET _socket;
	char _name[NAME_SIZE];


public:
	STATE _state;
	mutex _s_lock;
	int _prevremain;

};
class Player : public Session
{
public:
	void move() override;
	void attack() override;
private:

};
class Monster : public Session
{

public:
	void move() override;
	void attack() override;

private:
	Player** _chasetarget;

};


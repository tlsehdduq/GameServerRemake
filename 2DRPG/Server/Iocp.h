#pragma once
#include"Session.h"
#include"Timer.h"
class Iocp
{
public:
	Iocp();
	~Iocp();

	void Run();
	void Stop();

	void WorkerThread();
	void ProcessPacket(int id, char* packet);
	void disconnect();
	
	int CreateId();
	void InitializedMonster();
	void NpcOn(int npcid,int clid);

public:
	SOCKET GetListenSocket();
	SOCKET GetClientSocket();
	void SetListenSocket(SOCKET socket);
	void SetClientSocket(SOCKET socket);
	HANDLE GetIocpHandle();
	void SetIocpHandle(HANDLE handle);


private:
	SOCKET CreateSocket();
	bool SetLinger(SOCKET socket, UINT16 onoff, UINT16 linger);
	bool SetReuseAddress(SOCKET socket, bool flag);
	bool SetTcpNoDelay(SOCKET socket);

private:

	SOCKET _listensocket;
	SOCKET _clientsocket;
	OVERLAPPED_EX _over;
	HANDLE _iocphandle;
	Timer _timer;

	std::chrono::system_clock::time_point start_time;

public:

	thread _timerthread;
	vector<thread> _workerthread;
	array<Player, MAX_USER> _clients;
	array<Monster, MAX_NPC> _npcs;
};


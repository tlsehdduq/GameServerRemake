#pragma once
#include"Session.h"
//enum class COMP_TYPE : char { Accept, Recv, Send };
// Overlapped	
//class OVERLAPPED_EX
//{
//public:
//	OVERLAPPED_EX()
//	{
//		_wsaBuf.buf = _sendbuf;
//		_wsaBuf.len = 200;
//		_type = COMP_TYPE::Recv;
//		ZeroMemory(&_over, sizeof(_over));
//
//	}
//	OVERLAPPED_EX(char* packet)
//	{
//		_wsaBuf.buf = _sendbuf;
//		_wsaBuf.len = packet[0];
//		ZeroMemory(&_over, sizeof(_over));
//		_type = COMP_TYPE::Send;
//		memcpy(_sendbuf, packet, _wsaBuf.len);
//	}
//public:
//	WSAOVERLAPPED _over;
//	WSABUF _wsaBuf;
//	char _sendbuf[200];
//	COMP_TYPE _type;
//};
class Iocp
{
public:
	Iocp();
	~Iocp();

	void Run();
	void Stop();

	void WorkerThread();
	void ProcessPacket();
	void disconnect();

	int Getclientid();
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

public:

	thread _timerthread;
	vector<thread> _workerthread;
	array<Session, MAX_USER> _clients;

	
};


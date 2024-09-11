#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <concurrent_priority_queue.h>
#include <sqlext.h>  
#include <string>
#include <random>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

// Overlapped	
enum class COMP_TYPE : char { Accept, Recv, Send };
 //Overlapped	
class OVERLAPPED_EX
{
public:
	OVERLAPPED_EX()
	{
		_wsaBuf.buf = _sendbuf;
		_wsaBuf.len = 200;
		_type = COMP_TYPE::Recv;
		ZeroMemory(&_over, sizeof(_over));

	}
	OVERLAPPED_EX(char* packet)
	{
		_wsaBuf.buf = _sendbuf;
		_wsaBuf.len = packet[0];
		ZeroMemory(&_over, sizeof(_over));
		_type = COMP_TYPE::Send;
		memcpy(_sendbuf, packet, _wsaBuf.len);
	}
public:
	WSAOVERLAPPED _over;
	WSABUF _wsaBuf;
	char _sendbuf[200];
	COMP_TYPE _type;
};


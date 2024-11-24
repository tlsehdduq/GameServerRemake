#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_set>
#include<unordered_map>
#include <concurrent_priority_queue.h>
#include <sqlext.h>  
#include <string>
#include <random>
#include <set>
#include<queue>
#include<memory>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;
// Overlapped	
enum class COMP_TYPE : char { Accept, Recv, Send, NPC_UPDATE, NPC_INITIALIZED, END_ATTACK };
enum class EVENT_TYPE : char { EV_INIT, EV_ATTACK, EV_NPC_MOVE };
enum class MAP_SECTION : char { RIGHT_UP, LEFT_UP, RIGHT_DOWN, LEFT_DOWN , FREE};
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
	int target_id;
	COMP_TYPE _type;
};

struct TimerEvent {
	std::chrono::system_clock::time_point wakeupTime;
	int n_id; // monster id 
	int c_id; // client id  
	EVENT_TYPE evtype;
	constexpr bool operator < (const TimerEvent& L) const {
		return (wakeupTime > L.wakeupTime);
	}
	TimerEvent() {}

	TimerEvent(std::chrono::system_clock::time_point w_t, int id, EVENT_TYPE e_id) : wakeupTime(w_t), c_id(id), evtype(e_id)
	{
	}

	TimerEvent(std::chrono::system_clock::time_point w_t, int nid, int cid, EVENT_TYPE e_id) : wakeupTime(w_t), n_id(nid), c_id(cid), evtype(e_id)
	{
	}
};

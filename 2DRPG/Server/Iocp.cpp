#include "stdafx.h"
#include "Iocp.h"

extern array<Player, MAX_USER> _clients;
extern array<Monster, MAX_NPC> _npcs;
extern HANDLE _iocphandle;

Iocp::Iocp()
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
	_listensocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT_NUM);

	if (SOCKET_ERROR == bind(_listensocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)))
		cout << " Bind Error " << endl;
	if (SOCKET_ERROR == listen(_listensocket, SOMAXCONN))
		cout << " Listen Error " << endl;
	if (_listensocket == INVALID_SOCKET)
		cout << " Falied to Create listen socket " << endl;
	else
	{
		int optval;
		int optlen = sizeof(optval);
		if (getsockopt(_listensocket, SOL_SOCKET, SO_TYPE, reinterpret_cast<char*>(&optval), &optlen) == SOCKET_ERROR)
			cout << " Falied to get listen socket option " << endl;

		else
			cout << " Listen socket options retrieved successfully " << endl;

		sockaddr_in addr;
		int addrlen = sizeof(addr);
		if (getsockname(_listensocket, reinterpret_cast<sockaddr*>(&addr), &addrlen) == SOCKET_ERROR)
			cout << "Failed to get local address of listen socket" << endl;
		else
			cout << " Listen socket is bound to port " << ntohs(addr.sin_port) << endl;

	}
}

Iocp::~Iocp()
{

}

void Iocp::Run()
{
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	_iocphandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listensocket), _iocphandle, 9999, 0);

	_clientsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	LINGER option;
	option.l_linger = 0;
	option.l_onoff = 1;
	setsockopt(_clientsocket, SOL_SOCKET, SO_LINGER, (const char*)&option, sizeof(option));
	_over._type = COMP_TYPE::Accept;

	BOOL ret = AcceptEx(_listensocket, _clientsocket, _over._sendbuf, 0, addr_size + 16, addr_size + 16, 0, &_over._over);
	if (FALSE == ret)
	{
		int err = WSAGetLastError();
		if (ERROR_IO_PENDING != err)
		{
			cout << err << "  - Error " << endl;
		}
	}
	start_time = chrono::system_clock::now();
	InitializedMonster();
	int num_thread = thread::hardware_concurrency();
	for (int i = 0; i < num_thread; ++i)
		_workerthread.emplace_back(&Iocp::WorkerThread, this);


	for (auto& th : _workerthread)
		th.join();
}

void Iocp::Stop()
{
}

void Iocp::WorkerThread()
{
	while (true)
	{
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(_iocphandle, &num_bytes, &key, &over, INFINITE);
		OVERLAPPED_EX* over_ex = reinterpret_cast<OVERLAPPED_EX*>(over);

		if (FALSE == ret)
		{
			if (over_ex->_type == COMP_TYPE::Accept)cout << " Accept Error ";
			else
			{
				cout << " GQCS error on client [ " << key << "]\n";
				disconnect();
				if (over_ex->_type == COMP_TYPE::Send)delete over_ex;
				continue;
			}
		}
		if ((0 == num_bytes) && ((over_ex->_type == COMP_TYPE::Recv) || (over_ex->_type == COMP_TYPE::Send)))
		{
			disconnect();
			if (over_ex->_type == COMP_TYPE::Send)delete over_ex;
			continue;
		}
		switch (over_ex->_type)
		{
		case COMP_TYPE::Accept: {
			int client_id = CreateId();
			if (client_id != -1)
			{
				// 정보저장 
				{
					lock_guard<mutex>ll(_clients[client_id]._s_lock);
					_clients[client_id]._state = STATE::Alloc;
				}
				_clients[client_id].setPosx(0);
				_clients[client_id].setPosy(0);
				_clients[client_id].setId(client_id);
				_clients[client_id].setSocket(_clientsocket);
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(_clientsocket), _iocphandle, client_id, 0);
				if (_iocphandle == INVALID_HANDLE_VALUE) {
					cout << " Handle error " << endl;
					break;
				}
				//recv
				_clients[client_id].doRecv();
				_clientsocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else
				cout << " MAX USER " << endl;
			ZeroMemory(&_over._over, sizeof(_over._over));
			int addrsize = sizeof(SOCKADDR_IN);
			AcceptEx(_listensocket, _clientsocket, _over._sendbuf, 0, addrsize + 16, addrsize + 16, 0, &_over._over);
		}
							  break;
		case COMP_TYPE::Send: {
			delete over_ex;
		}
							break;
		case COMP_TYPE::Recv: {
			// 패킷 재조립 
			int remain_data = num_bytes + _clients[key]._prevremain;
			char* p = over_ex->_sendbuf;
			while (remain_data > 0)
			{
				int packetsize = p[0];
				if (packetsize <= remain_data)
				{
					ProcessPacket(static_cast<int>(key), p);
					p = p + packetsize;
					remain_data = remain_data - packetsize;

				}
				else break;
			}
			_clients[key]._prevremain = remain_data;
			if (remain_data > 0)
				memcpy(over_ex->_sendbuf, p, remain_data);
			_clients[key].doRecv();
		}
							break;
		case COMP_TYPE::NPC_UPDATE: {
			bool keepalive = false;
			for (int j = 0; j < MAX_USER; ++j)
			{
				if (_clients[j]._state != STATE::Ingame)continue;
				if (_clients[j].can_see(j, over_ex->target_id, 2)) //시야 거리에?  
				{
					keepalive = true;
					break;
				}
			}
			if (keepalive) {
				int c_id = static_cast<int>(key);
				// 몬스터의 시야거리에 플레이어가 있다면 ? move 
				_npcs[over_ex->target_id].move();
				TimerEvent ev{ std::chrono::system_clock::now() + std::chrono::seconds(1s),over_ex->target_id,c_id,EVENT_TYPE::EV_NPC_MOVE };
				_timer.InitTimerQueue(ev);
			}

			delete over_ex;
		}
			break;

		case COMP_TYPE::END_ATTACK: {
			for (auto& pl : _clients)
			{
				if (pl.getId() == key) continue;
				pl.sendAttack(key, false);
			}
			delete over_ex;
		}
								  break;
		default:
			break;
		}
	}
}

void Iocp::ProcessPacket(int id, char* packet)
{
	switch (packet[1])
	{

	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		//_clients[id].setId(p->id);
		short r_x = rand() % 1000;
		short r_y = rand() % 1000;
		_clients[id].setName(p->name);
		{
			lock_guard<mutex>ll{ _clients[id]._s_lock };
			_clients[id]._state = STATE::Ingame;
			_clients[id].setPosx(r_x);
			_clients[id].setPosy(r_y);
		}
		// 다시 보내야함 
		_clients[id].sendLoginPacket();
		for (auto& pl : _clients)
		{
			{
				lock_guard<mutex>ll(pl._s_lock);
				if (pl._state != STATE::Ingame)continue;
			}
			// SEND ADD PACKET 
			if (pl.getId() == id)continue;
			pl.sendAddPacket(_clients[id]);
			_clients[id].sendAddPacket(pl);
		}
		// 이것도 수정 
		for (auto& npc : _npcs)
		{
			if (npc.isalive == false)continue;
			if (_clients[id].can_see(id, npc.getId(), 2)) {
				_clients[id].sendMonsterInit(npc.getId()); // NPC의 모든 정보들을 전송? 시야거리에 있는 애들만 전송 
				NpcMoveOn(npc.getId(), id); // NPC 타이머 작동 
			}
		}
	}
				 break;
	case CS_ATTACK: {
		_clients[id].attack();
		for (auto& pl : _clients)
		{
			if (pl.getId() == -1)break;
			if (pl.getId() == id)continue;
			pl.sendAttack(id,true);
			TimerEvent ev{ std::chrono::system_clock::now() + std::chrono::milliseconds(100ms),id,EVENT_TYPE::EV_ATTACK};
			_timer.InitTimerQueue(ev);
		}
	}
				  break;
	case CS_CHAT: {

	}
				break;
	case CS_MOVE_PLAYER: {
		CS_MOVE_PLAYER_PACKET* p = reinterpret_cast<CS_MOVE_PLAYER_PACKET*>(packet);
		_clients[id].setMoveTime(p->move_time);
		_clients[id].move(p->dir);
		if (p->dir == 2)
			_clients[id]._leftright = true;
		else if (p->dir == 3)
			_clients[id]._leftright = false;

		unordered_set<int> nearvlist;
		_clients[id]._vl.lock();
		unordered_set<int> oldvlist = _clients[id].player_view_list;
		_clients[id]._vl.unlock();

		for (auto& pl : _clients)
		{
			if (pl._state != STATE::Ingame)continue;
			if (pl.getId() == id)continue;
			if (_clients[id].can_see(id, pl.getId(), 1)) // 1 : client client 2 : client monster
				nearvlist.insert(pl.getId());
		}
		_clients[id].sendMoverPlayerPacket(_clients[id]); // 나한테 내가 움직인걸 보낸다 .

		for (auto& pl : nearvlist)
		{
			auto& cl = _clients[pl];
			cl._vl.lock();
			if (_clients[pl].player_view_list.count(id)) {
				cl._vl.unlock();
				_clients[pl].sendMoverPlayerPacket(_clients[id]);
			}
			else
			{
				cl._vl.unlock();
				_clients[pl].sendAddPacket(_clients[id]);
			}
			if (oldvlist.count(pl) == 0)
				_clients[id].sendAddPacket(_clients[pl]);//  ? 
		}
		for (auto& pl : oldvlist)
		{
			if (nearvlist.count(pl) == 0)
			{
				_clients[id].sendRemovePacket(pl, 1);
				_clients[pl].sendRemovePacket(id, 1);
			}
		}
		// --------------------------------npc viewlist--------------------------------------------

		unordered_set<int> nearvnpclist;
		_clients[id]._vl.lock();
		unordered_set<int> oldvnpclist = _clients[id].monster_view_list;
		_clients[id]._vl.unlock();

		for (auto& npc : _npcs)
		{
			if (_clients[id].can_see(id, npc.getId(), 2) == true)
			{
				nearvnpclist.insert(npc.getId());
			}
		}
		for (auto npcid : nearvnpclist)
		{
			if (_clients[id].monster_view_list.count(npcid) != 0) // 이미 내 viewlist에 몬스터가 있다면? 행동 
			{
				NpcMoveOn(npcid, id);
			}
			else
				_clients[id].sendMonsterInit(npcid); // 없다면 추가부터 
			if (oldvnpclist.count(npcid) == 0)
				_clients[id].sendMonsterInit(npcid); // 없다면 추가부터 
		}
		for (auto npcid : oldvnpclist)
		{
			if (nearvnpclist.count(npcid) == 0)
			{
				_npcs[npcid].isalive = false;
				_clients[id].sendRemovePacket(npcid,2);
			}
		}
	}
					   break;
	}
}

void Iocp::disconnect()
{
}

int Iocp::CreateId()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (_clients[i].getId() == -1)return i;
	}
	cout << " MAX USER " << endl;
	return -1;
}

void Iocp::InitializedMonster() // 몬스터 랜덤 좌표지정 
{
	default_random_engine dre;
	uniform_int_distribution<int> uid{ 0, 1000 };

	for (int i = 0; i < MAX_NPC; ++i)
	{
		//_npcs[i].setPosx(3);
		_npcs[i].setPosx(uid(dre));
		//_npcs[i].setPosy(2);
		_npcs[i].setPosy(uid(dre));
		_npcs[i].setId(i);
		_npcs[i].setHp(100);
		_npcs[i].initClosedList();
	}

	// 여기서 몬스터 정보 전송? 

}

void Iocp::NpcMoveOn(int npcid, int id)
{
	if (_npcs[npcid].getHp() <= 0)return;
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&_npcs[npcid].isalive, &old_state, true))return;

	// false -> true 타이머 실행 
	TimerEvent ev{ chrono::system_clock::now(),npcid,id,EVENT_TYPE::EV_NPC_MOVE };
	_timer.InitTimerQueue(ev);
}

SOCKET Iocp::GetListenSocket()
{
	return SOCKET();
}

SOCKET Iocp::GetClientSocket()
{
	return SOCKET();
}

void Iocp::SetListenSocket(SOCKET socket)
{
}

void Iocp::SetClientSocket(SOCKET socket)
{
}

HANDLE Iocp::GetIocpHandle()
{
	return HANDLE();
}

void Iocp::SetIocpHandle(HANDLE handle)
{
}

SOCKET Iocp::CreateSocket()
{
	return SOCKET();
}

bool Iocp::SetLinger(SOCKET socket, UINT16 onoff, UINT16 linger)
{
	return false;
}

bool Iocp::SetReuseAddress(SOCKET socket, bool flag)
{
	return false;
}

bool Iocp::SetTcpNoDelay(SOCKET socket)
{
	return false;
}

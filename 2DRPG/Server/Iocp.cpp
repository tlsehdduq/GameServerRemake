#include "stdafx.h"
#include "Iocp.h"

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
	int num_thread = thread::hardware_concurrency();
	for (int i = 0; i < num_thread; ++i)
		_workerthread.emplace_back(&Iocp::WorkerThread, this);

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
			int client_id = Getclientid();
			if (client_id != -1)
			{
				// 정보저장 
				{
					lock_guard<mutex>ll(_clients[client_id]._s_lock);
					_clients[client_id]._state = STATE::Alloc;

				}
				std::default_random_engine dre;
				std::uniform_real_distribution<float> r_x; // 맵 사이즈를 정해야함 
				std::uniform_real_distribution<float> r_y; // 맵 사이즈를 정해야함 
				_clients[client_id].setPosx(r_x(dre));
				_clients[client_id].setPosy(r_y(dre));
				//_clients[client_id].setName()
				_clients[client_id].setId(client_id);
				_clients[client_id].setSocket(_clientsocket);
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(_clientsocket), _iocphandle, client_id, 0);
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
					// ProcessPacket();
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
		}
	}
}

void Iocp::ProcessPacket()
{
}

void Iocp::disconnect()
{
}

int Iocp::Getclientid()
{
	return 0;
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

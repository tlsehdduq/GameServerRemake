#include"stdafx.h"
#include "Session.h"
extern array<Player, MAX_USER> _clients;
extern array<Monster, MAX_NPC> _npcs;
short Session::getPosx()
{
	return _x;
}

short Session::getPosy()
{
	return _y;
}

int Session::getId()
{
	return _id;
}

SOCKET Session::getSocket()
{
	return _socket;
}

char* Session::getName()
{
	return _name;
}

int Session::getMoveTime()
{
	return _last_move_time;
}

void Session::setPosx(short x)
{
	_x = x;
}

void Session::setPosy(short y)
{
	_y = y;
}

void Session::setId(int id)
{
	_id = id;
}

void Session::setSocket(SOCKET socket)
{
	_socket = socket;
}

void Session::setName(char* name)
{
	strcpy_s(_name, name);
}

void Session::setMoveTime(int time)
{
	_last_move_time = time;
}

void Session::doRecv()
{
	DWORD recv_flag = 0;
	memset(&_s_over._over, 0, sizeof(_s_over._over));
	_s_over._wsaBuf.len = BUF_SIZE - _prevremain;
	_s_over._wsaBuf.buf = _s_over._sendbuf + _prevremain;
	WSARecv(_socket, &_s_over._wsaBuf, 1, 0, &recv_flag, &_s_over._over, 0);
}

void Session::doSend(void* packet)
{
	OVERLAPPED_EX* sendover = new OVERLAPPED_EX{ reinterpret_cast<char*>(packet) };
	WSASend(_socket, &sendover->_wsaBuf, 1, 0, 0, &sendover->_over, 0);
}

bool Session::can_see(int from, int to, int type)
{
	if (type == 1) {
		if (abs(_clients[from].getPosx() - _clients[to].getPosx()) > _viewrange)return false;
		return abs(_clients[from].getPosy() - _clients[to].getPosy()) <= _viewrange;
	}
	else
	{
		if (abs(_clients[from].getPosx() - _npcs[to].getPosx()) > _viewrange)return false;
		return abs(_clients[from].getPosy() - _npcs[to].getPosy()) <= _viewrange;
	}
}

void Session::sendLoginPacket()
{
	SC_LOGIN_PACKET p;
	p.size = sizeof(SC_LOGIN_PACKET);
	p.type = SC_LOGIN;
	p.id = _id;
	p.x = 0;
	p.y = 0;
	p.max_hp = 50;
	p.hp = 50;
	p.level = 1;
	doSend(&p);
}

void Session::sendAddPacket(const Session& client)
{
	SC_ADD_PACKET p;
	p.size = sizeof(SC_ADD_PACKET);
	p.type = SC_ADD_OBJECT;
	p.id = client._id;
	p.x = client._x;
	p.y = client._y;
	memcpy(p.name, client._name, sizeof(p.name));
	doSend(&p);
}

void Session::sendMoverPlayerPacket(const Session& client)
{
	SC_MOVE_PLAYER_PACKET p;
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.id = client._id;
	p.x = client._x;
	p.y = client._y;
	p.move_time = client._last_move_time;
	doSend(&p);
}

void Player::sendMonsterInit(Monster& monster)
{
	SC_MONTSER_INIT_PACKET p;
	p.size = sizeof(SC_MONTSER_INIT_PACKET);
	p.type = SC_MONTSER_INIT;
	p.x = monster.getPosx();
	p.y = monster.getPosy();
	p.id = monster.getId();
	doSend(&p);
}

void Player::sendMonsterMove(Monster& monster)
{
	SC_MONSTER_MOVE_PACKET p;
	p.size = sizeof(SC_MONSTER_MOVE_PACKET);
	p.type = SC_MONSTER_MOVE;
	p.id = monster.getId();
	p.x = monster.getPosx();
	p.y = monster.getPosy();
	doSend(&p);
}

void Monster::move()
{
	//unordered_set<int> _oldvl;
	//for (auto& cl : _clients)
	//{
	//	if (STATE::Ingame != cl._state)continue;
	//	if (can_see(cl.getId(), getId(), 2) == true)
	//	{
	//		_oldvl.insert(cl.getId());
	//	}
	//}
	switch (rand() % 4)
	{
	case 0:
		if (getPosx() < 0 || getPosx() > 500)break;
		setPosx(getPosx() + 1);
		break;
	case 1:
		if (getPosx() < 0 || getPosx() > 500)break;
		setPosx(getPosx() - 1);
		break;
	case 2:
		if (getPosy() < 0 || getPosy() > 500)break;
		setPosy(getPosy() + 1);
		break;
	case 3:
		if (getPosy() < 0 || getPosy() > 500)break;
		setPosy(getPosy() - 1);
		break;
	}
	//unordered_set<int> _newvl;
	//for (auto& cl : _clients)
	//{
	//	if (STATE::Ingame != cl._state)continue;
	//	if (can_see(cl.getId(), getId(), 2) == true)
	//	{
	//		_newvl.insert(cl.getId());
	//	}
	//}

}

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
	p.x = _clients[_id].getPosx();
	p.y = _clients[_id].getPosy();
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

	_vl.lock();
	player_view_list.insert(client._id);
	_vl.unlock();
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

void Session::sendRemovePacket(int id, int type)
{
	_vl.lock();
	if (player_view_list.count(id))
		player_view_list.erase(id);
	else {
		_vl.unlock();
		return;
	}
		_vl.unlock();
	SC_REMOVE_PACKET p;
	p.size = sizeof(SC_REMOVE_PACKET);
	p.type = SC_REMOVE;
	p.id = id;
	p.sessiontype = type; // 1 : clients 2 : monster 

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

	_vl.lock();
	monster_view_list.insert(monster.getId()); // 몬스터가 생길때 플레이어에게 몬스터의 정보를 저장 
	_vl.unlock();
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

void Monster::move(Player& client)
{
	// move를 하는데 여기서 몬스터의 시야거리에 있는 플레이어들에게만 send를 해야함 viewlist 
	unordered_set<int> _prevvl;
	_vl.lock();
	_prevvl = player_view_list;
	_vl.unlock();

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
	}// 몬스터 이동 

	unordered_set<int> _nearvl;
	for (auto& pl : _clients)
	{
		if (pl._state != STATE::Ingame)continue;
		if (can_see(pl.getId(), getId(), 2) == true)
		{
			_nearvl.insert(pl.getId());
		}
	}
	for (auto& pl : _nearvl)
	{
		auto& cl = _clients[pl];
		cl._vl.lock();
		if (_npcs[getId()].player_view_list.count(pl))
		{
			cl._vl.unlock();
			_clients[pl].sendMonsterMove(_npcs[getId()]);
		}
		else
		{
			cl._vl.unlock();
			_clients[pl].sendMonsterInit(_npcs[getId()]);
		}
		if (_prevvl.count(pl) == 0) {}
	}
}

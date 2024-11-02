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

int Session::getHp()
{
	return _hp;
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

void Session::setHp(int hp)
{
	_hp = hp;
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
	p.left = client._leftright;
	doSend(&p);
}

void Session::sendRemovePacket(int id, int type)
{
	//_vl.lock();
	//if (player_view_list.count(id))
	//	player_view_list.erase(id);
	//else {
	//	_vl.unlock();
	//	return;
	//}
	//_vl.unlock();
	SC_REMOVE_PACKET p;
	p.size = sizeof(SC_REMOVE_PACKET);
	p.type = SC_REMOVE;
	p.id = id;
	p.sessiontype = type; // 1 : clients 2 : monster 

	doSend(&p);
}

void Player::move(int dir)
{
	_dir = dir;

	switch (dir) {
	case 0:
		if (getPosy() <= 0) setPosy(0);
		else
			setPosy(getPosy() - 1);
		break;
	case 1:
		if (getPosy() >= 1000)setPosy(1000);
		else
			setPosy(getPosy() + 1);
		break;
	case 2:
		if (getPosx() <= 0)setPosx(0);
		else
			setPosx(getPosx() - 1);
		break;
	case 3:
		if (getPosx() >= 1000)setPosx(1000);
		else
			setPosx(getPosx() + 1);
		break;
	}
}

void Player::attack()
{
	for (auto& npc : monster_view_list)
	{
		//양옆에 있음 , 
		switch (_dir) {
		case 2: // 왼쪽을 바라보고 있을 때 
		{
			if (canatt(getId(), npc) && getPosx() - _npcs[npc].getPosx() >= 0)
			{
				cout << npc << "  우측 에서 몬스터 공격 " << endl;
				_npcs[npc].setHp(_npcs[npc].getHp() - 10);
				if (_npcs[npc].getHp() <= 0)
				{
					_npcs[npc].isalive = false;
					sendRemovePacket(npc, 2);
				}
			}
		}break;
		case 3: // 오른쪽을 바라보고 있을 떄
		{
			if (canatt(getId(), npc) && getPosx() - _npcs[npc].getPosx() <= 0)
			{
				cout << npc << "  좌측  에서 몬스터 공격 " << endl;
				_npcs[npc].setHp(_npcs[npc].getHp() - 10);
				if (_npcs[npc].getHp() <= 0)
				{
					_npcs[npc].isalive = false;
					sendRemovePacket(npc, 2);
				}
			}
		}break;
		}
	}
}

bool Player::canatt(int from, int to)
{
	if (_clients[from].getPosy() == _npcs[to].getPosy()) {
		// 공격은 양옆으로만 y값은 같아야함 
		if (abs(_clients[from].getPosx() - _npcs[to].getPosx()) != 1)return false;
		return true;
	}
	else return false;

}

void Player::sendMonsterInit(int id)
{
	SC_MONTSER_INIT_PACKET p;
	p.size = sizeof(SC_MONTSER_INIT_PACKET);
	p.type = SC_MONTSER_INIT;
	p.x = _npcs[id].getPosx();
	p.y = _npcs[id].getPosy();
	p.id = id;
	doSend(&p);

	_vl.lock();
	monster_view_list.insert(id); // 몬스터가 생길때 플레이어에게 몬스터의 정보를 저장 
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

void Player::sendMonsterRemove(int id)
{
	SC_MONSTER_REMOVE_PACKET p;
	p.size = sizeof(SC_MONSTER_REMOVE_PACKET);
	p.type = SC_MONSTER_REMOVE;
	p.id = id;

	doSend(&p);
}

void Player::sendAttack(int id, bool onoff)
{
	SC_PLAYER_ATTACK_PACKET p;
	p.size = sizeof(SC_PLAYER_ATTACK_PACKET);
	p.type = SC_PLAYER_ATTACK;
	p.id = id;
	p.onoff = onoff;
	doSend(&p);
}

void Monster::move()
{
	// move를 하는데 여기서 몬스터의 시야거리에 있는 플레이어들에게만 send를 해야함 viewlist 
	// 시야거리에 있다면? 플레이어를 추격해야함 
	unordered_set<int> _prevvl;
	int myid = getId();
	int traceclid = -1;
	for (auto& pl : _clients)
	{
		if (pl._state != STATE::Ingame)continue; //움직이기 이전 주변애들 파악 
		if (pl.can_see(pl.getId(), myid, 2))
		{
			_prevvl.insert(pl.getId());
			traceclid = pl.getId();
		}
	}

	//switch (rand() % 4)
	//{
	//case 0:
	//	if (getPosx() < 0 || getPosx() > 500)break;
	//	setPosx(getPosx() + 1);
	//	break;
	//case 1:
	//	if (getPosx() < 0 || getPosx() > 500)break;
	//	setPosx(getPosx() - 1);
	//	break;
	//case 2:
	//	if (getPosy() < 0 || getPosy() > 500)break;
	//	setPosy(getPosy() + 1);
	//	break;
	//case 3:
	//	if (getPosy() < 0 || getPosy() > 500)break;
	//	setPosy(getPosy() - 1);
	//	break;
	//}// 몬스터 이동 

	moveTowardsPlayer(_clients[traceclid].getPosx(), _clients[traceclid].getPosy());

	unordered_set<int> _nearvl;
	for (auto& pl : _clients)
	{
		if (pl._state != STATE::Ingame)continue;
		if (can_see(pl.getId(), myid, 2) == true)
		{
			_nearvl.insert(pl.getId());
		}
	}
	for (auto pl : _nearvl)
	{
		if (_prevvl.count(pl) == 0)
			_clients[pl].sendMonsterInit(myid);
		else
			_clients[pl].sendMonsterMove(_npcs[myid]);
	}

	for (auto pl : _prevvl)
	{
		if (_nearvl.count(pl) == 0)
		{
			_clients[pl]._vl.lock();
			if (_clients[pl].monster_view_list.count(myid) == 0) {
				_clients[pl]._vl.unlock();
				_clients[pl].sendRemovePacket(myid, 2);
			}
			else
				_clients[pl]._vl.unlock();
		}
	}
}

void Monster::moveTowardsPlayer(short playerx, short playery)
{
	Astar pathfinder;
	std::vector<AstarNode> path = pathfinder.findpath(map, _x, _y, playerx, playery);

	if (!path.empty()) {
		AstarNode nextStep = path.front(); // 다음 이동할 위치
		setPosx(nextStep._x);
		setPosy(nextStep._y);
		// 이동 후 상태 업데이트
	}
}


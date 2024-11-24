#include"stdafx.h"
#include "Session.h"
extern array<Player, MAX_USER> _clients;
extern array<Monster, MAX_NPC> _npcs;

extern unordered_set<int> _rightupSection;
extern unordered_set<int> _rightdownSection;
extern unordered_set<int> _leftupSection;
extern unordered_set<int> _leftdownSection;

extern unordered_set<int> _rightupNpcSection;
extern unordered_set<int> _rightdownNpcSection;
extern unordered_set<int> _leftupNpcSection;
extern unordered_set<int> _leftdownNpcSection;

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
		if (abs(_clients[from]._x - _clients[to]._x) > _viewrange)return false;
		return abs(_clients[from]._y - _clients[to]._y) <= _viewrange;
	}
	else
	{
		if (abs(_clients[from]._x - _npcs[to]._x) > _viewrange)return false;
		return abs(_clients[from]._y - _npcs[to]._y) <= _viewrange;
	}
}

void Session::setSection(bool isnpc)
{
	_sectionlock.lock();
	if (!isnpc) {
		if (_x > MAP_X_HALF && _y < MAP_Y_HALF) {

			if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownSection.erase(_id);
				_section = MAP_SECTION::RIGHT_UP;
				_rightupSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_UP) {
				_leftdownSection.erase(_id);
				_section = MAP_SECTION::RIGHT_UP;
				_rightupSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_DOWN) {
				_leftupSection.erase(_id);
				_section = MAP_SECTION::RIGHT_UP;
				_rightupSection.insert(_id);
			}
			_sectionlock.unlock();
		}
		else if (_x > MAP_X_HALF && _y > MAP_Y_HALF) {
			if (_section == MAP_SECTION::RIGHT_UP) {
				_rightupSection.erase(_id);
				_section = MAP_SECTION::RIGHT_DOWN;
				_rightdownSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_UP) {
				_leftdownSection.erase(_id);
				_section = MAP_SECTION::RIGHT_DOWN;
				_rightdownSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_DOWN) {
				_leftupSection.erase(_id);
				_section = MAP_SECTION::RIGHT_DOWN;
				_rightdownSection.insert(_id);
			}
			_sectionlock.unlock();
		}
		else if (_x < MAP_X_HALF && _y < MAP_Y_HALF) {

			if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownSection.erase(_id);
				_section = MAP_SECTION::LEFT_UP;
				_leftupSection.insert(_id);
			}
			else if (_section == MAP_SECTION::RIGHT_UP) {
				_rightupSection.erase(_id);
				_section = MAP_SECTION::LEFT_UP;
				_leftupSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_DOWN) {
				_leftupSection.erase(_id);
				_section = MAP_SECTION::LEFT_UP;
				_leftupSection.insert(_id);
			}
			_sectionlock.unlock();
		}
		else {

			if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownSection.erase(_id);
				_section = MAP_SECTION::LEFT_DOWN;
				_leftdownSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_UP) {
				_leftdownSection.erase(_id);
				_section = MAP_SECTION::LEFT_DOWN;
				_leftdownSection.insert(_id);
			}
			else if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownSection.erase(_id);
				_section = MAP_SECTION::LEFT_DOWN;
				_leftdownSection.insert(_id);
			}
			_sectionlock.unlock();
		}
	}
	else {

		if (_x > MAP_X_HALF && _y < MAP_Y_HALF) {

			if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownNpcSection.erase(_id);
				_section = MAP_SECTION::RIGHT_UP;
				_rightupNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_UP) {
				_leftdownNpcSection.erase(_id);
				_section = MAP_SECTION::RIGHT_UP;
				_rightupNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_DOWN) {
				_leftupNpcSection.erase(_id);
				_section = MAP_SECTION::RIGHT_UP;
				_rightupNpcSection.insert(_id);
			}
			_sectionlock.unlock();

		}
		else if (_x > MAP_X_HALF && _y > MAP_Y_HALF) {
			if (_section == MAP_SECTION::RIGHT_UP) {
				_rightupNpcSection.erase(_id);
				_section = MAP_SECTION::RIGHT_DOWN;
				_rightdownNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_UP) {
				_leftdownNpcSection.erase(_id);
				_section = MAP_SECTION::RIGHT_DOWN;
				_rightdownNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_DOWN) {
				_leftupNpcSection.erase(_id);
				_section = MAP_SECTION::RIGHT_DOWN;
				_rightdownNpcSection.insert(_id);
			}
			_sectionlock.unlock();
		}
		else if (_x < MAP_X_HALF && _y < MAP_Y_HALF) {

			if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownNpcSection.erase(_id);
				_section = MAP_SECTION::LEFT_UP;
				_leftupNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::RIGHT_UP) {
				_rightupNpcSection.erase(_id);
				_section = MAP_SECTION::LEFT_UP;
				_leftupNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_DOWN) {
				_leftupNpcSection.erase(_id);
				_section = MAP_SECTION::LEFT_UP;
				_leftupNpcSection.insert(_id);
			}
			_sectionlock.unlock();
		}
		else {

			if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownNpcSection.erase(_id);
				_section = MAP_SECTION::LEFT_DOWN;
				_leftdownNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::LEFT_UP) {
				_leftdownNpcSection.erase(_id);
				_section = MAP_SECTION::LEFT_DOWN;
				_leftdownNpcSection.insert(_id);
			}
			else if (_section == MAP_SECTION::RIGHT_DOWN) {
				_rightdownNpcSection.erase(_id);
				_section = MAP_SECTION::LEFT_DOWN;
				_leftdownNpcSection.insert(_id);
			}
			_sectionlock.unlock();
		}
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
	_vl.lock();
	if (type == 1) {

		if (player_view_list.count(id)) {
			player_view_list.erase(id);
		}
		else {
			_vl.unlock();
			return;
		}
	}
	else
	{
		if (monster_view_list.count(id)) {
			monster_view_list.erase(id);
		}
		else {
			_vl.unlock();
			return;
		}
	}
	_vl.unlock();
	SC_REMOVE_PACKET p;
	p.size = sizeof(SC_REMOVE_PACKET);
	p.type = SC_REMOVE;
	p.id = id;
	p.sessiontype = type; // 1 : clients 2 : monster 

	doSend(&p);
}

void Session::sendChatPacket(char* message, int id)
{
	SC_CHAT_PACKET p;
	p.size = sizeof(SC_CHAT_PACKET);
	p.type = SC_CHAT;
	p.id = id;
	memcpy(p.message, message, sizeof(message));
	doSend(&p);
}

void Player::move(int dir)
{
	Astar& map = Astar::getInstance();

	_dir = dir;
	short p_x = getPosx();
	short p_y = getPosy();

	switch (dir) {
	case 0:
		
		if (p_y <= 0 || map._map[p_x][p_y - 1] == 1) setPosy(p_y);
		else {
			setPosy(getPosy() - 1);
		}
		break;
	case 1:
		if (p_y >= 999 || map._map[p_x][p_y + 1] == 1)setPosy(p_y);
		else
			setPosy(getPosy() + 1);
		break;
	case 2:
		if (p_x <= 0 || map._map[p_x - 1][p_y] == 1)setPosx(p_x);
		else
			setPosx(getPosx() - 1);
		break;
	case 3:
		
		if (p_x >= 999 || map._map[p_x + 1][p_y] == 1)setPosx(p_x);
		else
			setPosx(getPosx() + 1);
		break;
	}
}

void Player::attack()
{
	for (auto& npc : monster_view_list)
	{
		//�翷�� ���� , 
		switch (_dir) {
		case 2: // ������ �ٶ󺸰� ���� �� 
		{
			if (canatt(_id, npc) && getPosx() - _npcs[npc].getPosx() >= 0)
			{
				cout << npc << "  ���� ���� ���� ���� " << endl;
				_npcs[npc].setHp(_npcs[npc].getHp() - 10);
				if (_npcs[npc].getHp() <= 0)
				{
					_npcs[npc].isalive = false;
					sendRemovePacket(npc, 2);
					break;
				}
			}
		}break;
		case 3: // �������� �ٶ󺸰� ���� ��
		{
			if (canatt(_id, npc) && getPosx() - _npcs[npc].getPosx() <= 0)
			{
				cout << npc << "  ����  ���� ���� ���� " << endl;
				_npcs[npc].setHp(_npcs[npc].getHp() - 50);
				if (_npcs[npc].getHp() <= 0)
				{
					_npcs[npc].isalive = false;
					sendRemovePacket(npc, 2);
					break;
				}
			}
		}break;
		}
	}
}

bool Player::canatt(int from, int to)
{
	if (_clients[from].getPosy() == _npcs[to].getPosy()) {
		// ������ �翷���θ� y���� ���ƾ��� 
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
	monster_view_list.insert(id); // ���Ͱ� ���涧 �÷��̾�� ������ ������ ���� 
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

Monster::Monster()
{

}

void Monster::MonsterMove()
{
	// move�� �ϴµ� ���⼭ ������ �þ߰Ÿ��� �ִ� �÷��̾�鿡�Ը� send�� �ؾ��� viewlist 
	// �þ߰Ÿ��� �ִٸ�? �÷��̾ �߰��ؾ��� 
	unordered_set<int> _prevvl;
	int myid = getId();
	int traceclid = -1; // �߰� ID 
	int nearplayer = -1;

	MAP_SECTION prevsection = _section;

	switch (prevsection) {

	case MAP_SECTION::LEFT_DOWN: {
		for (auto& pl : _leftdownSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true) {
				_prevvl.insert(pl);
				traceclid = pl;
			}
		}
		break;
	}
	case MAP_SECTION::LEFT_UP: {
		for (auto& pl : _leftupSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true) {

				_prevvl.insert(pl);
				traceclid = pl;
			}
		}
		break;
	}
	case MAP_SECTION::RIGHT_DOWN: {
		for (auto& pl : _rightdownSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true) {

				_prevvl.insert(pl);
				traceclid = pl;
			}
		}
		break;
	}
	case MAP_SECTION::RIGHT_UP: {
		for (auto& pl : _rightupSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true) {
				_prevvl.insert(pl);
				traceclid = pl;
			}
		}
		break;
	}
	}

	if (traceclid == -1)return; // moveTowardsPlayer�� ȣ���� �ʹ�����. 
	moveTowardsPlayer(_clients[traceclid].getPosx(), _clients[traceclid].getPosy());
	//randommove();
	unordered_set<int> _nearvl; // ������ �Ŀ� �丮��Ʈ

	MAP_SECTION section = _section;
	switch (section) {

	case MAP_SECTION::LEFT_DOWN: {
		for (auto& pl : _leftdownSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true)
				_nearvl.insert(pl);
		}
		break;
	}
	case MAP_SECTION::LEFT_UP: {
		for (auto& pl : _leftupSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true)
				_nearvl.insert(pl);
		}
		break;
	}
	case MAP_SECTION::RIGHT_DOWN: {
		for (auto& pl : _rightdownSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true)
				_nearvl.insert(pl);
		}
		break;
	}
	case MAP_SECTION::RIGHT_UP: {
		for (auto& pl : _rightupSection) {
			if (_clients[pl]._state != STATE::Ingame)continue;
			if (can_see(pl, myid, 2) == true)
				_nearvl.insert(pl);
		}
		break;
	}
	}

	for (auto pl : _nearvl)
	{
		if (_prevvl.count(pl) == 0) // �������µ� �þ߰Ÿ��� ���Դ�? Init ������ �����־���? Move 
			_clients[pl].sendMonsterInit(myid);
		else
			_clients[pl].sendMonsterMove(_npcs[myid]);
	}

	for (auto pl : _prevvl) // ������ �þ߿� �ִ� �ֵ��߿��� 
	{
		if (_nearvl.count(pl) == 0) //�����̰� ������ �þ߰Ÿ��� �ֵ��� ����? 
		{
			_clients[pl]._vl.lock();
			if (_clients[pl].monster_view_list.count(myid) == 0) {
				_clients[pl]._vl.unlock();
				_clients[pl].sendRemovePacket(myid, 2); // ���� 
				isalive = false;
			}
			else
				_clients[pl]._vl.unlock();
		}
	}
}

void Monster::randommove()
{
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
	}// ���� �̵� 
}

void Monster::moveTowardsPlayer(const short playerx, const short playery)
{
	Astar& pathfinder = Astar::getInstance();

	vector<AstarNode> path = pathfinder.findpath(_x, _y, playerx, playery);

	if (!path.empty()) {
		AstarNode nextStep = path.front(); // ���� �̵��� ��ġ
		setPosx(nextStep._x);
		setPosy(nextStep._y);
		// �̵� �� ���� ������Ʈ
		setSection(true);
	}
}

int Monster::findNearPlayer(short tox, short toy, short fromx, short fromy)
{

	return 0;
}



#include"stdafx.h"
#include "Session.h"
extern array<Player, MAX_USER> _clients;
extern array<Monster, MAX_NPC> _npcs;

int Monster::_map[MAPSIZE][MAPSIZE] = { 0 };
bool Monster::_closedList[MAPSIZE][MAPSIZE] = { false };
int Monster::_cost[MAPSIZE][MAPSIZE] = { 0 };

std::array<std::pair<short, short>, 120> Session::puddle_positions{ {
		{10, 15}, {50, 100}, {90, 120}, {150, 200}, {200, 250},
	{30, 40}, {80, 90}, {130, 140}, {180, 190}, {240, 250},
	{70, 20}, {120, 30}, {170, 40}, {220, 50}, {270, 60},
	{300, 300}, {320, 350}, {400, 400}, {450, 450}, {480, 490},
	{20, 300}, {60, 320}, {100, 350}, {200, 400}, {300, 450},
	{50, 250}, {100, 260}, {150, 270}, {200, 280}, {250, 290},
	{400, 10}, {420, 60}, {440, 120}, {460, 180}, {480, 250},
	{50, 30}, {100, 70}, {150, 110}, {200, 150}, {250, 190},
	{300, 20}, {320, 70}, {340, 120}, {360, 170}, {380, 220},
	{400, 300}, {420, 350}, {440, 400}, {460, 450}, {490, 480},
	{30, 480}, {70, 440}, {110, 400}, {150, 360}, {190, 320},
	{210, 210}, {250, 250}, {290, 290}, {330, 330}, {370, 370},
	{15, 15}, {60, 60}, {105, 105}, {150, 150}, {195, 195},
	{240, 240}, {285, 285}, {330, 330}, {375, 375}, {420, 420},
	{460, 460}, {20, 450}, {40, 430}, {60, 410}, {80, 390},
	{250, 50}, {260, 100}, {270, 150}, {280, 200}, {290, 250},
	{300, 300}, {310, 350}, {320, 400}, {330, 450}, {340, 490},
	{400, 30}, {450, 50}, {480, 70}, {490, 90}, {490, 130},
	{50, 20}, {100, 50}, {150, 100}, {200, 150}, {250, 200},
	{300, 250}, {350, 300}, {400, 350}, {450, 400}, {490, 450},
	{20, 100}, {50, 150}, {80, 200}, {110, 250}, {140, 300},
	{300, 20}, {350, 60}, {400, 110}, {450, 160}, {490, 210},
	{100, 490}, {150, 470}, {200, 450}, {250, 430}, {300, 410},
	{350, 390}, {400, 370}, {450, 350}, {490, 330}, {40, 470},
	}
};
std::array<std::pair<short, short>, 95> Session::castle_point{
	{
		{5, 25}, {15, 35}, {25, 45}, {35, 55}, {45, 65},
	{55, 75}, {65, 85}, {75, 95}, {85, 105}, {95, 115},
	{105, 125}, {115, 135}, {125, 145}, {135, 155}, {145, 165},
	{155, 175}, {165, 185}, {175, 195}, {185, 205}, {195, 215},
	{205, 225}, {215, 235}, {225, 245}, {235, 255}, {245, 265},
	{255, 275}, {265, 285}, {275, 295}, {285, 305}, {295, 315},
	{305, 325}, {315, 335}, {325, 345}, {335, 355}, {345, 365},
	{355, 375}, {365, 385}, {375, 395}, {385, 405}, {395, 415},
	{405, 425}, {415, 435}, {425, 445}, {435, 455}, {445, 465},
	{455, 475}, {465, 485}, {475, 495}, {485, 5}, {495, 15},
	{5, 495}, {15, 485}, {25, 475}, {35, 465}, {45, 455},
	{55, 445}, {65, 435}, {75, 425}, {85, 415}, {95, 405},
	{105, 395}, {115, 385}, {125, 375}, {135, 365}, {145, 355},
	{155, 345}, {165, 335}, {175, 325}, {185, 315}, {195, 305},
	{205, 295}, {215, 285}, {225, 275}, {235, 265}, {245, 255},
	{255, 245}, {265, 235}, {275, 225}, {285, 215}, {295, 205},
	{305, 195}, {315, 185}, {325, 175}, {335, 165}, {345, 155},
	{355, 145}, {365, 135}, {375, 125}, {385, 115}, {395, 105},
	{405, 95}, {415, 85}, {425, 75}, {435, 65}, {445, 55},
}
};
std::array<std::pair<short, short>, 150> Session::tree_positions{
	{
{3, 7}, {12, 18}, {21, 30}, {33, 45}, {44, 55},
	{66, 12}, {75, 24}, {85, 35}, {95, 46}, {100, 58},
	{110, 23}, {120, 37}, {130, 49}, {140, 61}, {150, 72},
	{160, 83}, {170, 94}, {180, 105}, {190, 116}, {200, 127},
	{210, 138}, {220, 149}, {230, 160}, {240, 171}, {250, 182},
	{260, 193}, {270, 204}, {280, 215}, {290, 226}, {300, 237},
	{310, 248}, {320, 259}, {330, 270}, {340, 281}, {350, 292},
	{360, 303}, {370, 314}, {380, 325}, {390, 336}, {400, 347},
	{410, 358}, {420, 369}, {430, 380}, {440, 391}, {450, 402},
	{460, 413}, {470, 424}, {480, 435}, {490, 446}, {500, 457},
	{5, 10}, {15, 20}, {25, 30}, {35, 40}, {45, 50},
	{55, 60}, {65, 70}, {75, 80}, {85, 90}, {95, 100},
	{105, 110}, {115, 120}, {125, 130}, {135, 140}, {145, 150},
	{155, 160}, {165, 170}, {175, 180}, {185, 190}, {195, 200},
	{205, 210}, {215, 220}, {225, 230}, {235, 240}, {245, 250},
	{255, 260}, {265, 270}, {275, 280}, {285, 290}, {295, 300},
	{305, 310}, {315, 320}, {325, 330}, {335, 340}, {345, 350},
	{355, 360}, {365, 370}, {375, 380}, {385, 390}, {395, 400},
	{405, 410}, {415, 420}, {425, 430}, {435, 440}, {445, 450},
	{455, 460}, {465, 470}, {475, 480}, {485, 490}, {495, 500},
	{13, 17}, {23, 27}, {33, 37}, {43, 47}, {53, 57},
	{63, 67}, {73, 77}, {83, 87}, {93, 97}, {103, 107},
	{113, 117}, {123, 127}, {133, 137}, {143, 147}, {153, 157},
	{163, 167}, {173, 177}, {183, 187}, {193, 197}, {203, 207},
	{213, 217}, {223, 227}, {233, 237}, {243, 247}, {253, 257},
	{263, 267}, {273, 277}, {283, 287}, {293, 297}, {303, 307},
	{313, 317}, {323, 327}, {333, 337}, {343, 347}, {353, 357},
	{363, 367}, {373, 377}, {383, 387}, {393, 397}, {403, 407},
	{413, 417}, {423, 427}, {433, 437}, {443, 447}, {453, 457},
	{463, 467}, {473, 477}, {483, 487}, {493, 497}, {5, 487},
}
};

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

Monster::Monster()
{

}

void Monster::move()
{
	// move를 하는데 여기서 몬스터의 시야거리에 있는 플레이어들에게만 send를 해야함 viewlist 
	// 시야거리에 있다면? 플레이어를 추격해야함 
	unordered_set<int> _prevvl;
	int myid = getId();
	int traceclid = -1; // 추격 ID 
	int nearplayer = -1;
	for (auto& pl : _clients)
	{
		if (pl._state != STATE::Ingame)continue; //움직이기 이전 주변애들 파악 
		if (pl.can_see(pl.getId(), myid, 2))
		{
			traceclid = pl.getId();
			_prevvl.insert(traceclid);
			//int dis = calculateDistance(pl.getPosx(), pl.getPosy());
		/*	if (nearplayer > dis) {

			}*/
		}
	}
	if (traceclid == -1)return; // moveTowardsPlayer의 호출이 너무많다. 
	moveTowardsPlayer(_clients[traceclid].getPosx(), _clients[traceclid].getPosy());
	//randommove();
	unordered_set<int> _nearvl; // 움직인 후에 뷰리스트
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
		if (_prevvl.count(pl) == 0) // 움직였는데 시야거리에 들어왔다? Init 기존에 원래있었다? Move 
			_clients[pl].sendMonsterInit(myid);
		else
			_clients[pl].sendMonsterMove(_npcs[myid]);
	}

	for (auto pl : _prevvl) // 기존에 시야에 있던 애들중에서 
	{
		if (_nearvl.count(pl) == 0) //움직이고 나서의 시야거리에 애들이 없다? 
		{
			_clients[pl]._vl.lock();
			if (_clients[pl].monster_view_list.count(myid) == 0) {
				_clients[pl]._vl.unlock();
				_clients[pl].sendRemovePacket(myid, 2); // 삭제 
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
	}// 몬스터 이동 
}

void Monster::moveTowardsPlayer(const short playerx, const short playery)
{
	Astar pathfinder;
	vector<AstarNode> path = pathfinder.findpath(_map, _x, _y, playerx, playery,_closedList);
	if (!path.empty()) {
		AstarNode nextStep = path.front(); // 다음 이동할 위치
		setPosx(nextStep._x);
		setPosy(nextStep._y);
		// 이동 후 상태 업데이트
	}
}

void Monster::initClosedList()
{
	memset(_map, false, sizeof(_map));

	for (const auto& pos : Session::puddle_positions) {
		_map[pos.first][pos.second] = 1;
	}
	for (const auto& pos : Session::castle_point) {
		_map[pos.first][pos.second] = 1;
	}
	for (const auto& pos : Session::tree_positions) {
		_map[pos.first][pos.second] = 1;
	}

}

int Monster::findNearPlayer(short tox, short toy, short fromx, short fromy)
{

	return 0;
}



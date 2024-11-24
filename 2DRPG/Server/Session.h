#pragma once
#pragma once
#pragma once
#include"protocol.h"
#include"Astar.h"
enum class STATE : unsigned int { Free, Alloc, Ingame, Start };
class Session
{
public:
	short getPosx();
	short getPosy();
	int getId();
	SOCKET getSocket();
	char* getName();
	int getMoveTime();
	int getHp();

	void setPosx(short x);
	void setPosy(short y);
	void setId(int id);
	void setSocket(SOCKET socket);
	void setName(char* name);
	void setMoveTime(int time);
	void setHp(int hp);

	void doRecv();
	void doSend(void* packet);

	bool can_see(int from, int to, int type);
	void setSection(bool isnpc);
public:

	Session& operator=(const Session& other) {
		if (this == &other) // 자기 자신과 대입 방지
			return *this;

		// 멤버 변수 복사
		this->_x = other._x;
		this->_y = other._y;
		this->_id = other._id;
		this->_hp = other._hp;
		this->_maxhp = other._maxhp;
		this->_att = other._att;
		this->_last_move_time = other._last_move_time;

		this->_socket = other._socket;

		// _name 배열 복사
		std::copy(std::begin(other._name), std::end(other._name), std::begin(this->_name));

		// static const 멤버는 복사하지 않아도 됨

		this->_section = other._section; // MAP_SECTION에 대해 적절한 복사 수행
		this->_state = other._state;
		this->_prevremain = other._prevremain;
		this->_viewrange = other._viewrange;
		this->_dir = other._dir;
		this->_leftright = other._leftright;

		// unordered_set 복사
		this->player_view_list = other.player_view_list;
		this->monster_view_list = other.monster_view_list;

		// 뮤텍스는 복사할 수 없음
		// 필요한 경우 새로운 동기화 객체를 초기화해야 함
		// 여기서는 멤버 변수와 관련된 복사만 수행

		return *this;
	}

public:
	//virtual void attack() = 0;
	void sendLoginPacket();
	void sendAddPacket(const Session& client);
	void sendMoverPlayerPacket(const Session& client);
	void sendRemovePacket(int id, int type);
	void sendChatPacket(char* message,int id);

protected:
	OVERLAPPED_EX _s_over;
	short _x;
	short _y;
	int _id = -1;
	int _hp = 0;
	int _maxhp = 0;
	int _att = 0;
	int _last_move_time;

	SOCKET _socket;
	char _name[NAME_SIZE];

	const static std::array<std::pair<short, short>, 120> puddle_positions;
	const static std::array<std::pair<short, short>, 95> castle_point;
	const static std::array<std::pair<short, short>, 150> tree_positions;

public:
	STATE _state;
	mutex _s_lock;
	int _prevremain;
	int _viewrange = 5;
	int _dir = 0;
	bool _leftright = false;
	unordered_set <int> player_view_list;
	unordered_set <int> monster_view_list;
	MAP_SECTION _section; 
	mutex _sectionlock;
	mutex _vl;
};
class Monster;
class Player : public Session
{
public:
	void move(int dir);
	void attack();
	bool canatt(int from, int to);
	//void attack() override;

public:
	void sendMonsterInit(int id);
	void sendMonsterMove(Monster& monster);
	void sendMonsterRemove(int id);
	void sendAttack(int id, bool onoff);
private:
	int _attrange = 1;

};

class Monster : public Session {
public:
	Monster();
	void MonsterMove();
	void randommove();
	inline void moveTowardsPlayer(const short playerx, const short playery);

	int findNearPlayer(short tox, short toy, short fromx, short fromy);

	int calculateDistance(short playerX, short playerY) {
		return abs(playerX - _x) + abs(playerY - _y);
	}

	atomic_bool isalive = false;
	mutex _lock;



};


#include"stdafx.h"
#include "Session.h"

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

void Session::doRecv()
{
    DWORD recv_flag = 0;
    memset(&_s_over._over, 0, sizeof(_s_over._over));
    _s_over._wsaBuf.len = BUF_SIZE - _prevremain;
    _s_over._wsaBuf.buf = _s_over._sendbuf + _prevremain;
    WSARecv(_socket, &_s_over._wsaBuf, 1, 0, &recv_flag,&_s_over._over,0);
}

void Session::doSend(void* packet)
{
    OVERLAPPED_EX* sendover = new OVERLAPPED_EX{ reinterpret_cast<char*>(packet) };
    WSASend(_socket, &sendover->_wsaBuf, 1, 0, 0, &sendover->_over, 0);
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
    doSend(&p);
}

//void Player::move()
//{
//}

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
    switch (rand() % 4)
    {
    case 0:
        setPosx(getPosx() + 1);
        break;
    case 1:
        setPosx(getPosx() - 1);
        break;
    case 2:
        setPosy(getPosy() + 1);
        break;
    case 3:
        setPosy(getPosy() - 1);
        break;
    }   
}

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

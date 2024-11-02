#include "stdafx.h"
#include"Iocp.h"

array<Player, MAX_USER> _clients;
array<Monster, MAX_NPC> _npcs;
HANDLE _iocphandle;
int main()
{
	Iocp iocp;
	iocp.Run();
}
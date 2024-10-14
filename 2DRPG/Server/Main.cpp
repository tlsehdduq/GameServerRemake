#include "stdafx.h"
#include"Iocp.h"

extern array<Player, MAX_USER> _clients;
extern array<Monster, MAX_NPC> _npcs;

int main()
{
	Iocp iocp;

	iocp.Run();


}
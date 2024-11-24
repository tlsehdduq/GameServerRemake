#include "stdafx.h"
#include"Iocp.h"

array<Player, MAX_USER> _clients;
array<Monster, MAX_NPC> _npcs;

unordered_set<int> _rightupSection;
unordered_set<int> _rightdownSection;
unordered_set<int> _leftupSection;
unordered_set<int> _leftdownSection;

unordered_set<int> _rightupNpcSection;
unordered_set<int> _rightdownNpcSection;
unordered_set<int> _leftupNpcSection;
unordered_set<int> _leftdownNpcSection;

HANDLE _iocphandle;
int main()
{
	Iocp iocp;
	iocp.Run();
}
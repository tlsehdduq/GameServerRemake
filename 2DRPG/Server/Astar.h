#pragma once
class AstarNode
{
public:
	int calFcost() const;
	int calHcost(short fromx, short fromy, short tox,short toy); // ������������ ����Ÿ� 
	
	AstarNode(short x, short y, int g, int h, AstarNode* parent = nullptr)
		: _x(x), _y(y), _gcost(g), _hcost(h), parent(parent) {}

	bool operator<(const AstarNode& other) const {
		return calFcost() > other.calFcost(); // fCost�� ���� ������ ����
	}
public:
	int _gcost;
	int _hcost;
	short _x;
	short _y;

	AstarNode* parent;
};

class Astar
{
private:
	int calculateH(short fromX, short fromY, short toX, short toY);
	bool isValid(int map[1000][1000], int x, int y, bool closedList[1000][1000]); // ��ȿ�� �˻� �Լ� �߰�

public:
	vector<AstarNode>findpath(int map[1000][1000], short startx, short starty, short goalx, short goaly);
	mutex closedListMutex;
};

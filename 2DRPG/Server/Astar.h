#pragma once
class AstarNode
{
public:
	int calFcost() const;
	int calHcost(short fromx, short fromy, short tox,short toy); // 목적지까지의 예상거리 
	
	AstarNode(short x, short y, int g, int h, AstarNode* parent = nullptr)
		: _x(x), _y(y), _gcost(g), _hcost(h), parent(parent) {}

	bool operator<(const AstarNode& other) const {
		return calFcost() > other.calFcost(); // fCost가 낮은 순서로 정렬
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
	bool isValid(int map[1000][1000], int x, int y, bool closedList[1000][1000]); // 유효성 검사 함수 추가

public:
	vector<AstarNode>findpath(int map[1000][1000], short startx, short starty, short goalx, short goaly);
	mutex closedListMutex;
};

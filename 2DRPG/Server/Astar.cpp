#include "stdafx.h"
#include "Astar.h"


int AstarNode::calFcost() const
{
	return _gcost + _hcost;
}

int AstarNode::calHcost(short fromx, short fromy, short tox, short toy)
{
	return abs(fromx - tox) + abs(fromy - toy);
}

int Astar::calculateH(short fromX, short fromY, short toX, short toY)
{
	return 0;
}

bool Astar::isValid(int map[10][10], int x, int y, bool closedList[10][10])
{
    // ���� ���� ���� �ְ�, ��ֹ��� ������, ���� �湮���� ���� ��쿡�� ��ȿ
    if (x >= 0 && x < 10 && y >= 0 && y < 10 && map[x][y] == 0 && !closedList[x][y]) {
        return true;
    }
    return false;
}

vector<AstarNode> Astar::findpath(int map[10][10], short startx, short starty, short goalx, short goaly)
{
    std::priority_queue<AstarNode> openList;
    bool closedList[10][10] = { false };
    AstarNode startNode = { startx, starty, 0, calculateH(startx, starty, goalx, goaly), nullptr };
    openList.push(startNode);

    int dx[] = { -1, 1, 0, 0 };
    int dy[] = { 0, 0, -1, 1 };

    while (!openList.empty()) {
        AstarNode current = openList.top();
        openList.pop();

        // ��ǥ�� ������ ��� ��� ����
        if (current._x == goalx && current._y == goaly) {
            std::vector<AstarNode> path;
            while (current.parent) {
                path.push_back(current);
                current = *current.parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedList[current._x][current._y] = true;

        // ���� ��� Ž��
        for (int i = 0; i < 4; ++i) {
            int newX = current._x + dx[i];
            int newY = current._y + dy[i];

            if (isValid(map, newX, newY, closedList)) {
                int gCost = current._gcost + 1;
                int hCost = calculateH(newX, newY, goalx, goaly);
                AstarNode neighbor(newX, newY, gCost, hCost, new AstarNode(current));

                openList.push(neighbor);
            }
        }
    }
    return {}; // ��ΰ� ���� ��� �� ���� ��ȯ
}

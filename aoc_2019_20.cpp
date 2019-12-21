#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>

using Input = std::vector<std::string>;

struct Point
{
    int64_t x, y;
    friend bool operator==(const Point& a, const Point& b)
    {
        return a.x == b.x && a.y == b.y;
    }
    friend bool operator<(const Point& a, const Point& b)
    {
        return a.x < b.x || a.x == b.x && a.y < b.y;
    }
    friend Point operator+(const Point& a, const Point& b)
    {
        return { a.x + b.x, a.y + b.y };
    }
};

using GateAccessPoints = std::map<Point, Point>;

Input ReadInput()
{
    Input input;

    while (std::cin)
    {
        std::string value;
        if (std::getline(std::cin, value))
        {
            input.push_back(value);
        }
    }

    return input;
}

bool isLetter(char ch)
{
    return ch >= 'A' && ch <= 'Z';
}

std::array<Point, 4> deltas = {
    Point{ -1,  0},
    {  0,  -1},
    {  1,  0},
    { 0,   1}
};

GateAccessPoints ProcessGates(Input& input, Point& startPoint, Point& endPoint)
{
    GateAccessPoints gateAccessPoints;
    std::vector<std::string> gates;
    std::vector<Point> gatePoints;

    for (int i = 0; i < input.size(); i++)
    {
        for (int j = 0; j < input[0].size(); j++)
        {
            if (input[i][j] == '.')
            {
                std::string gateName;
                for (int k = 0; k < deltas.size(); k++)
                {
                    auto ii = i + deltas[k].y;
                    auto jj = j + deltas[k].x;
                    auto ch = input[ii][jj];
                    if (isLetter(ch))
                    {
                        gateName += ch;
                        ii += deltas[k].y;
                        jj += deltas[k].x;
                        gateName += input[ii][jj];
                        if (k < 2)
                            std::reverse(gateName.begin(), gateName.end());
                        Point accessibleFrom{ j, i };
                        gates.push_back(gateName);
                        gatePoints.push_back(accessibleFrom);
                    }

                }
            }
        }
    }

    for (int i = 0; i < gates.size(); i++)
    {
        if (gates[i] == "AA")
        {
            startPoint = gatePoints[i];
        }
        else if (gates[i] == "ZZ")
        {
            endPoint = gatePoints[i];
        }
        else
        {
            for (int j = 0; j < gates.size(); j++)
            {
                if (i != j && gates[i] == gates[j])
                {
                    gateAccessPoints[gatePoints[i]] = gatePoints[j];
                    gateAccessPoints[gatePoints[j]] = gatePoints[i];
                }
            }
        }
    }

    return gateAccessPoints;
}

void ShortestPath(Input& input, const Point& startPoint, const Point& endPoint, const GateAccessPoints& gateAccessPoints)
{
    std::vector<std::vector<int64_t>> dists;
    dists.resize(input.size());
    for (auto& e : dists)
    {
        e.resize(input[0].size(), 0);
    }

    std::queue<Point> toVisit;

    toVisit.push(startPoint);

    while (!toVisit.empty())
    {
        auto cell = toVisit.front();
        toVisit.pop();

        auto& cellDist = dists[cell.y][cell.x];

        if (gateAccessPoints.find(cell) != gateAccessPoints.end())
        {
            auto newCell = gateAccessPoints.at(cell);

            auto& newCellDist = dists[newCell.y][newCell.x];

            if (newCellDist == 0 || newCellDist > cellDist + 1)
            {
                newCellDist = cellDist + 1;
                toVisit.push(newCell);
            }
        }
        for (int k = 0; k < deltas.size(); k++)
        {
            auto newCell = cell + deltas[k];

            if (input[newCell.y][newCell.x] != '.')
                continue;

            auto& newCellDist = dists[newCell.y][newCell.x];

            if (newCellDist == 0 || newCellDist > cellDist + 1)
            {
                newCellDist = cellDist + 1;
                toVisit.push(newCell);
            }
        }

    }

    std::cout << dists[endPoint.y][endPoint.x];

}

void Solve1(const Input& initialInput)
{
    Input input = initialInput;
    Point startPoint, endPoint;
    auto gateAccessPoints = ProcessGates(input, startPoint, endPoint);
    ShortestPath(input, startPoint, endPoint, gateAccessPoints);
}

bool IsInnerPoint(const Point& point, int64_t mazeSizeX, int64_t mazeSizeY)
{
    return point.x > 3 && point.x < mazeSizeX - 3 && point.y > 3 && point.y < mazeSizeY - 3;
}

void ShortestPath2(Input& input, const Point& startPoint, const Point& endPoint, const GateAccessPoints& gateAccessPoints)
{
    const int64_t maxNumLevels = 32;
    std::vector<std::vector<std::array<int64_t, maxNumLevels>>> dists;
    dists.resize(input.size());
    for (auto& e : dists)
    {
        e.resize(input[0].size());
        for (auto v : e)
            v.fill(0);
    }

    struct CellInfo
    {
        Point point;
        int64_t level;
    };

    std::queue<CellInfo> toVisit;

    toVisit.push({ startPoint, 0 });

    while (!toVisit.empty())
    {
        auto cell = toVisit.front().point;
        auto level = toVisit.front().level;
        toVisit.pop();

        auto& cellDist = dists[cell.y][cell.x][level];

        if (gateAccessPoints.find(cell) != gateAccessPoints.end())
        {
            bool cellIsInnerCell = IsInnerPoint(cell, input[0].size(), input.size());

            if (cellIsInnerCell || level > 0)
            {
                auto newCell = gateAccessPoints.at(cell);

                auto newLevel = cellIsInnerCell ? level + 1 : level - 1;
                if (newLevel < maxNumLevels)
                {
                    auto& newCellDist = dists[newCell.y][newCell.x][newLevel];

                    if (newCellDist == 0 || newCellDist > cellDist + 1)
                    {
                        newCellDist = cellDist + 1;
                        toVisit.push({ newCell, newLevel });
                    }
                }
            }
        }

        for (int k = 0; k < deltas.size(); k++)
        {
            auto newCell = cell + deltas[k];
            auto newLevel = level;

            if (input[newCell.y][newCell.x] != '.')
                continue;

            auto& newCellDist = dists[newCell.y][newCell.x][newLevel];

            if (newCellDist == 0 || newCellDist > cellDist + 1)
            {
                newCellDist = cellDist + 1;
                toVisit.push({ newCell, newLevel });
            }
        }
    }

    std::cout << dists[endPoint.y][endPoint.x][0];
}

void Solve2(const Input& initialInput)
{
    Input input = initialInput;
    Point startPoint, endPoint;
    auto gateAccessPoints = ProcessGates(input, startPoint, endPoint);
    ShortestPath2(input, startPoint, endPoint, gateAccessPoints);
}

int main()
{
    Input input = ReadInput();

    Solve2(input);

    return 0;
}
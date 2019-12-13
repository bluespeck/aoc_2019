#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <set>
#include <vector>

using Input = std::vector<int64_t>;

int64_t relBase = 0;

Input ReadInput()
{
    Input input;

    while (std::cin)
    {
        int64_t value;
        if (std::cin >> value)
        {
            input.push_back(value);
        }
        char c;
        std::cin >> c;
    }

    return input;
}

int64_t GetOpcode(int64_t value)
{
    return value % 100;
}

int64_t GetMode(int64_t value, int64_t paramIndex)
{
    int64_t result = value / 100;
    for (int i = 0; i < paramIndex - 1; i++)
    {
        if (result == 0)
            return 0;
        result /= 10;
    }
    return result % 10;
}

int64_t& ProcessLValueParam(Input& input, int64_t address, int64_t index)
{
    int64_t mode = GetMode(input[address], index);
    int64_t value = input[address + index];
    switch (mode)
    {
    case 0:
        return input[value];
    case 1:
        assert(false);
    case 2:
        return input[relBase + value];
    default:
        assert(false);
    }
    return input[0];
}

int64_t ProcessRValueParam(Input& input, int64_t address, int64_t index)
{
    int64_t mode = GetMode(input[address], index);
    int64_t value = input[address + index];
    switch (mode)
    {
    case 0:
        return input[value];
    case 1:
        return value;
    case 2:
        return input[relBase + value];
    default:
        assert(false);
    }
    return 0;
}

void Add(Input& input, int64_t address)
{
    int64_t value1 = ProcessRValueParam(input, address, 1);
    int64_t value2 = ProcessRValueParam(input, address, 2);
    ProcessLValueParam(input, address, 3) = value1 + value2;
}

void Multiply(Input& input, int64_t address)
{
    int64_t value1 = ProcessRValueParam(input, address, 1);
    int64_t value2 = ProcessRValueParam(input, address, 2);
    ProcessLValueParam(input, address, 3) = value1 * value2;
}

void Read(Input& input, int64_t address, int64_t inputValue)
{
    ProcessLValueParam(input, address, 1) = inputValue;
}

int64_t Write(Input& input, int64_t address)
{
    return ProcessRValueParam(input, address, 1);
}

void JumpIfTrue(Input& input, int64_t& address)
{
    int64_t value1 = ProcessRValueParam(input, address, 1);
    int64_t value2 = ProcessRValueParam(input, address, 2);
    address = value1 != 0 ? value2 : address + 3;
}

void JumpIfFalse(Input& input, int64_t& address)
{
    int64_t value1 = ProcessRValueParam(input, address, 1);
    int64_t value2 = ProcessRValueParam(input, address, 2);
    address = value1 == 0 ? value2 : address + 3;
}

void LessThan(Input& input, int64_t address)
{
    int64_t value1 = ProcessRValueParam(input, address, 1);
    int64_t value2 = ProcessRValueParam(input, address, 2);
    ProcessLValueParam(input, address, 3) = (value1 < value2 ? 1 : 0);
}

void Equals(Input& input, int64_t address)
{
    int64_t value1 = ProcessRValueParam(input, address, 1);
    int64_t value2 = ProcessRValueParam(input, address, 2);
    ProcessLValueParam(input, address, 3) = (value1 == value2 ? 1 : 0);
}

void AdjustRelBase(Input& input, int64_t address)
{
    int64_t value1 = ProcessRValueParam(input, address, 1);
    relBase += value1;
}

bool DecodeExec(Input& input, int64_t& address, int64_t inputValue, int64_t& outputValue, bool& outputChanged)
{
    auto opCode = GetOpcode(input[address]);
    outputChanged = false;
    switch (opCode)
    {
    case 1:
        Add(input, address);
        address += 4;
        break;
    case 2:
        Multiply(input, address);
        address += 4;
        break;
    case 3:
        Read(input, address, inputValue);
        address += 2;
        break;
    case 4:
    {
        outputValue = Write(input, address);
        outputChanged = true;
        //std::cout << outputValue << "\n";
        address += 2;
        break;
    }
    case 5:
        JumpIfTrue(input, address);
        break;
    case 6:
        JumpIfFalse(input, address);
        break;
    case 7:
        LessThan(input, address);
        address += 4;
        break;
    case 8:
        Equals(input, address);
        address += 4;
        break;
    case 9:
        AdjustRelBase(input, address);
        address += 2;
        break;
    case 99:
        return false;

    default:
        assert(false);
    }
    return true;
}

struct point
{
    int x;
    int y;
};

bool operator<(const point& a, const point& b)
{
    return a.x < b.x || a.x == b.x && a.y < b.y;
}

point deltas[] = {
{0, 1},
{1, 0},
{0, -1},
{-1, 0}
};

void Solve1(const Input& initialInput)
{
    auto input = initialInput;
    relBase = 0;

    input.resize(input.size() * 50, 0);
    int64_t inputValue = 0; //initial color
    int64_t outputValue = 0;

    std::set<point> positions, visitedAtLeastOnce;
    point pos = { 0, 0 };
    int64_t dir = 0;
    int64_t address = 0;
    bool nextOutputColor = true;

    while (true)
    {
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValue, outputValue, outputChanged))
            break;

        if (outputChanged)
        {
            if (nextOutputColor)
            {
                if (outputValue == 0)
                    positions.erase(pos);
                else
                    positions.insert(pos);
                visitedAtLeastOnce.insert(pos);
            }
            else
            {
                dir = dir + (outputValue == 0 ? -1 : 1);
                if (dir == 4)
                    dir = 0;
                if (dir == -1)
                    dir = 3;
                pos.x += deltas[dir].x;
                pos.y += deltas[dir].y;
                inputValue = positions.find(pos) == positions.end() ? 0 : 1;
            }
            nextOutputColor = !nextOutputColor;
        }
    }

    std::cout << visitedAtLeastOnce.size() << "\n";
}

void Solve2(const Input& initialInput)
{
    auto input = initialInput;
    relBase = 0;

    input.resize(input.size() * 50, 0);
    int64_t inputValue = 1;
    int64_t outputValue = 0;

    std::set<point> positions, visitedAtLeastOnce;
    point pos = { 0, 0 };
    int64_t dir = 0;
    int64_t address = 0;
    bool nextOutputColor = true;

    while (true)
    {
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValue, outputValue, outputChanged))
            break;

        if (outputChanged)
        {
            if (nextOutputColor)
            {
                if (outputValue == 0)
                    positions.erase(pos);
                else
                    positions.insert(pos);
                visitedAtLeastOnce.insert(pos);
            }
            else
            {
                dir = dir + (outputValue == 0 ? -1 : 1);
                if (dir == 4)
                    dir = 0;
                if (dir == -1)
                    dir = 3;
                pos.x += deltas[dir].x;
                pos.y += deltas[dir].y;
                inputValue = positions.find(pos) == positions.end() ? 0 : 1;
            }
            nextOutputColor = !nextOutputColor;
        }
    }
    char a[10][40];
    for (auto& e : positions)
    {
        a[5 + e.y][e.x] = '*';
    }

    for (int i = 9; i >= 0; i--)
    {
        for (int j = 0; j < 40; j++)
            if (a[i][j] == '*')
                std::cout << '#';
            else
                std::cout << ' ';
        std::cout << std::endl;
    }
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}

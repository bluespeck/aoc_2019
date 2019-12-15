#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include <queue>

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

struct Position
{
    int x, y;
};

enum class Direction
{
    north = 1,
    south = 2,
    west = 3,
    east = 4
};

enum class State
{
    wall = 0,
    moved = 1,
    oxygen = 2,
    exit = 3
};

Position deltas[5] =
{
    { 0, 0},
    { 0, -1 },
    { 0, 1},
    { -1, 0},
    { 1, 0}
};

State DoStep(Input& input, int64_t& address, Direction dir)
{
    int64_t inputValue = static_cast<int64_t>(dir);
    while (true)
    {
        int64_t outputValue;
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValue, outputValue, outputChanged))
        {
            return State::exit;
        }

        if (outputChanged)
            return static_cast<State>(outputValue);
    }

    assert(false);
    return State::wall;
}

using Grid = std::array<std::array<int64_t, 50>, 50>;

Direction GetOppositeDir(Direction dir)
{
    switch (dir)
    {
    case Direction::north:
        return Direction::south;
    case Direction::south:
        return Direction::north;
    case Direction::west:
        return Direction::east;
    case Direction::east:
        return Direction::west;
    default:
        assert(false);
    }

    return Direction::north;
}

int64_t& GetGridCell(Grid& grid, Position pos)
{
    return grid[pos.x][pos.y];
}

int64_t GetGridCell(const Grid& grid, Position pos)
{
    return grid[pos.x][pos.y];
}

Position operator+(const Position& pos1, const Position& pos2)
{
    return { pos1.x + pos2.x, pos1.y + pos2.y };
}

void BackTrack1(Input& input, int64_t& address, Position& currentPos, Grid& grid, int64_t pathLength = 0)
{
    for (int64_t dir = 1; dir <= 4; dir++)
    {
        auto state = DoStep(input, address, static_cast<Direction>(dir));
        switch (state)
        {
        case State::wall:
            break;
        case State::moved:
        {
            Position prevPos = currentPos;
            currentPos = currentPos + deltas[dir];
            //std::cout << dir << " " << currentPos.x << " " << currentPos.y << std::endl;

            auto& currentGridCell = grid[currentPos.x][currentPos.y];
            if (pathLength + 1 < currentGridCell)
            {
                currentGridCell = pathLength + 1;

                if (pathLength + 1 < 500)
                {
                    BackTrack1(input, address, currentPos, grid, pathLength + 1);
                }
            }
            else
            {
                //std::cout << "Grid cell already visited." << std::endl;
            }
            //std::cout << "Going back to " << prevPos.x << " " << prevPos.y << std::endl;
            auto initialState = DoStep(input, address, GetOppositeDir(static_cast<Direction>(dir)));
            assert(initialState == State::moved);
            currentPos = prevPos;
            break;
        }
        case State::oxygen:
        {
            auto prevPos = currentPos;
            currentPos = currentPos + deltas[dir];
            //std::cout << "Final destination: " << dir << " " << currentPos.x << " " << currentPos.y << std::endl;
            auto& cell = GetGridCell(grid, currentPos);

            if (pathLength + 1 < cell)
            {
                cell = pathLength + 1;
                std::cout << "Shortest path to oxygen: " << pathLength + 1 << std::endl << std::endl;
            }
            
            auto initialState = DoStep(input, address, GetOppositeDir(static_cast<Direction>(dir)));
            assert(initialState == State::moved);
            currentPos = prevPos;
            return;
        }
        }
    }
}

void Solve1(const Input& initialInput)
{
    auto input = initialInput;
    relBase = 0;

    Grid grid;
    for (auto& e : grid)
    {
        e.fill(std::numeric_limits<int64_t>().max());
    }

    input.resize(input.size() * 50, 0);
    int64_t address = 0;
    
    Position startingPosition{ 25, 25 };
    GetGridCell(grid, startingPosition) = 0;

    BackTrack1(input, address, startingPosition, grid);

}

void BackTrack2(Input& input, int64_t& address, Position& currentPos, Grid& grid, int64_t pathLength = 0)
{
    for (int64_t dir = 1; dir <= 4; dir++)
    {
        auto testPos = currentPos + deltas[dir];
        if (testPos.x == 5 && testPos.y == 7)
            continue;
        auto state = DoStep(input, address, static_cast<Direction>(dir));
        switch (state)
        {
        case State::wall:
            break;
        case State::moved:
        {
            Position prevPos = currentPos;
            currentPos = currentPos + deltas[dir];
           // std::cout << dir << " " << currentPos.x << " " << currentPos.y << std::endl;

            auto& currentGridCell = grid[currentPos.x][currentPos.y];
            if (pathLength + 1 < currentGridCell)
            {
                currentGridCell = pathLength + 1;

                if (pathLength + 1 < 1600)
                {
                    BackTrack2(input, address, currentPos, grid, pathLength + 1);
                }
            }
            else
            {
                //std::cout << "Grid cell already visited." << std::endl;
            }
            //std::cout << "Going back to " << prevPos.x << " " << prevPos.y << std::endl;
            auto initialState = DoStep(input, address, GetOppositeDir(static_cast<Direction>(dir)));
            assert(initialState == State::moved);
            currentPos = prevPos;
            break;
        }
        case State::oxygen:
        {
            assert(false);
            return;
        }
        }
    }
}

void BackTrack3(Input& input, int64_t& address, Position& currentPos, Grid& grid, int64_t pathLength = 0)
{
    for (int64_t dir = 1; dir <= 4; dir++)
    {
        auto& testCell = GetGridCell(grid, currentPos + deltas[dir]);
        if (testCell > 10000)
            continue;

        Position prevPos = currentPos;
        currentPos = currentPos + deltas[dir];
        //std::cout << dir << " " << currentPos.x << " " << currentPos.y << std::endl;

        if (pathLength + 1 < testCell)
        {
            testCell = pathLength + 1;

            BackTrack3(input, address, currentPos, grid, pathLength + 1);
        }
        currentPos = prevPos;
    }
}

void Solve2(const Input& initialInput)
{
    auto input = initialInput;
    relBase = 0;

    Grid grid;
    for (auto& e : grid)
    {
        e.fill(std::numeric_limits<int64_t>().max());
    }

    input.resize(input.size() * 50, 0);
    int64_t address = 0;

    Position startingPosition{ 25, 25 };
    GetGridCell(grid, startingPosition) = 0;

    BackTrack2(input, address, startingPosition, grid);
   
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            if (i == 5 && j == 7)
                std::cout << "O";
            else if (i == 25 && j == 25)
                std::cout << "D";
            else
                std::cout << (grid[i][j] < 1000 ? '.' : '#');
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            if (grid[i][j] < 1000)
                grid[i][j] += 1000;
        }
    }

    startingPosition = { 5, 7 };
    GetGridCell(grid, startingPosition) = 0;
    
    BackTrack3(input, address, startingPosition, grid);

    int maxPath = 0;
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            if (grid[i][j] < 1000 && grid[i][j] > maxPath)
                maxPath = grid[i][j];
        }
    }

    std::cout << "\nIt takes " << maxPath << " minutes until the ship fills with oxygen." << std::endl;

}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}

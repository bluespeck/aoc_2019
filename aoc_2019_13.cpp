#include <array>
#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <set>
#include <vector>
#include <cstdlib>

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

void Solve1(const Input& initialInput)
{
    auto input = initialInput;
    relBase = 0;

    input.resize(input.size() * 50, 0);
    int64_t inputValue = 0;
    int64_t outputValue = 0;
    int64_t address = 0;
    int64_t outputCount = 0;
    using Tile = std::array<int64_t, 3>;
    Tile tileInput;
    std::vector<Tile> tiles;
    int64_t xMax = 0;
    int64_t yMax = 0;
    while (true)
    {
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValue, outputValue, outputChanged))
            break;
        if (outputChanged)
        {
            tileInput[outputCount] = outputValue;
            outputCount++;
            if (outputCount == 3)
            {
                tiles.push_back(tileInput);
                outputCount = 0;
            }
        }
    }

    int64_t count = 0;
    for (auto e : tiles)
    {
        if (e[2] == 2)
            count++;
    }
    std::cout << count << std::endl;

}

char GetTileChar(int64_t id)
{
    switch (id)
    {
    case 0:
        return ' ';
    case 1:
        return '#';
    case 2:
        return '*';
    case 3:
        return 'P';
    case 4:
        return 'O';
    }
}

void Solve2(const Input& initialInput)
{
    auto input = initialInput;
    relBase = 0;

    input.resize(input.size() * 50, 0);
    input[0] = 2;
    int64_t inputValue = 0;
    int64_t outputValue = 0;
    int64_t address = 0;
    int64_t outputCount = 0;
    using Tile = std::array<int64_t, 3>;
    Tile tileInput;
    std::vector<Tile> tiles;

    bool gameEnded = false;
    bool shouldStopAddingTile = false;
    int64_t ballX = 0;
    int64_t paddleX = 0;
    int64_t score = 0;
    const int sizeY = 20;
    const int sizeX = 45;
    char game[20][45];

    for (int i = 0; i < sizeY; i++)
        for (int j = 0; j < sizeX; j++)
            game[i][j] = ' ';

    while (true)
    {
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValue, outputValue, outputChanged))
            break;
        if (outputChanged)
        {
            tileInput[outputCount] = outputValue;
            outputCount++;
            if (outputCount == 3)
            {

                if (tileInput[0] == -1 && tileInput[1] == 0)
                {
                    score = tileInput[2];
                    if (!shouldStopAddingTile)
                    {
                        int64_t count = 0;
                        for (auto e : tiles)
                        {
                            game[e[1]][e[0]] = GetTileChar(e[2]);
                        }

                        shouldStopAddingTile = true;
                    }
                    else
                    {

                        int blockCount = 0;
                        for (int i = 0; i < sizeY; i++)
                        {
                            for (int j = 0; j < sizeX; j++)
                            {
                                if (game[i][j] == '*')
                                {
                                    blockCount++;
                                }
                            }
                        }
                        if (blockCount == 0)
                        {
                            gameEnded = true;
                        }

                    }
                }
                else
                {
                    if (!shouldStopAddingTile)
                    {
                        if (tileInput[2] == 4)
                            ballX = tileInput[0];
                        if (tileInput[2] == 3)
                            paddleX = tileInput[0];
                        tiles.push_back(tileInput);
                    }
                    else
                    {
                        game[tileInput[1]][tileInput[0]] = GetTileChar(tileInput[2]);
                        if (tileInput[2] == 4 && !gameEnded)
                        {
                            ballX = tileInput[0];

                            if (paddleX < ballX)
                            {
                                inputValue = 1;
                                paddleX += 1;
                            }
                            else if (paddleX > ballX)
                            {
                                inputValue = -1;
                                paddleX -= 1;
                            }
                            else
                            {
                                inputValue = 0;
                            }

                            system("cls");
                            for (int i = 0; i < sizeY; i++)
                            {
                                for (int j = 0; j < sizeX; j++)
                                {
                                    std::cout << game[i][j];
                                }
                                std::cout << std::endl;
                            }
                             std::cout << "Score " << score << "\n";
                            std::cout << std::endl;

                        }
                    }
                }
                outputCount = 0;
            }
        }
    }
    std::cout << "Score :" << score << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve2(input);
    Solve1(input);

    return 0;
}

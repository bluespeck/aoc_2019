#include <array>
#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <numeric>
#include <cstdio>
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

bool DecodeExec(Input& input, int64_t& address, std::vector<int64_t>& inputValues, int64_t& outputValue, bool& outputChanged)
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
    {
        auto value = inputValues.back();
        inputValues.pop_back();
        Read(input, address, value);
        address += 2;
        break;
    }
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

    std::vector<int64_t> inputValues;
    int64_t outputValue = 0;
    int64_t address = 0;
    int64_t counter = 0;
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            inputValues.push_back(i);
            inputValues.push_back(j);
            address = 0;
            relBase = 0;
            input = initialInput;
            input.resize(input.size() * 50, 0);

            while (true)
            {
                bool outputChanged = false;
                if (!DecodeExec(input, address, inputValues, outputValue, outputChanged))
                    break;
                if (outputChanged)
                {
                    counter += outputValue;
                }
            }
        }
        //std::cout << std::endl;
    }
    std::cout << counter << std::endl;
}

void Solve2_toFile(const Input& initialInput)
{
    for (int i = 0; i < 2000; i++)
    {
        for (int j = 0; j < 2000; j++)
        {
            auto input = initialInput;

            std::vector<int64_t> inputValues;
            int64_t outputValue = 0;
            int64_t address = 0;
            relBase = 0;

            
            inputValues.push_back(i);
            inputValues.push_back(j);

            while (true)
            {
                bool outputChanged = false;
                if (!DecodeExec(input, address, inputValues, outputValue, outputChanged))
                    break;
                if (outputChanged)
                {
                    std::cout << (outputValue == 1 ? '#' : ' ');
                    break;
                }
            }
        }
        std::cout << std::endl;
    }
}

void Solve2()
{
    const int64_t gridSide = 2000;
    std::array<std::string, gridSide> grid;
    std::ifstream is;
    is.open(R"(x.txt)");
    for (int i = 0; i < gridSide; i++)
    {
        std::string line;
        if (std::getline(is, line))
        {
            grid[i] = line;
        }
    }
    is.close();

    const int64_t squareSide = 100;
    for (int i = 600; i < gridSide; i++)
    {   
        int j = 0;
        for (j = 0; grid[i][j] == ' ' && j < 2000; j++);
        
        int64_t x = j;
        int64_t y = i - (squareSide - 1);
        if (x >= gridSide)
            continue;
        if (grid[y][j + (squareSide - 1)] == '#')
        {
            std::cout << x * 10'000 + y;
            return;
        }
    }
}


int main()
{
    Input input = ReadInput();
    input.resize(input.size() * 50, 0);

    //Solve1(input);
    //Solve2_toFile(input);
    Solve2();

    return 0;
 }
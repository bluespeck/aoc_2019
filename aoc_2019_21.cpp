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
    input = initialInput;
    input.resize(input.size() * 50, 0);

    std::array < std::string, 13> program = {
        "NOT A J",
        "NOT C T",
        "AND D T",
        "OR T J ",
        "WALK"
    };
    for (int i = program.size() - 1; i >= 0; i--)
    {
        inputValues.push_back(10);
        inputValues.insert(inputValues.end(), program[i].rbegin(), program[i].rend());
    }

    while (true)
    {
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValues, outputValue, outputChanged))
            break;
        if (outputChanged)
        {
            if (outputValue < 128)
            {
                std::cout << static_cast<char>(outputValue);
            }
            else
            {
                std::cout << "\n Hull damage " << outputValue << std::endl;
            }
        }
    }
    
}

void Solve2(const Input& initialInput)
{
    auto input = initialInput;

    relBase = 0;
    std::vector<int64_t> inputValues;
    int64_t outputValue = 0;
    int64_t address = 0;
    input = initialInput;
    input.resize(input.size() * 50, 0);

    std::array < std::string, 15> program = {
        // !B & !E & D <=> (!A | !B )!D // hole picked up now by sensor E will be picked up by sensor A after the jump
        // we have !A & (!C & D) combined with the above =>
        // ((!A | !B | !C) & D)
        "NOT A J", // J = !A
        "NOT B T", // T = !B
        "OR T J ", // J = !A | !B
        "NOT C T", // T = !C
        "OR T J ", // J = !A | !B | !C
        "AND D J", // J = D & (!A | !B | !C)
        // now we need to not jump if either H or E are hull tiles
        "AND C T", // make T false (!C & C)
        "OR H T ", // T = H
        "OR E T ", // T = H | E
        "AND T J", // J = D & (!A | !B | !C) & (H | E)
        "RUN"
    };
    for (int i = program.size() - 1; i >= 0; i--)
    {
        inputValues.push_back(10);
        inputValues.insert(inputValues.end(), program[i].rbegin(), program[i].rend());
    }

    while (true)
    {
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValues, outputValue, outputChanged))
            break;
        if (outputChanged)
        {
            if (outputValue < 128)
            {
                std::cout << static_cast<char>(outputValue);
            }
            else
            {
                std::cout << "\n Hull damage " << outputValue << std::endl;
            }
        }
    }

}

int main()
{
    Input input = ReadInput();
    Solve1(input);
    Solve2(input);

    return 0;
 }
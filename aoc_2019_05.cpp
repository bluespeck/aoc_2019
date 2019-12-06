#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <vector>

using Input = std::vector<int64_t>;

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

void Add(Input& input, int64_t p1, int64_t p2, int64_t p3, int64_t mode1, int64_t mode2)
{
    int64_t value1 = mode1 == 0 ? input[p1] : p1;
    int64_t value2 = mode2 == 0 ? input[p2] : p2;
    input[p3] = value1 + value2;
}

void Multiply(Input& input, int64_t p1, int64_t p2, int64_t p3, int64_t mode1, int64_t mode2)
{
    int64_t value1 = mode1 == 0 ? input[p1] : p1;
    int64_t value2 = mode2 == 0 ? input[p2] : p2;
    input[p3] = value1 * value2;
}

void Read(Input& input, int64_t p1, int64_t inputValue)
{
    input[p1] = inputValue;
}

int64_t Write(Input& input, int64_t p1, int64_t mode1)
{
    return mode1 == 0 ? input[p1] : p1;
}

void JumpIfTrue(Input& input, int64_t p1, int64_t p2, int64_t mode1, int64_t mode2, int64_t & address)
{
    int64_t value1 = (mode1 == 0 ? input[p1] : p1);
    int64_t value2 = (mode2 == 0 ? input[p2] : p2);
    if (value1 != 0)
    {
        address = value2;
    }
    else
    {
        address += 3;
    }
}

void JumpIfFalse(Input& input, int64_t p1, int64_t p2, int64_t mode1, int64_t mode2, int64_t& address)
{
    int64_t value1 = mode1 == 0 ? input[p1] : p1;
    int64_t value2 = mode2 == 0 ? input[p2] : p2;
    if (value1 == 0)
    {
        address = value2;
    }
    else address += 3;
}

void LessThan(Input& input, int64_t p1, int64_t p2, int64_t p3, int64_t mode1, int64_t mode2)
{
    int64_t value1 = mode1 == 0 ? input[p1] : p1;
    int64_t value2 = mode2 == 0 ? input[p2] : p2;
    if (value1 < value2)
    {
        input[p3] = 1;
    }
    else
    {
        input[p3] = 0;
    }
}

void Equals(Input& input, int64_t p1, int64_t p2, int64_t p3, int64_t mode1, int64_t mode2)
{
    int64_t value1 = mode1 == 0 ? input[p1] : p1;
    int64_t value2 = mode2 == 0 ? input[p2] : p2;
    if (value1 == value2)
    {
        input[p3] = 1;
    }
    else
    {
        input[p3] = 0;
    }
}

bool DecodeExec(Input& input, int64_t & address, int64_t inputValue)
{
    auto firstValue = input[address];
    auto opCode = GetOpcode(input[address]);
    auto mode1 = GetMode(firstValue, 1);
    auto mode2 = GetMode(firstValue, 2);

    if (opCode == 99)
        return false;
    switch (opCode)
    {
    case 1:
        Add(input, input[address + 1], input[address + 2], input[address + 3], mode1, mode2);
        address += 4;
        break;
    case 2:
        Multiply(input, input[address + 1], input[address + 2], input[address + 3], mode1, mode2);
        address += 4;
        break;
    case 3:
        Read(input, input[address + 1], inputValue);
        address += 2;
        break;
    case 4:
    {
        auto outputValue = Write(input, input[address + 1], mode1);
        std::cout << outputValue << "\n";
        address += 2;
        break;
    }
    case 5:
        JumpIfTrue(input, input[address + 1], input[address + 2], mode1, mode2, address);
        break;
    case 6:
        JumpIfFalse(input, input[address + 1], input[address + 2], mode1, mode2, address);
        break;
    case 7:
        LessThan(input, input[address + 1], input[address + 2], input[address + 3], mode1, mode2);
        address += 4;
        break;
    case 8:
        Equals(input, input[address + 1], input[address + 2], input[address + 3], mode1, mode2);
        address += 4;
        break;
    default:
        assert(false);
    }
    return true;
}

void Solve1(const Input& initialInput)
{
    auto input = initialInput;
    int64_t inputValue = 1;
    int64_t outputValue = -2;
    
    for (int64_t i = 0; DecodeExec(input, i, inputValue););
}

void Solve2(const Input& initialInput)
{
    auto input = initialInput;
    int64_t inputValue = 5;
    int64_t outputValue = -2;
    for (int64_t i = 0; DecodeExec(input, i, inputValue););
}



int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
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
        char c = inputValues.back();
        inputValues.pop_back();
        Read(input, address, c);
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

using Grid = std::array<std::string, 100 >;
Grid grid;
void Solve2(const Input& initialInput)
{
    auto input = initialInput;
    input.resize(input.size() * 50, 0);

    relBase = 0;
    
    std::vector<int64_t> inputValues;
    int64_t outputValue = 0;
    int64_t address = 0;


    std::string input1 = "A,A,B,C,C,A,C,B,C,B\n";
    std::string inputA = "L,4,L,4,L,6,R,10,L,6\n";
    std::string inputB = "L,12,L,6,R,10,L,6\n";
    std::string inputC = "R,8,R,10,L,6\n";

    inputValues.insert(inputValues.end(), input1.begin(), input1.end());
    inputValues.insert(inputValues.end(), inputA.begin(), inputA.end());
    inputValues.insert(inputValues.end(), inputB.begin(), inputB.end());
    inputValues.insert(inputValues.end(), inputC.begin(), inputC.end());
    inputValues.push_back('n');
    inputValues.push_back('\n');
    
    std::reverse(inputValues.begin(), inputValues.end());

    while (true)
    {
        bool outputChanged = false;
        if (!DecodeExec(input, address, inputValues, outputValue, outputChanged))
            break;
        if (outputChanged)
        {
            if(outputValue > 256)
            std::cout << outputValue << std::endl;
        }
    }
}

int main()
{
    Input input = ReadInput();
	// did first one by hand (summing all intersection coordinate products from the printed grid)
	// for the second one also, I did the sequence by hand
    Solve2(input);
    
     return 0;
}

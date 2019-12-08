#include <array>
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

void JumpIfTrue(Input& input, int64_t p1, int64_t p2, int64_t mode1, int64_t mode2, int64_t& address)
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

enum class ProgramState
{
    running,
    ended,
    waitingForInput
};

ProgramState DecodeExec(Input& input, int64_t& address, std::vector<int64_t> &inputBuffer, int64_t &outputValue)
{
    auto firstValue = input[address];
    auto opCode = GetOpcode(input[address]);
    auto mode1 = GetMode(firstValue, 1);
    auto mode2 = GetMode(firstValue, 2);

    if (opCode == 99)
        return ProgramState::ended;
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
        if (inputBuffer.empty())
            return ProgramState::waitingForInput;
        Read(input, input[address + 1], inputBuffer.back());
        inputBuffer.pop_back();
        address += 2;
        break;
    case 4:
    {
        outputValue = Write(input, input[address + 1], mode1);
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
        return ProgramState::ended;
    }
    return ProgramState::running;
}

void Solve1(const Input& initialInput)
{
    const int64_t numAmplifiers = 5;
    std::array<Input, numAmplifiers> amplifiers;

    std::array<int64_t, numAmplifiers> phases;
    int64_t maxThrusterSignal = 0;
    int64_t maxThrust = 0;

    std::iota(phases.begin(), phases.end(), 0);

    do
    {
        std::vector<int64_t> inputBuffer;
        int64_t outputValue = 0;

        for (int j = 0; j < numAmplifiers; j++)
        {
            inputBuffer.clear();
            inputBuffer.push_back(outputValue);
            inputBuffer.push_back(phases[j]);
            amplifiers[j] = initialInput;
            for (int64_t address = 0; DecodeExec(amplifiers[j], address, inputBuffer, outputValue) == ProgramState::running;);

        }
        if (outputValue >= maxThrust)
        {
            maxThrust = outputValue;
        }

        
    } while (std::next_permutation(phases.begin(), phases.end()));

    std::cout << maxThrust << std::endl;
}

void Solve2(const Input& initialInput)
{
    const int64_t numAmplifiers = 5;
    std::array<Input, numAmplifiers> amplifiers;
    std::array<int64_t, numAmplifiers> currentAddress;

    std::array<int64_t, numAmplifiers> phases;
    int64_t maxThrusterSignal = 0;
    int64_t maxThrust = 0;
    
    std::iota(phases.begin(), phases.end(), 5);
    
    do
    {
        std::vector<int64_t> inputBuffer;
        int64_t outputValue = 0;
        bool shouldContinueLooping = false;
        do
        {
            for (int j = 0; j < numAmplifiers; j++)
            {
                inputBuffer.push_back(outputValue);
                if (!shouldContinueLooping)
                {
                    inputBuffer.push_back(phases[j]);
                    amplifiers[j] = initialInput;
                    currentAddress[j] = 0;
                }
                
                for (;;)
                {
                    auto state = DecodeExec(amplifiers[j], currentAddress[j], inputBuffer, outputValue);
                    if (j == numAmplifiers - 1)
                    {
                        if (state == ProgramState::ended)
                        {
                            if (outputValue >= maxThrust)
                            {
                                maxThrust = outputValue;
                            }
                            shouldContinueLooping = false;
                        }
                        else if (state == ProgramState::waitingForInput)
                        {
                            shouldContinueLooping = true;
                        }
                    }
                    if(state != ProgramState::running)
                    {
                        break;
                    }
                }
            }
        } while (shouldContinueLooping);
    } while (std::next_permutation(phases.begin(), phases.end()));

    std::cout << maxThrust << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
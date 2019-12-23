#include <array>
#include <assert.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <numeric>
#include <cstdio>
#include <queue>
#include <string>
#include <set>
#include <vector>
#include <cstdlib>

using Input = std::vector<int64_t>;

struct ProgramState
{
    Input program;
    int64_t relBase;
    int64_t address;
    std::queue<int64_t> inputQueue;
    std::vector<int64_t> output;
    int64_t idleCount = 0;
};

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

int64_t& ProcessLValueParam(Input& input, int64_t address, int64_t index, int64_t& relBase)
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

int64_t ProcessRValueParam(Input& input, int64_t address, int64_t index, int64_t relBase)
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

void Add(Input& input, int64_t address, int64_t relBase)
{
    int64_t value1 = ProcessRValueParam(input, address, 1, relBase);
    int64_t value2 = ProcessRValueParam(input, address, 2, relBase);
    ProcessLValueParam(input, address, 3, relBase) = value1 + value2;
}

void Multiply(Input& input, int64_t address, int64_t relBase)
{
    int64_t value1 = ProcessRValueParam(input, address, 1, relBase);
    int64_t value2 = ProcessRValueParam(input, address, 2, relBase);
    ProcessLValueParam(input, address, 3, relBase) = value1 * value2;
}

void Read(Input& input, int64_t address, int64_t inputValue, int64_t relBase)
{
    ProcessLValueParam(input, address, 1, relBase) = inputValue;
}

int64_t Write(Input& input, int64_t address, int64_t relBase)
{
    return ProcessRValueParam(input, address, 1, relBase);
}

void JumpIfTrue(Input& input, int64_t& address, int64_t relBase)
{
    int64_t value1 = ProcessRValueParam(input, address, 1, relBase);
    int64_t value2 = ProcessRValueParam(input, address, 2, relBase);
    address = value1 != 0 ? value2 : address + 3;
}

void JumpIfFalse(Input& input, int64_t& address, int64_t relBase)
{
    int64_t value1 = ProcessRValueParam(input, address, 1, relBase);
    int64_t value2 = ProcessRValueParam(input, address, 2, relBase);
    address = value1 == 0 ? value2 : address + 3;
}

void LessThan(Input& input, int64_t address, int64_t relBase)
{
    int64_t value1 = ProcessRValueParam(input, address, 1, relBase);
    int64_t value2 = ProcessRValueParam(input, address, 2, relBase);
    ProcessLValueParam(input, address, 3, relBase) = (value1 < value2 ? 1 : 0);
}

void Equals(Input& input, int64_t address, int64_t relBase)
{
    int64_t value1 = ProcessRValueParam(input, address, 1, relBase);
    int64_t value2 = ProcessRValueParam(input, address, 2, relBase);
    ProcessLValueParam(input, address, 3, relBase) = (value1 == value2 ? 1 : 0);
}

void AdjustRelBase(Input& input, int64_t address, int64_t& relBase)
{
    int64_t value1 = ProcessRValueParam(input, address, 1, relBase);
    relBase += value1;
}

bool DecodeExec(ProgramState& programState, int64_t& outputValue, bool& outputChanged)
{
    auto& input = programState.program;
    auto& address = programState.address;
    auto& relBase = programState.relBase;
    auto& inputValues = programState.inputQueue;

    auto opCode = GetOpcode(input[address]);
    outputChanged = false;
    switch (opCode)
    {
    case 1:
        Add(input, address, relBase);
        address += 4;
        break;
    case 2:
        Multiply(input, address, relBase);
        address += 4;
        break;
    case 3:
    {
        if(inputValues.empty())
        {
            Read(input, address, -1, relBase);
            //std::cout << "Reading -1 \n";
            programState.idleCount ++;
        }
        else
        {
            auto value = inputValues.front();
            inputValues.pop();
            Read(input, address, value, relBase);
            programState.idleCount = 0;
            //std::cout << "Reading " << value << "\n";
        }
        address += 2;
        break;
    }
    case 4:
    {
        outputValue = Write(input, address, relBase);
        outputChanged = true;
        programState.idleCount = 0;
        //std::cout << "Writing " << outputValue << "\n";
        address += 2;
        break;
    }
    case 5:
        JumpIfTrue(input, address, relBase);
        break;
    case 6:
        JumpIfFalse(input, address, relBase);
        break;
    case 7:
        LessThan(input, address, relBase);
        address += 4;
        break;
    case 8:
        Equals(input, address, relBase);
        address += 4;
        break;
    case 9:
        AdjustRelBase(input, address, relBase);
        address += 2;
        break;
    case 99:
        return false;

    default:
        assert(false);
    }
    return true;
}


void Solve12(const Input& initialInput)
{
    auto input = initialInput;
    input.resize(input.size() * 50);

    std::array<ProgramState, 50> programStates;
    for (int i = 0; i < 50; i++)
    {
        auto& state = programStates[i];
        state.program = input;
        state.address = 0;
        state.relBase = 0;
        state.inputQueue.push(i);
    }

    std::array<std::vector<int64_t>, 50> tempQueues;
    
    struct Nat
    {
        std::set<int64_t> ySent;
        int64_t x;
        int64_t y;
    } nat;

    bool firstTimeSentTo255 = true;
    while (true)
    {
        for (int i = 0; i < 50; i++)
        {
            int64_t outputValue;
            bool outputChanged = false;
            bool readingEmptyQueue = false;
            if (!DecodeExec(programStates[i], outputValue, outputChanged))
                break;
            if (outputChanged)
            {
                auto& output = programStates[i].output;
                output.push_back(outputValue);
                if (output.size() == 3)
                {
                    if (output[0] == 255)
                    {
                        nat.x = output[1];
                        nat.y = output[2];
                        //std::cout << i << " sending ( " << output[1] << ", " << output[2] << " ) to NAT." << std::endl;
                        if (firstTimeSentTo255)
                        {
                            std::cout << "First Y value sent to 255 is " << output[2] << ".\n";
                            firstTimeSentTo255 = false;
                        }
                    }
                    else
                    {
                        //std::cout << i << " sending (" << output[1] << ", " << output[1] << " ) to " << output[2] << "."<< std::endl;
                        auto& destInput = tempQueues[output[0]];
                        destInput.push_back(output[1]);
                        destInput.push_back(output[2]);
                    }
                    output.clear();
                }
            }
        }
        for (int i = 0; i < 50; i++)
        {
            for (auto e : tempQueues[i])
            {
                programStates[i].inputQueue.push(e);
            }
            tempQueues[i].clear();
        }

        {
            int idleCount = std::count_if(programStates.begin(), programStates.end(),
                [](const ProgramState& ps) 
                {
                    return ps.idleCount > 0;
                });
            if (idleCount == programStates.size())
            {
                //std::cout << "NAT sending ( " << nat.x << ", " << nat.y << " ) to " << 0 << std::endl;
                auto& destInput = programStates[0].inputQueue;
                destInput.push(nat.x);
                destInput.push(nat.y);
                programStates[0].idleCount = 0;

                if (nat.ySent.find(nat.y) != nat.ySent.end())
                {
                    std::cout << "First Y value sent twice by NAT is " << nat.y << ".\n";
                    return;
                }
                else
                {
                    nat.ySent.insert(nat.y);
                }
            }
        }
    }
}

int main()
{
    Input input = ReadInput();
    Solve12(input);

    return 0;
}
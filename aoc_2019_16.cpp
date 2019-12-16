#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using Input = std::vector<int>;

Input ReadInput()
{
    Input input;

    while (std::cin)
    {
        char value;
        if (std::cin >> value)
        {
            input.push_back(value - '0');
        }
    }

    return input;
}

void Solve1(const Input& initialInput)
{
    Input input = initialInput;
    Input output = initialInput;
    
    std::array<int64_t, 4> pattern = { 0, 1, 0, -1 };

    const int64_t numPhases = 100;

    for (int64_t phase = 0; phase < numPhases; phase++)
    {
        input = output;
        for (int i = 0; i < output.size(); i++)
        {
            int64_t indexInPattern = 0;
            int64_t indexRepeat = 0;

            indexRepeat++;
            if (indexRepeat == i + 1)
            {
                indexRepeat = 0;
                indexInPattern = (indexInPattern + 1) % pattern.size();
            }
            
            output[i] = 0;
            for (int j = 0; j < output.size(); j++)
            {
                auto value = input[j] * pattern[indexInPattern];
                output[i] += value;

                indexRepeat++;
                if (indexRepeat == i + 1)
                {
                    indexRepeat = 0;
                    indexInPattern = (indexInPattern + 1) % pattern.size();
                }
            }
            if (output[i] < 0)
                output[i] = -output[i];
            output[i] = output[i] % 10;
        }
    }
    for (int64_t i = 0; i < 8; i++)
    {
        std::cout << output[i];
    }
    std::cout << std::endl;
}

void Solve2(const Input& initialInput)
{
    int64_t offset = 0;
    for (int i = 0; i < 7; i++)
        offset = offset * 10 + initialInput[i];

    Input input;
    for (int i = 0; i < 10000; i++)
    {
        input.insert(input.end(), initialInput.begin(), initialInput.end());
    }
    auto output = input;
    
    const int64_t numPhases = 100;

    for (int64_t phase = 0; phase < numPhases; phase++)
    {
        input = output;
        auto sum = std::accumulate(input.begin() + offset, input.end(), 0);
        for (int64_t i = offset; i < output.size(); i++)
        {
            output[i] = sum % 10;
            sum -= input[i];
        }
    }

    for (int64_t i = offset; i < offset + 8; i++)
    {
        std::cout << output[i];
    }
    std::cout << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
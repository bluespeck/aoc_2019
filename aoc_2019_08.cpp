#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <vector>

using Input = std::vector<uint8_t>;

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

size_t CountLayerPixels(const Input& input, size_t layerSize, size_t layerIndex, uint8_t color)
{
    return std::count_if(std::begin(input) + layerIndex * layerSize, std::begin(input) + (layerIndex + 1) * layerSize, 
        [color](uint8_t digit) { return digit == color; }
    );
}

void Solve1(const Input& input)
{
    size_t width = 25;
    size_t height = 6;
    size_t layerSize = width * height;
    size_t layerIndex = 0;
    size_t maxZeros = 150;
    for (size_t i = 0; i < input.size() / layerSize; i++)
    {
        auto zeros = CountLayerPixels(input, layerSize, i, 0);
        if (zeros < maxZeros)
        {
            layerIndex = i;
            maxZeros = zeros;
        }
    }
    
    auto ones = CountLayerPixels(input, layerSize, layerIndex, 1);
    auto twos = CountLayerPixels(input, layerSize, layerIndex, 2);
    std::cout << ones * twos << "\n";
}

void Solve2(const Input& input)
{
    size_t width = 25;
    size_t height = 6;
    size_t layerSize = width * height;
    size_t layerIndex = 0;
    size_t maxZeros = 150;
    for (int j = 0; j < 150; j++)
    {
        for (size_t i = 0; i < input.size() / layerSize; i++)
        {
            if (input[i * layerSize + j] != 2)
            {
                auto chr = input[i * layerSize + j];
                std::cout << (chr == 1 ? '*' : ' ');
                break;
            }
        }
        if ((j + 1) % width == 0)
            std::cout << "\n";
    }
}


int main()
{
    Input input = ReadInput();
    Solve1(input);
    Solve2(input);

    return 0;
}
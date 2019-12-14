#include <algorithm>
#include <array>
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct MatQuantity
{
    int64_t q;
    std::string name;
};

struct MatRecipe
{
    int64_t q;
    std::vector<MatQuantity> recipe;
};

using Input = std::unordered_map<std::string, MatRecipe>;
using SurplusMap = std::unordered_map<std::string, int64_t>;
SurplusMap surplusMap;

Input ReadInput()
{
    Input input;
    std::string line;

    while (std::getline(std::cin, line))
    {
        if (line.size())
        {
            std::replace(line.begin(), line.end(), ',', ' ');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '>', ' ');
            std::istringstream ss(line);

            MatQuantity value;
            std::vector<MatQuantity> recipe;
            while (ss >> value.q >> value.name)
            {
                if (value.q > 0)
                {
                    recipe.push_back(value);
                }
            }

            MatQuantity result = recipe.back();
            recipe.pop_back();

            assert(input.find(result.name) == input.end());
            auto& elem = input[result.name];
            elem.q = result.q;
            elem.recipe = recipe;
            surplusMap[result.name] = 0;
        }
        
    }

    return input;
}

void ClearSurplusMap()
{
    for (auto& e : surplusMap)
    {
        e.second = 0;
    }
}

int64_t GetOreQuantity(const Input& input, const MatQuantity& desiredMatQ)
{
    if (desiredMatQ.name == "FUEL")
    {
        // needed? probably not, but just to be safe
        ClearSurplusMap();
    }

    auto& resultNode = input.at(desiredMatQ.name);
    int64_t producedPerReaction = resultNode.q;
    int64_t desiredTotal = desiredMatQ.q;
    int64_t surplusAvailable = surplusMap.at(desiredMatQ.name);
    int64_t toBeProduced = desiredTotal - surplusAvailable;
    
    int64_t multiplier = toBeProduced / producedPerReaction;

    if (toBeProduced > multiplier * producedPerReaction)
    {
        multiplier++;
    }
    int64_t newSurplus = multiplier * producedPerReaction - toBeProduced;
    surplusMap[desiredMatQ.name] = newSurplus;

    int64_t acc = 0;
    for (auto& comp : resultNode.recipe)
    {
        if (comp.name == "ORE")
        {
            acc += comp.q * multiplier;
        }
        else
        {
            acc += GetOreQuantity(input, { comp.q * multiplier, comp.name });
        }
    }

    return acc;
}

void Solve1(const Input& input)
{   
    std::cout << "Ore per fuel: " << GetOreQuantity(input, { 1, "FUEL" }) << std::endl;
}

void Solve2(const Input& input)
{
    const int64_t trillion = 1'000'000'000'000;
    int64_t minFuel = trillion / GetOreQuantity(input, { 1, "FUEL" });
    int64_t step = 1'000'000;
    while (step > 0)
    {
        for (int64_t i = minFuel; ; i += step)
        {
            int64_t oreQuantity = GetOreQuantity(input, { i, "FUEL" });
            if (oreQuantity >= trillion)
            {
                minFuel = i - step;
                break;
            }
        }

        step /= 10;
    }

    std::cout << "Fuel per 1 trillion ore: " << minFuel << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);
    

    return 0;
}
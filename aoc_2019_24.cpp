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

using Input = std::vector<std::string>;

Input ReadInput()
{
    Input input;

    while (std::cin)
    {
        std::string value;
        if (std::getline(std::cin , value))
        {
            input.push_back(value);
        }
    }

    return input;
}

int CountNeighbors(const Input& input, int i, int j)
{
    int deltai[] = { 1, 0, -1, 0 };
    int deltaj[] = { 0, 1, 0, -1 };
    int count = 0;
    for (int k = 0; k < 4; k++)
    {
        auto ii = i + deltai[k];
        auto jj = j + deltaj[k];
        if (ii < 5 && jj < 5 && ii >= 0 && jj >= 0 && input[ii][jj] == '#')
        {

            count++;
        }
    }
    return count;
}

void Solve1(const Input& originalInput)
{
    Input input = originalInput;
    std::array<std::array<int, 5>, 5> counts;
    std::set<int64_t> seen;

    while (true)
    {

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                auto count = CountNeighbors(input, i, j);
                counts[i][j] = count;
            }
        }
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (input[i][j] == '#' && counts[i][j] != 1)
                    input[i][j] = '.';
                else if (input[i][j] == '.' && (counts[i][j] == 1 || counts[i][j] == 2))
                    input[i][j] = '#';
            //    std::cout << input[i][j];
            }
          //  std::cout << std::endl;
        }
        //std::cout << std::endl;
        int64_t bioDiv = 0;
        int64_t pointsPerTile = 1;
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (input[i][j] == '#')
                {
                    bioDiv += pointsPerTile;
                }
                pointsPerTile <<= 1;
            }
        }
        if (std::find(seen.begin(), seen.end(), bioDiv) != seen.end())
        {
            std::cout << "Biodiversity for first repeating layout: " << bioDiv << std::endl;
            break;
        }
        seen.insert(bioDiv);
    }
}

using InfiniteInput = std::unordered_map<int64_t, Input>;

int CountEdgeBugs(const Input& input, int64_t k)
{
    int count = 0;
    for (int kk = 0; kk < 5; kk++)
    {
        switch (k)
        {
        case 0:
            if (input[0][kk] == '#')
                count++;
            break;
        case 1:
            if (input[kk][0] == '#')
                count++;
            break;
        case 2:
            if (input[4][kk] == '#')
                count++;
            break;
        case 3:
            if (input[kk][4] == '#')
                count++;
            break;
        }
    }
    return count;
}

int CountNeighbors2(const InfiniteInput& infiniteInput, const Input& emptySpace, int64_t level, int i, int j)
{
    int deltai[] = { 1, 0, -1, 0 };
    int deltaj[] = { 0, 1, 0, -1 };
    int count = 0;
    for (int k = 0; k < 4; k++)
    {
        auto ii = i + deltai[k];
        auto jj = j + deltaj[k];
        if (ii == 2 && jj == 2)
        {
            const Input& inwardInput = (infiniteInput.find(level + 1) == infiniteInput.end() ? emptySpace : infiniteInput.at(level + 1));
            count += CountEdgeBugs(inwardInput, k);
        }
        else if (ii < 0 || ii >= 5 || jj < 0 || jj >= 5)
        {
            const Input& outwardInput = (infiniteInput.find(level - 1) == infiniteInput.end() ? emptySpace : infiniteInput.at(level - 1));
            if (outwardInput[2 + deltai[k]][2 + deltaj[k]] == '#')
            {
                count++;
            }
        }
        else
        if(infiniteInput.at(level)[ii][jj] == '#')
        {   
            count++;
        }
    }
    return count;
}

int64_t CountBugs(const Input& input)
{
    int64_t totalCount = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (i == 2 && j == 2)
                continue;
            if (input[i][j] == '#')
                totalCount++;
        }
    }
    return totalCount;
}

void Solve2(const Input& originalInput)
{
    InfiniteInput infiniteInput;

    std::unordered_map<int64_t, std::array<std::array<int, 5>, 5>> counts;

    Input emptySpace(5, ".....");
    

    int64_t minLevel = 0;
    int64_t maxLevel = 0;
    infiniteInput[0] = originalInput;

    for (int iter = 0; iter < 200; iter++)
    {
        //std::cout << "Iteration " << iter << std::endl;
        if (CountBugs(infiniteInput[minLevel]) > 0)
        {
            minLevel--;
            infiniteInput[minLevel] = emptySpace;
        }
        if (CountBugs(infiniteInput[maxLevel]) > 0)
        {
            maxLevel++;
            infiniteInput[maxLevel] = emptySpace;
        }
        for (int64_t level = minLevel; level <= maxLevel; level++)
        {
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    auto count = CountNeighbors2(infiniteInput, emptySpace, level, i, j);
                    counts[level][i][j] = count;
                }
            }
        }

        for (int64_t level = minLevel; level <= maxLevel; level++)
        {
            //std::cout << "Level " << level << std::endl;
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    if (infiniteInput[level][i][j] == '#' && counts[level][i][j] != 1)
                    {
                        infiniteInput[level][i][j] = '.';
                    }
                    else if (infiniteInput[level][i][j] == '.' && (counts[level][i][j] == 1 || counts[level][i][j] == 2))
                    {
                        infiniteInput[level][i][j] = '#';
                    }
                    //std::cout << infiniteInput[level][i][j];
                }
                //std::cout << std::endl;
            }
            //std::cout << std::endl;
        }

    }

    int64_t totalCount = 0;
    for (int64_t level = minLevel; level < maxLevel; level++)
    {
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (i == 2 && j == 2)
                    continue;
                if (infiniteInput[level][i][j] == '#')
                    totalCount++;
            }
        }
    }

    std::cout << "After 200 minutes there are a total of " << totalCount << " bugs " << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
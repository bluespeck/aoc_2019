#include <array>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <vector>

using coord = std::array<int64_t, 3>;

using Input = std::array<coord, 4>;

Input ReadInput()
{
    Input input;
    
    input[0] = { 1, 2, -9 };
    input[1] = { -1, -9, -4 };
    input[2] = { 17, 6, 8 };
    input[3] = { 12, 4, 2 };

    //input[0] = { -1, 0, 2 };
    //input[1] = { 2, -10, -7 };
    //input[2] = { 4, -8, 8 };
    //input[3] = { 3, 5, -1 };
    //

    return input;
}

int64_t Energy(coord& c)
{
    return std::abs(c[0]) + std::abs(c[1]) + std::abs(c[2]);
}

void Solve1(const Input& input)
{
    auto moons = input;
    std::vector<coord> velocities;
    velocities.resize(moons.size(), { 0, 0, 0 });
    for (int64_t step = 0; step < 1000; step++)
    {
        // std::cout << "after " << step << " steps " << std::endl;
        // for (int i = 0; i < moons.size(); i++)
        // {
        //     std::cout << "p: ";
        //     for (int k = 0; k < 3; k++)
        //     {
        //         std::cout << moons[i][k] << " ";
        //     }
        //     std::cout << " v: ";
        //     for (int k = 0; k < 3; k++)
        //     {
        //         std::cout << velocities[i][k] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        for (int i = 0; i < moons.size(); i++)
        {
            for (int j = i+ 1; j < moons.size(); j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    if (moons[i][k] < moons[j][k])
                    {
                        velocities[i][k] ++;
                        velocities[j][k] --;
                    }
                    else if (moons[i][k] > moons[j][k])
                    {
                        velocities[i][k] --;
                        velocities[j][k] ++;
                    }
                }
            }
        }

        for (int i = 0; i < moons.size(); i++)
        {   
            for (int k = 0; k < 3; k++)
            {
                moons[i][k] += velocities[i][k];
            }
        }
        
    }
    int64_t sysEnergy = 0;
    for (int i = 0; i < moons.size(); i++)
    {
        sysEnergy += Energy(moons[i]) * Energy(velocities[i]);
    }

    std::cout << sysEnergy << std::endl;

}

void Solve2(const Input& input)
{
    auto moons = input;
    std::vector<coord> velocities;
    velocities.resize(moons.size(), { 0, 0, 0 });

    using PosVelocity = std::pair<int64_t, int64_t>;
    std::array<std::vector<PosVelocity>, 3> initialConfigs;

    
    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < 4; i++)
        {
            initialConfigs[k].emplace_back(moons[i][k], velocities[i][k]);
        }
    }

    std::array<int64_t, 3> periods = { 0, 0, 0 };

    
    for (int64_t step = 0; ;)
    {
        for (int i = 0; i < moons.size(); i++)
        {
            for (int j = i + 1; j < moons.size(); j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    if (moons[i][k] < moons[j][k])
                    {
                        velocities[i][k] ++;
                        velocities[j][k] --;
                    }
                    else if (moons[i][k] > moons[j][k])
                    {
                        velocities[i][k] --;
                        velocities[j][k] ++;
                    }
                }
            }
        }

        for (int i = 0; i < moons.size(); i++)
        {
            for (int k = 0; k < 3; k++)
            {
                moons[i][k] += velocities[i][k];
            }
        }

        step++;

        for (int k = 0; k < 3; k++)
        {
            bool found = true;
            for (int i = 0; i < moons.size(); i++)
            {
                if (moons[i][k] != initialConfigs[k][i].first || velocities[i][k] != initialConfigs[k][i].second)
                {
                    found = false;
                    break;
                }
            }
            if(found && periods[k] == 0)
            { 
                periods[k] = step;
            }
        }

        if (periods[0] != 0 && periods[1] != 0 && periods[2] != 0)
        {
            break;
        }
    }
    
    int64_t numSteps = periods[0] * periods[1] / std::gcd(periods[0], periods[1]);
    numSteps = numSteps * periods[2] / std::gcd(numSteps, periods[2]);
    
    std::cout << numSteps << std::endl;

}

int main()
{
    Input input = ReadInput();

    Solve2(input);

    return 0;
}
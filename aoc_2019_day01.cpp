#include <cstdint>
#include <iostream>
#include <numeric>

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
    }

    return input;
}

void Solve1(Input& input)
{
    int64_t total = std::reduce(std::begin(input), std::end(input), 0, [](auto total, auto val) 
        { 
            return total + val / 3 - 2; 
        });

    std::cout << total << std::endl;
}

void Solve2(Input& input)
{
    int64_t total = std::reduce(std::begin(input), std::end(input), 0, [](auto total, auto val)
        {
            int64_t moduleFuel = 0;
            do
            {
                val = val / 3 - 2;
                if (val > 0)
                    moduleFuel += val;
            } while (val > 0);

            return total + moduleFuel;
        });
    
    std::cout << total << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
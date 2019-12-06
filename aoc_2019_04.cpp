#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <vector>

using Input = struct
{
    int start;
    int end;
};

void Solve1(const Input& input)
{
    int count = 0;
    for (int i = input.start; i <= input.end; i++)
    {
        auto nr = std::to_string(i);
        bool foundIdentical = false;
        bool valid = true;
        for (size_t j = 1; j < nr.size(); j++)
        {
            if (nr[j] == nr[j - 1])
                foundIdentical = true;
            else if (nr[j] < nr[j - 1])
            {
                valid = false;
                break;
            }
        }
        if (valid && foundIdentical)
            count++;
    }
    std::cout << count << "\n";
}

void Solve2(const Input& input)
{
    int count = 0;
    for (int i = input.start; i <= input.end; i++)
    {
        auto nr = std::to_string(i);
        bool foundIdentical = false;
        bool valid = true;
        for (size_t j = 1; j < nr.size(); j++)
        {
            if (nr[j] == nr[j - 1] && (j <= 1 || nr[j - 2] != nr[j]) && (j >= nr.size() - 1 || nr[j+1] != nr[j]))
                foundIdentical = true;
            else if (nr[j] < nr[j - 1])
            {
                valid = false;
                break;
            }
        }
        if (valid && foundIdentical)
            count++;
    }
    std::cout << count << "\n";
}

int main()
{
    Input input = { 156218, 652527 };

    Solve1(input);
    Solve2(input);

    return 0;
}
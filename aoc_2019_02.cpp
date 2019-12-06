#include <cassert>
#include <cstdint>
#include <iostream>
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

void Solve1(const Input& input)
{
    Input program = input;
    program[1] = 12;
    program[2] = 2;
    for (int64_t i = 0; i < program.size(); i+=4 )
    {
        if(program[i + 0] == 99)
            break;
        if (program[i + 0] == 1)
            program[program[i + 3]] = program[program[i + 1]] + program[program[i + 2]];
        else if (program[i + 0] == 2)
            program[program[i + 3]] = program[program[i + 1]] * program[program[i + 2]];
        else
        {
            assert(false);
        }
    }

    std::cout << program[0] << std::endl;
}

void Solve2(const Input& input)
{
    Input program = input;
    for (int64_t noun = 0; noun <= 99; noun++)
    {
        for (int64_t verb = 0; verb <= 99; verb++)
        {
            program = input;

            program[1] = noun;
            program[2] = verb;
            for (int64_t i = 0; i < program.size(); i += 4)
            {
                if (program[i + 0] == 99)
                    break;
                if (program[i + 0] == 1)
                    program[program[i + 3]] = program[program[i + 1]] + program[program[i + 2]];
                else if (program[i + 0] == 2)
                    program[program[i + 3]] = program[program[i + 1]] * program[program[i + 2]];
                else
                {
                    continue;
                }
            }
            if(program[0] == 19690720)
                std::cout << noun * 100 + verb << std::endl;
        }
    }
    
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
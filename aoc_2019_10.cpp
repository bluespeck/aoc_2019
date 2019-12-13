#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <vector>

using Input = std::vector<std::string>;

Input ReadInput()
{
    Input input;

    while (std::cin)
    {
        std::string line;
        if (std::getline(std::cin, line))
        {
            input.push_back(line);
        }
    }

    return input;
}

void Solve1(const Input& input)
{
    using Counts = std::vector<int>;
    Counts counts;
    counts.resize(input.size() * input[0].size(), 0);
    for(int jo = 0; jo < input.size(); jo++)
        for (int io = 0; io < input[jo].size(); io++)
        {
            if (input[jo][io] == '#')
            {
                for (int j = 0; j < input.size(); j++)
                    for (int i = 0; i < input[j].size(); i++)
                        if((i!= io || j != jo) && input[j][i] == '#')
                        {
                            int di = i - io;
                            int dj = j - jo;
                            auto divisor = std::gcd(di, dj);
                            di /= divisor;
                            dj /= divisor;
                            bool foundOne = true;
                            for (int jd = jo + dj, id = io + di; jd != j || id != i; jd += dj, id += di)
                            {
                                if (input[jd][id] == '#')
                                {
                                    foundOne = false;
                                    break;
                                }
                            }
                            if (foundOne)
                                counts[jo * input[0].size() + io] ++;
                        }
            }
        }

    auto result = std::max_element(counts.begin(), counts.end());
    std::cout << *result << std::endl;

    int y = (result - counts.begin()) / input[0].size();
    int x = (result - counts.begin()) % input[0].size();

}

struct point { int i; int j; };

bool less(point a, point b)
{
    // taken from https://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order
    point center{ 29,28 };
    if (a.i - center.i >= 0 && b.i - center.i < 0)
        return true;
    if (a.i - center.i < 0 && b.i - center.i >= 0)
        return false;
    if (a.i - center.i == 0 && b.i - center.i == 0) {
        if (a.j - center.j >= 0 || b.j - center.j >= 0)
            return a.j > b.j;
        return b.j > a.j;
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (a.i - center.i) * (b.j - center.j) - (b.i - center.i) * (a.j - center.j);
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    int d1 = (a.i - center.i) * (a.i - center.i) + (a.j - center.j) * (a.j - center.j);
    int d2 = (b.i - center.i) * (b.i - center.i) + (b.j - center.j) * (b.j - center.j);
    return d1 > d2;
}

bool greater(point a, point b)
{
    return !less(a, b);
}

void Solve2(const Input& input)
{
    
    std::vector<point> asteroids;
    int jo = 28;
    int io = 29;
        {
            if (input[jo][io] == '#')
            {
                for (int j = 0; j < input.size(); j++)
                    for (int i = 0; i < input[j].size(); i++)
                        if ((i != io || j != jo) && input[j][i] == '#')
                        {
                            int di = i - io;
                            int dj = j - jo;
                            auto divisor = std::gcd(di, dj);
                            di /= divisor;
                            dj /= divisor;
                            bool foundOne = true;
                            for (int jd = jo + dj, id = io + di; jd != j || id != i; jd += dj, id += di)
                            {
                                if (input[jd][id] == '#')
                                {
                                    foundOne = false;
                                    break;
                                }
                            }
                            if (foundOne)
                                asteroids.push_back({ i, j });
                        }
            }
        }

        auto q1 = std::count_if(asteroids.begin(), asteroids.end(), [](auto a) { return a.i >= 29 && a.j < 28; });
        auto q2 = std::count_if(asteroids.begin(), asteroids.end(), [](auto a) { return a.i >= 29 && a.j >= 28; });
        auto q3 = std::count_if(asteroids.begin(), asteroids.end(), [](auto a) { return a.i < 29 && a.j >= 28; });
        int index = 200 - (q1 + q2 + q3) - 1;
        std::vector<point> q4Points;
        for (auto& e : asteroids)
        {
            if (e.i < 29 && e.j < 28)
                q4Points.push_back(e);
        }

        std::sort(q4Points.begin(), q4Points.end(), greater);
        std::cout << q4Points[index].i * 100 + q4Points[index].j << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
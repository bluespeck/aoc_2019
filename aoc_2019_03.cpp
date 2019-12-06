#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <sstream>
#include <vector>
#include <optional>

using Point = struct { int64_t x, y; };
using Segment = struct { Point p1, p2; };
using Wire = std::vector<Point>;
using Input = std::array<Wire, 2>;

Input ReadInput()
{
    Input input;
    
    size_t wireIndex = 0;
    while (std::cin)
    {
        std::string line;
        if (std::getline(std::cin, line))
        {
            input[wireIndex].push_back({ 0,0 });
            auto& currentWire = input[wireIndex++];
            std::istringstream is{ line };
            while ( is)
            {
                char c;
                char dir;
                int64_t extent;
                if (is >> dir >> extent)
                {
                    const auto & currentPoint = currentWire[currentWire.size() - 1];
                    Point newPoint;
                    switch (dir)
                    {
                    case 'R':
                        newPoint.x = currentPoint.x + extent;
                        newPoint.y = currentPoint.y;
                        break;
                    case 'U':
                        newPoint.x = currentPoint.x;
                        newPoint.y = currentPoint.y + extent;
                        break;
                    case 'D':
                        newPoint.x = currentPoint.x;
                        newPoint.y = currentPoint.y - extent;
                        break;
                    case 'L':
                        newPoint.x = currentPoint.x - extent;
                        newPoint.y = currentPoint.y;
                        break;
                    }
                    currentWire.push_back(newPoint);
                }

                if(!(is >> c))
                    break;
            }
        }
    }

    return input;
}

bool IsHorizontalSegment(const Segment & s)
{
    return s.p1.y == s.p2.y;
}

bool IsVerticalSegment(const Segment & s)
{
    return s.p1.x == s.p2.x;
}

bool ArePerpendicular(const Segment & s1, const Segment & s2)
{
    return IsHorizontalSegment(s1) && IsVerticalSegment(s2) || IsHorizontalSegment(s2) && IsVerticalSegment(s1);
}

std::optional<Point> GetIntersect(const Segment & _s1, const Segment & _s2)
{
    if (ArePerpendicular(_s1, _s2))
    {
        Segment s1 = _s1;
        Segment s2 = _s2;
        if (IsVerticalSegment(_s1))
        {
            s1 = _s2;
            s2 = _s1;
        }
        
        if ((s1.p1.x <= s1.p2.x && s1.p1.x <= s2.p1.x && s2.p1.x <= s1.p2.x || s1.p2.x <= s2.p1.x && s2.p1.x <= s1.p1.x)
            && (s2.p1.y <= s2.p2.y && s2.p1.y <= s1.p1.y && s1.p1.y <= s2.p2.y || s2.p2.y <= s1.p1.y && s1.p1.y <= s2.p1.y))
        {
            return Point{ s2.p1.x, s1.p1.y };
        }
        else
        {
            return {};
        }
        
    }
    return {};
}

int64_t Norm(const Point & p)
{
    return std::abs(p.x) + std::abs(p.y);
}

int64_t Distance(const Point & p1, const Point & p2)
{
    return std::abs(p1.x - p2.x) + std::abs(p2.y - p1.y);
}

int64_t Length(const Segment & s)
{
    return Distance(s.p1, s.p2);
}

bool NormCompare(const Point & p1, const Point & p2)
{
    return Norm(p1) < Norm(p2);
}

void Solve1(const Input & input)
{
    std::vector<Point> intersects;
    auto& wire1 = input[0];
    auto& wire2 = input[1];

    for (size_t i = 1; i < wire1.size(); i++)
    {
        Segment s1{ wire1[i - 1], wire1[i] };
        for (size_t j = 1; j < wire2.size(); j++)
        {
            Segment s2{ wire2[j - 1], wire2[j] };
            auto optPoint = GetIntersect(s1, s2);
            if (optPoint.has_value())
            {
                auto value = optPoint.value();
                if (value.x != 0 || value.y != 0)
                {
                    intersects.push_back(optPoint.value());
                }
            }
        }
    }
    
    if (intersects.size() > 0)
    {
        auto it = std::min_element(std::begin(intersects), std::end(intersects), NormCompare);
        std::cout << std::abs(it->x) + std::abs(it->y) << "\n";
    }

}

void Solve2(const Input& input)
{
    using WireLength = int64_t;

    std::vector<WireLength> intersects;
    
    auto& wire1 = input[0];
    auto& wire2 = input[1];

    int64_t dist1 = 0;
    for (size_t i = 1; i < wire1.size(); i++)
    {
        Segment s1{ wire1[i - 1], wire1[i] };
        int64_t dist2 = 0;
        
        for (size_t j = 1; j < wire2.size(); j++)
        {
            Segment s2{ wire2[j - 1], wire2[j] };
            auto optPoint = GetIntersect(s1, s2);
            if (optPoint.has_value())
            {
                auto value = optPoint.value();
                if (value.x != 0 || value.y != 0)
                {
                    auto intersectDist1 = dist1 + Distance(s1.p1, value);
                    auto intersectDist2 = dist2 + Distance(s2.p1, value);
                    intersects.push_back(intersectDist1 + intersectDist2);
                }
            }
            dist2 += Length(s2);
        }
        dist1 += Length(s1);
    }

    if (intersects.size() > 0)
    {
        auto it = std::min_element(std::begin(intersects), std::end(intersects));
        std::cout << *it << "\n";
    }
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
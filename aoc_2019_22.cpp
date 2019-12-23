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
#include <immintrin.h>

enum OpType
{
    dealNew,
    dealIncrement,
    cut
};

struct OpInfo
{
    OpType type;
    int64_t n;
};


using Input = std::vector<OpInfo>;


Input ReadInput()
{
    Input input;

    const std::string dealIntoNewStack = "deal into new stack";
    const std::string cut = "cut";
    const std::string dealWithIncrement = "deal with increment";

    while (std::cin)
    {
        std::string value;
        if (std::getline(std::cin, value))
        {
            OpInfo newOpInfo;
            if (value.size() < dealIntoNewStack.size())
            {
                newOpInfo.type = OpType::cut;
                std::istringstream is(value.substr(cut.size() + 1));
                is >> newOpInfo.n;
            }
            else if (value.size() == dealIntoNewStack.size())
            {
                newOpInfo.type = OpType::dealNew;
                newOpInfo.n = 0;
            }
            else
            {
                newOpInfo.type = OpType::dealIncrement;
                std::istringstream is(value.substr(dealWithIncrement.size() + 1));
                is >> newOpInfo.n;
            }
            
            input.push_back(newOpInfo);
        }
    }

    return input;
}

void Solve1(const Input& input)
{
    std::vector<int64_t> cards(10007);
    std::iota(cards.begin(), cards.end(), 0);

    for (auto e : input)
    {
        switch (e.type)
        {
        case OpType::dealNew:
std::reverse(cards.begin(), cards.end());
break;
        case cut:
        {
            auto itStart = e.n > 0 ? cards.begin() : cards.begin() + cards.size() + e.n;
            auto itEnd = e.n > 0 ? cards.begin() + e.n : cards.end();
            std::vector<int64_t> cutCards(itStart, itEnd);
            cards.erase(itStart, itEnd);
            cards.insert(e.n > 0 ? cards.end() : cards.begin(), cutCards.begin(), cutCards.end());
            break;
        }
        case OpType::dealIncrement:
            size_t i = 0;
            std::vector<int64_t> newDeck(cards.size());
            for (auto card : cards)
            {
                newDeck[i] = card;
                i = (i + e.n) % cards.size();
            }
            cards = newDeck;
            break;
        }
    }

    //std::cout << "Result:";
    //for(auto card : cards)
    //{
    //    std::cout << " " << card;
    //}
    for (int i = 0; i < cards.size(); i++)
        if (cards[i] == 2019)
        {
            std::cout << i << std::endl;
            break;
        }

}

void Solve2_1(const Input& input)
{
    int64_t finalPos = 2020;
    int64_t numCards = 119315717514047;
    int64_t numIterations = 101741582076661;

    int64_t currentPosition = finalPos;

    for (int64_t iteration = 0; iteration < numIterations; iteration++)
    {
        for (int64_t i = input.size() - 1; i >= 0; i--)
        {
            auto e = input[i];
            switch (input[i].type)
            {
            case OpType::dealNew:
                currentPosition = numCards - 1 - currentPosition;
                break;
            case cut:
            {
                auto minusN = -e.n;
                if (minusN >= 0)
                {
                    if (minusN > currentPosition)
                        currentPosition += numCards - minusN;
                    else
                        currentPosition -= minusN;
                }
                else
                {
                    if (numCards - (-minusN) < currentPosition)
                    {
                        currentPosition -= numCards - (-minusN);
                    }
                    else
                    {
                        currentPosition += (-minusN);
                    }
                }
                break;
            }
            case OpType::dealIncrement:
            {
                for (int j = 0; j < e.n; j++)
                {
                    if ((currentPosition + j * numCards) % e.n == 0)
                    {
                        currentPosition = (currentPosition + j * numCards) / e.n;
                        break;
                    }
                }
                
                break;
            }
            }
        }
    }

    std::cout << currentPosition << "\n";
}

int64_t EuclidianRemainder(int64_t num, int64_t modulus)
{
    num = num % modulus;
    return (num + modulus) % modulus;
}

int64_t ModularMul(int64_t a, int64_t b, int64_t modulus)
{   
    int64_t result = 0;
    int64_t signA = a < 0 ? -1 : 1;
    int64_t signB = b < 0 ? -1 : 1;
    a = a < 0 ? -a : a;
    b = b < 0 ? -b : b;

    while (a != 0)
    {
        if (a & 1)
            result = (result + b) % modulus;
        a >>= 1;
        b = (b << 1) % modulus;
    }

    result *= signA * signB;
    result = EuclidianRemainder(result, modulus);
    return result;
}

int64_t ModularPow(int64_t base, int64_t exponent, int64_t modulus)
{
    //std::cout << "(" << base << " ^ " << exponent << " = ";
    int64_t result = 1;
    base = EuclidianRemainder(base, modulus);

    while (exponent > 0)
    {
        if (exponent & 1)
        {
            
            result = ModularMul(result, base, modulus); // prevents overflow
        }

        exponent >>= 1;
        base = ModularMul(base, base, modulus);
    }
    //std::cout << result << ")\n";
    return result;
}

void Solve2(const Input& input)
{
    // couldn't solve this so that it would run in a timely fashion on my own;
    // using the awesome explanation by mcpower on https://www.reddit.com/r/adventofcode/comments/ee0rqi/2019_day_22_solutions/fbnkaju/

    int64_t finalPos = 2020;
    int64_t numCards = 119315717514047;
    int64_t numIterations = 101741582076661;

    int64_t offset = 0;
    int64_t increment = 1;
    
    for (int i = 0; i < input.size(); i++)
    {
        switch (input[i].type)
        {
        case OpType::dealNew:
            increment = ModularMul(increment, -1, numCards);
            offset = EuclidianRemainder(offset + increment, numCards);
            break;
        case OpType::cut:
            offset += ModularMul(increment, input[i].n, numCards) ;
            break;
        case OpType::dealIncrement:
            increment = ModularMul(increment, ModularPow(input[i].n, numCards - 2, numCards), numCards);
            break;
        }
    }

    int64_t offsetDifference = offset;
    int64_t incrementMultiplier = increment;

    increment = ModularPow(incrementMultiplier, numIterations, numCards);
    offset = ModularMul(ModularMul(offsetDifference, (1 - increment), numCards),
                ModularPow(1 - incrementMultiplier, numCards - 2, numCards)
                , numCards);

    std::cout << EuclidianRemainder(offset + ModularMul(increment, 2020, numCards), numCards) << std::endl;
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    Solve2(input);

    return 0;
}
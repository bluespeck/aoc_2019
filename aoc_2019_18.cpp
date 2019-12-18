#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>

using Input = std::vector<std::string>;

Input ReadInput()
{
    Input input;

    while (std::cin)
    {
        std::string value;
        if (std::getline(std::cin, value))
        {
            if (value.size())
                input.push_back(value);
        }
    }

    return input;
}

struct Point
{
    int64_t x, y;
};

using Grid = std::vector<std::vector<int64_t>>;


using KeyDependencies = std::array<std::string, 128>;
using KeyPairDependencies = std::array<KeyDependencies, 128>;

using KeyPairDists = std::array<std::array<int64_t, 128>, 128>;
KeyPairDists keyPairDists;


void ExploreNeighbors(const Input& input, Grid& grid, const std::string& collectedKeys, Point pos, int64_t pathLength);

void ExploreFromPoint(const Input& input, Grid& grid, const std::string& collectedKeys, Point pos, int64_t pathLength = 1)
{
    auto& cell = grid[pos.y][pos.x];
    if (cell > 0 && cell < pathLength)
        return;

    auto& ch = input[pos.y][pos.x];
    if (ch == '#')
    {
        return;
    }
    else if (ch == '.')
    {
        cell = pathLength;
        ExploreNeighbors(input, grid, collectedKeys, pos, pathLength);
    }
    else if (ch >= 'A' && ch <= 'Z')
    {
        if (collectedKeys.find(ch + 0x20) != std::string::npos)
        {
            cell = pathLength;
            ExploreNeighbors(input, grid, collectedKeys, pos, pathLength);
        }
        else
            return;
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        cell = pathLength;
        ExploreNeighbors(input, grid, collectedKeys, pos, pathLength);
    }
    else if (ch == '@')
    {
        cell = pathLength;
        ExploreNeighbors(input, grid, collectedKeys, pos, pathLength);
    }
}

void ExploreNeighbors(const Input& input, Grid& grid, const std::string& collectedKeys, Point pos, int64_t pathLength)
{
    ExploreFromPoint(input, grid, collectedKeys, { pos.x + 1, pos.y }, pathLength + 1);
    ExploreFromPoint(input, grid, collectedKeys, { pos.x - 1, pos.y }, pathLength + 1);
    ExploreFromPoint(input, grid, collectedKeys, { pos.x, pos.y + 1 }, pathLength + 1);
    ExploreFromPoint(input, grid, collectedKeys, { pos.x, pos.y - 1 }, pathLength + 1);
}

Point FindStartingPos(const Input& input)
{
    Point startingPos{ 0,0 };
    for (int i = 0; i < input.size(); i++)
    {
        auto it = std::find(input[i].begin(), input[i].end(), '@');
        if (it != input[i].end())
        {
            startingPos.x = it - input[i].begin();
            startingPos.y = i;
        }
    }
    return startingPos;
}

using KeyPositions = std::vector<Point>;

KeyPositions FindKeys(const Input& input)
{
    KeyPositions keyPositions;
    for (int i = 0; i < input.size(); i++)
    {
        for (int j = 0; j < input[i].size(); j++)
        {
            auto ch = input[i][j];
            if (ch >= 'a' && ch <= 'z')
                keyPositions.push_back({ j, i });
        }
    }
    return keyPositions;
}

KeyPositions FindKeysExploredInGrid(const Input& input, const KeyPositions& allKeys, const Grid& grid, std::string collectedKeys)
{
    KeyPositions foundKeys;
    for (const auto& key : allKeys)
    {
        if (grid[key.y][key.x] > 0 && collectedKeys.find(input[key.y][key.x]) == std::string::npos)
        {
            foundKeys.push_back(key);
        }
    }
    return foundKeys;
}

KeyPositions allKeys;
int64_t minPath = std::numeric_limits<int64_t>::max();
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

void BackTrackingToRuleThemAll(const Input& input, Grid& grid, std::string& collectedKeys, Point pos, int64_t pathLength = 0)
{
    if (pathLength > minPath)
        return;


    if (collectedKeys.size() == allKeys.size())
    {
        if (minPath > pathLength)
        {
            minPath = pathLength;
            std::cout << collectedKeys << " ";
            std::cout << pathLength << "\n";
        }
        return;
    }

    for (auto& v : grid)
    {
        std::memset(&v[0], 0, v.size() * sizeof(int64_t));
    }
    ExploreFromPoint(input, grid, collectedKeys, pos);

    auto keysExplored = FindKeysExploredInGrid(input, allKeys, grid, collectedKeys);

    // cache values of interest from the grid so that they don't get stomped by other calls to Backtracking
    int64_t lengthsToKey[128];
    for (auto& key : keysExplored)
    {
        lengthsToKey[input[key.y][key.x]] = grid[key.y][key.x];
    }

    std::sort(keysExplored.begin(), keysExplored.end(),
        [&lengthsToKey, &input](auto key1, auto key2)
        {
            return lengthsToKey[input[key1.y][key1.x]] < lengthsToKey[input[key2.y][key2.x]];
        });

    if (collectedKeys.size() < 10)
    {
        std::shuffle(keysExplored.begin(), keysExplored.end(), std::default_random_engine(seed));
    }

    for (auto& key : keysExplored)
    {
        auto keyChar = input[key.y][key.x];
        if (collectedKeys.find(keyChar) == std::string::npos)
        {
            collectedKeys.push_back(keyChar);
            BackTrackingToRuleThemAll(input, grid, collectedKeys, key, pathLength + lengthsToKey[keyChar] - 1);
            collectedKeys.pop_back();
        }
    }
}

void FloodFill(Input& input, Point pos)
{
    int64_t i = pos.y;
    int64_t j = pos.x;
    if (input[i][j] == '.' || (input[i][j] >= 'A' && input[i][j] <= 'Z'))
    {
        int count = 0;
        if (input[i - 1][j] == '#')
            count++;
        if (input[i + 1][j] == '#')
            count++;
        if (input[i][j - 1] == '#')
            count++;
        if (input[i][j + 1] == '#')
            count++;
        if (count >= 3)
        {
            if (input[i][j] >= 'A' && input[i][j] <= 'Z')
                std::cout << input[i][j] << std::endl;

            input[i][j] = '#';
            FloodFill(input, { j - 1, i });
            FloodFill(input, { j + 1, i });
            FloodFill(input, { j, i - 1 });
            FloodFill(input, { j, i + 1 });
        }
    }
}

void PrepareInput(Input& input)
{
    bool found = false;
    do
    {
        found = false;
        for (int64_t i = 1; i < input.size() - 1 && found == false; i++)
            for (int64_t j = 1; j < input[i].size() - 1; j++)
            {
                if (input[i][j] == '.' || (input[i][j] >= 'A' && input[i][j] <= 'Z'))
                {
                    int count = 0;
                    if (input[i - 1][j] == '#')
                        count++;
                    if (input[i + 1][j] == '#')
                        count++;
                    if (input[i][j - 1] == '#')
                        count++;
                    if (input[i][j + 1] == '#')
                        count++;
                    if (count >= 3)
                    {
                        FloodFill(input, { j, i });
                        found = true;
                        break;
                    }
                }
            }
    } while (found == true);
}

void ExploreNoDoors(const Input& input, Grid& grid, Point pos, int64_t pathLength = 1)
{
    if (input[pos.y][pos.x] == '#')
        return;
    if (grid[pos.y][pos.x] > 0 && grid[pos.y][pos.x] < pathLength)
        return;
    grid[pos.y][pos.x] = pathLength;
    ExploreNoDoors(input, grid, { pos.x + 1, pos.y }, pathLength + 1);
    ExploreNoDoors(input, grid, { pos.x - 1, pos.y }, pathLength + 1);
    ExploreNoDoors(input, grid, { pos.x, pos.y + 1 }, pathLength + 1);
    ExploreNoDoors(input, grid, { pos.x, pos.y - 1 }, pathLength + 1);
}

KeyPairDependencies FindKeyDependencies(const Input& input, Grid& grid, Point playerPos)
{
    KeyPairDependencies keyDeps;

    for (auto i = 0; i <= allKeys.size(); i++)
    {
        char startKeyChar;
        Point startPos;
        if (i < allKeys.size())
        {
            startPos = allKeys[i];
            startKeyChar = input[startPos.y][startPos.x];
        }
        else
        {
            startPos = playerPos;
            startKeyChar = 0;
        }
        for (auto& v : grid)
        {
            std::memset(&v[0], 0, v.size() * sizeof(int64_t));
        }
        ExploreNoDoors(input, grid, startPos);

        for (int j = 0; j < allKeys.size(); j++)
        {
            if (i == j)
                continue;
            auto endPos = allKeys[j];
            auto endKeyChar = input[endPos.y][endPos.x];

            auto length = grid[endPos.y][endPos.x];
            keyPairDists[startKeyChar][endKeyChar] = length - 1;
            auto& deps = keyDeps[startKeyChar][endKeyChar];
            while (!(endPos.x == startPos.x && endPos.y == startPos.y))
            {
                if (input[endPos.y][endPos.x] >= 'A' && input[endPos.y][endPos.x] <= 'Z')
                {
                    deps.push_back(input[endPos.y][endPos.x] + 0x20);
                }
                length--;
                if (grid[endPos.y - 1][endPos.x] == length)
                {
                    endPos.y--;
                }
                else if (grid[endPos.y + 1][endPos.x] == length)
                {
                    endPos.y++;
                }
                else if (grid[endPos.y][endPos.x - 1] == length)
                    endPos.x--;
                else if (grid[endPos.y][endPos.x + 1] == length)
                {
                    endPos.x++;
                }
                else
                    assert(false);
            }
            std::reverse(deps.begin(), deps.end());
        }
    }
    return keyDeps;
}

bool PathIsValid(std::string path, const KeyPairDependencies& keyPairDependencies)
{
    if (keyPairDependencies[0][path[0]].size() > 0)
        return false;
    for (int i = 1; i < path.size(); i++)
    {
        const auto& deps = keyPairDependencies[path[i - 1]][path[i]];
        for (int k = 0; k < deps.size(); k++)
        {
            bool found = false;
            for (int j = 0; j < i - 1; j++)
            {
                if (deps[k] == path[j])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
        }
    }
    return true;
}

int64_t GetPathLength(std::string path)
{
    int64_t length = keyPairDists[0][path[0]];
    for (int i = 1; i < path.size(); i++)
        length += keyPairDists[path[i - 1]][path[i]];
    return length;
}

void DijkstraLike(const Input& input, Grid& grid, Point pos)
{
    using PathLengths = std::unordered_map<std::string, int64_t>;
    using KeychainDistances = std::array<PathLengths, 128>;

    KeychainDistances dists;

    auto keyDependencies = FindKeyDependencies(input, grid, pos);

    struct KeyAndChain
    {
        char key;
        std::string chain;
    };
    std::queue<KeyAndChain> toVisit;
    toVisit.push({ 0, "" });

    int64_t minLength = std::numeric_limits<int64_t>::max();

    while (!toVisit.empty())
    {
        char key = toVisit.front().key;
        auto chain = toVisit.front().chain;
        toVisit.pop();

        //for (auto e: allKeys)
        //{
        //    auto ch = input[e.y][e.x];
        for(auto ch = 'a'; ch <= 'z'; ch++)
        {
            if (chain.find(ch) != chain.npos)
                continue;

            bool dependenciesMet = true;
            for (auto e : keyDependencies[0][ch])
            {
                if (chain.find(e) == chain.npos)
                {
                    dependenciesMet = false;
                    break;
                }
            }
            if (!dependenciesMet)
                continue;
            
            auto newKeyChain = chain + ch;
            std::sort(newKeyChain.begin(), newKeyChain.end());

            auto& newKeychainMap = dists[ch];

            if (newKeychainMap.find(newKeyChain) != newKeychainMap.end())
            {
                if (newKeychainMap[newKeyChain] > dists[key][chain] + keyPairDists[key][ch])
                {
                    newKeychainMap[newKeyChain] = dists[key][chain] + keyPairDists[key][ch];
                }
                else
                    continue;
            }
            else
            {
                newKeychainMap[newKeyChain] = dists[key][chain] + keyPairDists[key][ch];
                if (newKeyChain.size() == allKeys.size())
                {
                    if (newKeychainMap[newKeyChain] < minLength)
                    {
                        minLength = newKeychainMap[newKeyChain];
                        std::cout << minLength << std::endl;
                    }
                }
                toVisit.push({ ch, newKeyChain });
            }
        }
    }
}

void Solve1_1(const Input& initialInput)
{
    auto input = initialInput;

    PrepareInput(input);

    auto startingPos = FindStartingPos(input);
    allKeys = FindKeys(input);
    Grid grid(input.size());
    for (auto& e : grid)
    {
        e.resize(input[0].size());
    }

    auto keyDependencies = FindKeyDependencies(input, grid, startingPos);

    auto minPathLength = std::numeric_limits<int64_t>::max();
    std::string path;
    path.resize('z' - 'a' + 1);
    std::iota(path.begin(), path.end(), 'a');

    while (std::next_permutation(path.begin(), path.end()))
    {
        if (PathIsValid(path, keyDependencies))
        {
            auto pathLength = GetPathLength(path);
            if (pathLength < minPathLength)
            {
                minPathLength = pathLength;
                std::cout << path << "\n";
            }
        }
        std::cout << path << "\n";
    }
}

void Solve1_2(const Input& initialInput)
{
    auto input = initialInput;

    PrepareInput(input);
    for (int i = 0; i < input.size(); i++)
    {
        std::cout << input[i] << std::endl;
    }

    std::string collectedKeys;
    std::string deadendKeys = "cvnahztk";

    auto startingPos = FindStartingPos(input);
    allKeys = FindKeys(input);
    Grid grid(input.size());
    for (auto& e : grid)
    {
        e.resize(input[0].size());
    }

    auto keyDependencies = FindKeyDependencies(input, grid, startingPos);

    // for (auto ch = 'a'; ch <= 'z'; ch++)
    // {
    //     for (auto ch1 = 'a'; ch1 <= 'z'; ch1++)
    //     {
    //         std::cout << ch << ch1 << " dist: " << keyPairDists[ch][ch1] << "   depends on " << keyDependencies[ch][ch1] << std::endl;
    //     }
    // }
    BackTrackingToRuleThemAll(input, grid, collectedKeys, startingPos);

    std::cout << minPath;
}

void Solve1(const Input& initialInput)
{
    auto input = initialInput;

    PrepareInput(input);
    for (int i = 0; i < input.size(); i++)
    {
        std::cout << input[i] << std::endl;
    }

    auto startingPos = FindStartingPos(input);
    allKeys = FindKeys(input);
    Grid grid(input.size());
    for (auto& e : grid)
    {
        e.resize(input[0].size());
    }
    DijkstraLike(input, grid, startingPos);
}

int main()
{
    Input input = ReadInput();

    Solve1(input);
    return 0;
}
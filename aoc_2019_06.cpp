#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>

#include <string>
#include <vector>

struct Edge
{
    std::string first;
    std::string second;
};
using Input = std::vector<Edge>;

struct Node
{
    std::string name;
    Node* parent;
    std::vector<Node*> children;
};

Input ReadInput()
{
    Input input;

    while (std::cin)
    {
        std::string line;
        if (std::getline(std::cin, line))
        {
            Edge edge{ line.substr(0, 3), line.substr(4) };
            input.push_back(edge);
        }
    }

    return input;
}

Node* FindNode(Node* node, const std::string& value)
{
    if (node->name == value)
        return node;
    else
    {
        if (node->children.size() > 0)
        {
            Node* foundNode = nullptr;
            for (auto node : node->children)
            {
                foundNode = FindNode(node, value);
                if(foundNode != nullptr)
                    break;
            }
            return foundNode;
        }
    }
    return nullptr;
}

Node* BuildTree(const Input& input)
{
    Node * com = new Node{ "COM", nullptr, {} };
    auto edges = input;
    while (edges.size())
    {
        for (size_t i = 0; i < edges.size(); i++)
        {
            auto node = FindNode(com, edges[i].first);
            if (node != nullptr)
            {
                node->children.push_back(new Node{edges[i].second, node, {} });
                edges[i] = edges.back();
                edges.resize(edges.size() - 1);
                break;
            }
        }
    }
    return com;
}

void PreOrder(Node* node, int level, int64_t& sumLevels)
{
    if (node != nullptr)
    {
        sumLevels += level;
        for (auto child : node->children)
        {
            PreOrder(child, level + 1, sumLevels);
        }
    }
}

void Solve1(const Input& input, Node* com)
{
    int64_t sum = 0;
    PreOrder(com, 0, sum);
    std::cout << sum << "\n";
}

std::vector<Node*> PathToRoot(Node* node)
{
    std::vector<Node*> path;
    while (node != nullptr && node->name != "COM" )
    {
        path.push_back(node);
        node = node->parent;
    }
    return path;
}

void Solve2(const Input& input, Node* com)
{
    Node* you = FindNode(com, "YOU");
    Node* san = FindNode(com, "SAN");

    std::vector<Node*> youPath = PathToRoot(you->parent);
    std::vector<Node*> sanPath = PathToRoot(san->parent);
    
    auto itYou = youPath.rbegin();
    auto itSan = sanPath.rbegin();
    while (itYou != youPath.rend() && itSan != sanPath.rend() && (*itYou)->name == (*itSan)->name)
    {
        itYou++;
        itSan++;
    }
    
    std::vector<Node*> youPath2(itYou, youPath.rend());
    std::vector<Node*> sanPath2(itSan, sanPath.rend());
    std::cout << youPath2.size() + sanPath2.size() << std::endl;
}

void DeleteTree(Node* root)
{
    for (auto& node : root->children)
    {
        DeleteTree(node);
    }
    delete root;
}

int main()
{
    Input input = ReadInput();

    Node* com = BuildTree(input);
    Solve1(input, com);
    Solve2(input, com);

    DeleteTree(com);
    return 0;
}
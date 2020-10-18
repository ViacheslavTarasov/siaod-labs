#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cctype>

using namespace std;

const string LEVEL_SEPARATOR = ".";
const string AND_STRING = "AND";
const string OR_STRING = "OR";

enum NodeType
{
    NODETYPE_DATA,
    NODETYPE_OR,
    NODETYPE_AND
};

struct NodeWeightInterval
{
    int min = 0;
    int max = 0;
};

NodeType getNodeType(string operand)
{
    if (operand == OR_STRING)
    {
        return NODETYPE_OR;
    }

    if (operand == AND_STRING)
    {
        return NODETYPE_AND;
    }

    return NODETYPE_DATA;

    // throw "Invalid node type operand";
}

string nodeTypeToString(NodeType nodeType)
{
    if (nodeType == NODETYPE_AND)
    {
        return AND_STRING;
    }
    if (nodeType == NODETYPE_OR)
    {
        return OR_STRING;
    }
    return "";
}

int readMaxWeight(ifstream &in)
{
    int maxWeight;
    if (in.is_open())
    {
        if (in >> maxWeight)
        {
            return maxWeight;
        }
    }
    throw "Read max weight error";
}

struct Node
{
    int level;
    int weight;
    NodeType nodeType;
    Node *parent;
    vector<Node *> children;
    NodeWeightInterval *weightInterval;

    ~Node()
    {
        delete weightInterval;
        for (Node *n : children)
        {
            delete n;
        }
    }
};

Node *createNode(string line)
{
    int level = 0;
    NodeType nodeType;
    string linePart;
    int weight;
    Node *node;

    size_t lastLSPos = line.find_first_not_of(LEVEL_SEPARATOR);
    if (lastLSPos != string::npos)
    {
        level = lastLSPos;
    }
    
    linePart = line.substr(lastLSPos, line.size());
    nodeType = getNodeType(linePart);
    if (nodeType == NODETYPE_DATA)
    {
        weight = stoi(linePart);
        if (weight == 0)
        {
            throw "Weight not parsed";
        }
    }

    node = new Node();
    node->level = level;
    node->nodeType = nodeType;
    node->weight = weight;
    node->weightInterval = new NodeWeightInterval;
    return node;
}

string nodeToString(Node *node)
{
    string result = "";
    if (node->level)
    {
        for (int i = 0; i < node->level; i++)
        {
            result += LEVEL_SEPARATOR;
        }
    }
    if (node->nodeType == NODETYPE_DATA)
    {
        result += to_string(node->weight);
    }
    else
    {
        result += nodeTypeToString(node->nodeType);
    }
    return result;
}

Node *getRoot(Node *node, int level)
{
    if (level < 1)
    {
        throw "Invalid parsed level. Too small";
    }
    if (level > (node->level + 1))
    {
        throw "Invalid parsed level. Too large";
    }
    if (level == (node->level + 1))
    {
        return node;
    }

    return getRoot(node->parent, level);
}

Node *readTree(ifstream &in)
{
    string line;
    Node *node;
    Node *root;
    Node *tree = NULL;
    if (in.is_open())
    {
        while (in >> line)
        {
            node = createNode(line);
            if (node->level == 0 && tree != NULL)
            {
                throw "Tree root already exists";
            }

            if (tree == NULL)
            {
                tree = node;
                root = node;
                continue;
            }

            root = getRoot(root, node->level);
            node->parent = root;
            root->children.push_back(node);

            if (node->level > root->level)
            {
                root = node;
            }
        }
    }

    return tree;
}

void printTree(Node *node)
{
    cout << nodeToString(node) << " [" << node->weightInterval->min << "; " << node->weightInterval->max << "]" << endl;
    for (int i = 0; i < node->children.size(); i++)
    {
        printTree(node->children[i]);
    }
}

void writeNoSolutions(Node *node, ofstream &out)
{
    out << "No solutions: tree weight inteval "
        << " [" << node->weightInterval->min << "; " << node->weightInterval->max << "]" << endl;
}

void writeTree(Node *node, ofstream &out)
{
    out << nodeToString(node) << endl;
    for (int i = 0; i < node->children.size(); i++)
    {
        writeTree(node->children[i], out);
    }
}

void calculateWeightIntervals(Node *node)
{
    if (node->nodeType == NODETYPE_DATA)
    {
        node->weightInterval->min = node->weight;
        node->weightInterval->max = node->weight;
        return;
    }

    for (int i = 0; i < node->children.size(); i++)
    {
        calculateWeightIntervals(node->children[i]);

        if (node->nodeType == NODETYPE_AND)
        {
            node->weightInterval->min += node->children[i]->weightInterval->min;
            node->weightInterval->max += node->children[i]->weightInterval->max;
        }

        if (node->nodeType == NODETYPE_OR)
        {
            if (node->weightInterval->min == 0 || node->weightInterval->min > node->children[i]->weightInterval->min)
            {
                node->weightInterval->min = node->children[i]->weightInterval->min;
            }

            if (node->weightInterval->max == 0 || node->weightInterval->max < node->children[i]->weightInterval->max)
            {
                node->weightInterval->max = node->children[i]->weightInterval->max;
            }
        }
    }
}

bool hasSolution(Node *node, int maxWeight)
{
    return node->weightInterval->min <= maxWeight;
}

void filterTreeByMaxWeight(Node *node, int maxWeight)
{
    if (!hasSolution(node, maxWeight))
    {
        throw "No solutions: min node weight is " + to_string(node->weightInterval->min) + ", but max weight is " + to_string(maxWeight);
    }

    if (node->weightInterval->max <= maxWeight)
    {
        return;
    }

    for (int i = 0; i < node->children.size();)
    {
        if (node->nodeType == NODETYPE_OR && !hasSolution(node->children[i], maxWeight))
        {
            delete node->children[i];
            node->children.erase(node->children.begin() + i);
        }
        else
        {
            int newMaxWeight = maxWeight - (node->weightInterval->min - node->children[i]->weightInterval->min);
            filterTreeByMaxWeight(node->children[i], newMaxWeight);
            i++;
        }
    }
}

int main()
{
    string inputFileName = "input.txt";
    string outputFileName = "output.txt";
    int maxWeight;
    Node *tree;
    ifstream in;
    ofstream out;

    in.open(inputFileName);
    maxWeight = readMaxWeight(in);
    tree = readTree(in);
    in.close();

    calculateWeightIntervals(tree);

    // printTree(tree);

    out.open(outputFileName);
    out << "Max weight: " << maxWeight << endl;
    if (!hasSolution(tree, maxWeight))
    {
        writeNoSolutions(tree, out);
    }
    else
    {
        filterTreeByMaxWeight(tree, maxWeight);
        writeTree(tree, out);
    }
    out.close();

    delete tree;
    tree = nullptr;
    cout << "End of program" << endl;
    return 0;
}
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
const string AND_STRING = "&";
const string OR_STRING = "|";

enum NodeType
{
    NODETYPE_DATA,
    NODETYPE_OR,
    NODETYPE_AND
};

NodeType getNodeType(char operand)
{
    if (operand == '|')
    {
        return NODETYPE_OR;
    }

    if (operand == '&')
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
};

Node *createNode(string line)
{
    int level = 0;
    NodeType nodeType;
    string op;
    int weight;
    Node *node;
    size_t opPos = 0;
    size_t weightPos = 0;

    size_t lastLSPos = line.find_first_not_of(LEVEL_SEPARATOR);
    if (lastLSPos != string::npos)
    {
        level = lastLSPos;
        opPos = lastLSPos;
        weightPos = lastLSPos;
    }

    nodeType = getNodeType(line[opPos]);
    if (nodeType == NODETYPE_DATA)
    {
        weight = atoi(line.c_str() + weightPos);
        if (weight == 0)
        {
            throw "Weight not parsed";
        }
    }

    node = new Node();
    node->level = level;
    node->nodeType = nodeType;
    node->weight = weight;
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
    cout << nodeToString(node) << endl;    
    for (int i = 0; i < node->children.size(); i++)
    {
        printTree(node->children[i]);
    }
}


void writeTree(Node *node, ofstream &out)
{
    out << nodeToString(node) << endl;    
    for (int i = 0; i < node->children.size(); i++)
    {
        writeTree(node->children[i], out);
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


    cout << "Max weight: " << maxWeight << endl;

    // delete --> 
    cout << "Tree: " << endl;
    printTree(tree);
    // <-- delete

    out.open(outputFileName);
    writeTree(tree, out);
    out.close();

    cout << "End of program" << endl;
    return 0;
}
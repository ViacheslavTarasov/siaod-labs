/** Графы. 19
 * Имеется  сеть  автомобильных  дорог.  Для каждой дороги
 * известна максимальная масса груза, которую можно  провезти  по 
 * этой   дороге.   С   помощью  алгоритма  Дейкстры   определить 
 * максимальный   груз,   который  можно  провести  между   двумя 
 * указанными городам
  */

#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <stack>

using namespace std;

const int INF = numeric_limits<int>::max();
const int INF_MINUS = numeric_limits<int>::min();

struct StartEnd
{
    int start;
    int end;
    StartEnd(int s, int e) : start(s), end(e)
    {
    }
};

StartEnd readStartEnd(ifstream &in)
{
    string line;
    if (in.is_open() && getline(in, line))
    {
        int start, end;
        istringstream iss(line);
        if (iss >> start >> end)
        {
            return StartEnd(start - 1, end - 1);
        }
    }

    throw "Read StartEnd error";
}

int readNumberOfPoints(ifstream &in)
{
    string line;
    if (in.is_open() && getline(in, line))
    {
        return stoi(line);
    }
    throw "Read number of points error";
}

void fillCostMatrix(ifstream &in, int costMatrix[], int size)
{
    string line;

    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            costMatrix[i * size + j] = 0;
        }
    }

    if (in.is_open())
    {
        while (getline(in, line))
        {
            int from, to, weight;
            istringstream iss(line);
            if (!(iss >> from >> to >> weight))
            {
                throw "Read cost error";
            }

            if (from > size)
            {
                throw "Read departure error";
            }

            if (to > size)
            {
                throw "Read destination error";
            }

            costMatrix[(from - 1) * size + (to - 1)] = weight;
        }
    }
}

void writeCostMatrix(ofstream &out, int costMatrix[], int size)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            out << costMatrix[i * size + j] << " ";
        }
        out << endl;
    }
}

void writeResult(ofstream &out, stack<int> result, int maxWeight)
{
    if (!out.is_open())
    {
        throw "Write result error";
    }

    if (result.empty())
    {
        out << "path not found";
    }
    else
    {
        out << "Max weight: " << maxWeight << endl;
    }

    while (!result.empty())
    {
        out << " -> " << result.top();
        result.pop();
    }
}

stack<int> pathToStack(int path[], StartEnd startEnd)
{
    stack<int> result;
    int current = startEnd.end;
    if (path[current] == INF)
    {
        return result;
    }
    result.push(current + 1);
    while (path[current] != startEnd.start && path[current] != current && path[current] != INF)
    {
        current = path[current];
        result.push(current + 1);
    }
    if (path[current] != current)
    {
        result.push(path[current] + 1);
    }

    return result;
}

int main()
{
    string inputFileName = "input.txt";
    string outputFileName = "output.txt";
    ifstream in;
    ofstream out;
    string line;
    int numberOfPoints;

    in.open(inputFileName);
    StartEnd startEnd = readStartEnd(in);
    numberOfPoints = readNumberOfPoints(in);
    int costMatrix[numberOfPoints][numberOfPoints];
    fillCostMatrix(in, &costMatrix[0][0], numberOfPoints);
    in.close();

    // --> implementation of Dijkstra's algorithm
    int tempPoints[numberOfPoints];
    int constPoints[numberOfPoints];
    int path[numberOfPoints];
    for (size_t i = 0; i < numberOfPoints; i++)
    {
        path[i] = INF;
        tempPoints[i] = i == startEnd.start ? INF : INF_MINUS;
        constPoints[i] = i == startEnd.start ? INF : INF_MINUS;
    }

    bool changed = true;
    int current = startEnd.start;
    int maxOfTempPoints = current;
    do
    {
        changed = false;
        for (size_t i = 0; i < numberOfPoints; i++)
        {
            if (constPoints[i] != INF_MINUS)
            {
                continue;
            }

            if (costMatrix[current][i] != 0)
            {
                int minCapacity = std::min(constPoints[current], costMatrix[current][i]);
                int temp = std::max(minCapacity, tempPoints[i]);
                if (temp > tempPoints[i])
                {
                    tempPoints[i] = temp;
                    path[i] = current;
                    changed = true;
                }
            }

            if (tempPoints[i] == INF_MINUS)
            {
                continue;
            }

            bool isFirstMaxOfTempPoints = maxOfTempPoints == current;
            if (isFirstMaxOfTempPoints || tempPoints[i] > tempPoints[maxOfTempPoints])
            {
                maxOfTempPoints = i;
                changed = true;
            }
        }
        current = maxOfTempPoints;
        constPoints[current] = tempPoints[current];
    } while (changed);

    // <-- implementation of Dijkstra's algorithm

    stack<int> result = pathToStack(path, startEnd);

    out.open(outputFileName);
    // writeCostMatrix(out, &costMatrix[0][0], numberOfPoints);
    writeResult(out, result, constPoints[startEnd.end]);
    out.close();

    cout << "End of program" << endl;
    return 0;
}
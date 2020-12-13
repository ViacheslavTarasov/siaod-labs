/** Графы. 19
    Квадратное озеро с островами задается матрицей размером NxN (1<=N<=500).  Каждый элемент матрицы содержит либо  символ '@' (собака),  обозначающий территорию, 
которая включает часть острова,  либо  символ  '.'  (точка),   обозначающий   участок свободной   воды.   В   левом  верхнем  углу  озера  находится квадратный плот 
размером MxM (1<=M<N) клеток. За один шаг плот может  сдвигаться на одну клетку по горизонтали или вертикали.
В  каждый  момент  плавания  не  должно  быть  общих   клеток, принадлежащих как плоту,  так и какому-либо острову. Требуется определить минимальное  число  шагов,  
необходимых  для  того, чтобы плот достиг правого нижнего угла озера.

   Ввод. В первой строке входного файла  INPUT.TXT  содержатся числа  N  и  M,  разделенные пробелами.  В следующих N строках находится матрица,  
представляющая озеро,  по N подряд  идущих символов  в  строке.  Подматрица  размером MxM,  находящаяся в левом верхнем углу,  не содержит островов,  то есть  начальное
положение плота всегда допустимо.

   Вывод. Выходной файл OUTPUT.TXT должен содержать  в  первой строке  единственное число - количество необходимых шагов.  Во второй строке выдается путь перемещения
верхнего  левого  угла плота  в виде последовательности координат клеток.  Координаты клетки  задаются  в  скобках  двумя  числами  через   запятую,
соответствующими   номерам  строки  и  столбца.  Если  правого нижнего угла  достичь  невозможно,  то  выходной  файл  должен содержать No (12). 
   Время работы программы не более 1 сек

   Пример
Ввод
7 2
.......
...@...
.......
..@....
.......
.......
....@..
Вывод
10
(1,1) (2,1) (3,1) (4,1) (5,1) (5,2) (5,3) (5,4) (5,5) (5,6) (6,6)   
  */

#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <stack>
#include <queue>
#include <iomanip>
#include <chrono>

#include <algorithm>
#include <string>

using namespace std;

class Timer
{
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1>>;

    std::chrono::time_point<clock_t> m_beg;

public:
    Timer() : m_beg(clock_t::now())
    {
    }

    void reset()
    {
        m_beg = clock_t::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }
};

const char WATER_CHAR = '.';
const char ISLAND_CHAR = '@';

const int START = 0;
const int FINISH = numeric_limits<int>::max();
const int WATER = -1;
const int ISLAND = -2;

struct Point
{
    int x, y, waveNumber;
    Point(int x, int y, int w) : x(x), y(y), waveNumber(w)
    {
    }
};

enum Directions
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

struct Sizes
{
    int lake;
    int raft;
    Sizes(int l, int r) : lake(l), raft(r)
    {
    }
};

Sizes readSizes(ifstream &in)
{
    string line;
    if (in.is_open() && getline(in, line))
    {
        int lakeSize, raftSize;
        istringstream iss(line);
        if (iss >> lakeSize >> raftSize)
        {
            if (raftSize < 1 || lakeSize - raftSize < 1)
            {
                throw "Invalid sizes error";
            }
            return Sizes(lakeSize, raftSize);
        }
    }

    throw "Read Sizes error";
}

string pointToString(Point point)
{
    return "(" + to_string(point.x + 1) + "; " + to_string(point.y + 1) + ") "; // + to_string(point.waveNumber);
}

int convertCharToMapItem(char ch)
{
    if (ch == WATER_CHAR)
    {
        return WATER;
    }
    if (ch == ISLAND_CHAR)
    {
        return ISLAND;
    }
    throw "invalid char of map";
}

void readLakeMap(ifstream &in, int lakeMap[], int size)
{
    string line;

    if (!in.is_open())
    {
        throw "stream is closed";
    }

    for (size_t i = 0; i < size; i++)
    {
        if (!getline(in, line))
        {
            throw "too few lines";
        }

        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        if (line.size() != size)
        {
            throw "invalid line length";
        }
        for (size_t j = 0; j < size; j++)
        {
            lakeMap[i * size + j] = convertCharToMapItem(line[j]);
        }
    }
}

void writeLakeMap(ofstream &out, int lakeMap[], int size)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            out << right << setw(4) << lakeMap[i * size + j];
        }
        out << endl;
    }
}

void writeResult(ofstream &out, stack<Point> path)
{
    if (path.empty())
    {
        out << "No";
        return;
    }
    out << to_string(path.size() - 1) << endl;
    while (!path.empty())
    {
        out << pointToString(path.top()) << " ";
        path.pop();
    }
}

Point createPoint(Point point, Directions direction, int waveNumber)
{
    switch (direction)
    {
    case Directions::UP:
        return Point(point.x, point.y - 1, waveNumber);
    case Directions::RIGHT:
        return Point(point.x + 1, point.y, waveNumber);
    case Directions::DOWN:
        return Point(point.x, point.y + 1, waveNumber);
    case Directions::LEFT:
        return Point(point.x - 1, point.y, waveNumber);
    default:
        throw "Invalid direction";
    }
}

Point getPrevPoint(Point point, int lakeMap[], Sizes sizes)
{
    Directions directions[4] = {Directions::UP, Directions::RIGHT, Directions::DOWN, Directions::LEFT};
    for (const auto direction : directions)
    {
        Point prevPoint = createPoint(point, direction, point.waveNumber - 1);

        if (prevPoint.x < 0 || prevPoint.x >= sizes.lake || prevPoint.y < 0 || prevPoint.y >= sizes.lake)
        {
            continue;
        }
        int mapValue = lakeMap[prevPoint.y * sizes.lake + prevPoint.x];
        if (mapValue == prevPoint.waveNumber)
        {
            return prevPoint;
        }
    }
    throw "previos point not found";
}

Point getNextPoint(Point point, Directions direction)
{
    int newWaveNumber = point.waveNumber + 1;
    return createPoint(point, direction, newWaveNumber);
}

bool raftHasVerticalObstacle(Point point, int newX, int lakeMap[], Sizes sizes)
{
    for (size_t i = 0; i < sizes.raft; i++)
    {
        int y = point.y + i;
        if (lakeMap[(point.y + i) * sizes.lake + newX] == ISLAND)
        {
            return true;
        }
    }

    return false;
}

bool raftHasHorizontalObstacle(Point point, int newY, int lakeMap[], Sizes sizes)
{
    for (size_t i = 0; i < sizes.raft; i++)
    {
        if (lakeMap[newY * sizes.lake + point.x + i] == ISLAND)
        {
            return true;
        }
    }

    return false;
}

bool isWater(int x, int y, int lakeMap[], Sizes sizes)
{
    return lakeMap[y * sizes.lake + x] == WATER;
}

bool canMoveUp(Point point, int lakeMap[], Sizes sizes)
{
    int newY = point.y - 1;
    if (newY < 0 || !isWater(point.x, newY, &lakeMap[0], sizes))
    {
        return false;
    }

    return !raftHasHorizontalObstacle(point, newY, lakeMap, sizes);
}

bool canMoveRight(Point point, int lakeMap[], Sizes sizes)
{
    int newX = point.x + 1;
    int newXRight = point.x + sizes.raft;

    if (newXRight > sizes.lake - 1 || lakeMap[point.y * sizes.lake + newX] != WATER)
    {
        return false;
    }

    return !raftHasVerticalObstacle(point, newXRight, lakeMap, sizes);
}

bool canMoveDown(Point point, int lakeMap[], Sizes sizes)
{
    int newY = point.y + 1;
    int newYDown = point.y + sizes.raft;
    if (newYDown > sizes.lake - 1 || !isWater(point.x, newY, &lakeMap[0], sizes))
    {
        return false;
    }

    return !raftHasHorizontalObstacle(point, newYDown, lakeMap, sizes);
}

bool canMoveLeft(Point point, int lakeMap[], Sizes sizes)
{
    int newX = point.x - 1;

    if (newX < 0 || !isWater(newX, point.y, &lakeMap[0], sizes))
    {
        return false;
    }

    return !raftHasVerticalObstacle(point, newX, lakeMap, sizes);
}

bool raftInFinish(Point point, Sizes sizes)
{
    return point.y + sizes.raft == sizes.lake && point.x + sizes.raft == sizes.lake;
}

bool canMove(Point fromPoint, Directions direction, int lakeMap[], Sizes sizes)
{
    switch (direction)
    {
    case Directions::UP:
        return canMoveUp(fromPoint, lakeMap, sizes);
    case Directions::RIGHT:
        return canMoveRight(fromPoint, lakeMap, sizes);
    case Directions::DOWN:
        return canMoveDown(fromPoint, lakeMap, sizes);
    case Directions::LEFT:
        return canMoveLeft(fromPoint, lakeMap, sizes);
    default:
        throw "Invalid direction";
    }
}

void markLakeWithWaveNumber(Point point, int lakeMap[], Sizes sizes)
{
    lakeMap[point.y * sizes.lake + point.x] = point.waveNumber;
}

stack<Point> getPath(int lakeMap[], Sizes sizes)
{
    int raftFinishX = sizes.lake - sizes.raft;
    int raftFinishY = sizes.lake - sizes.raft;
    stack<Point> path;

    int waveNumberInFinishPoint = lakeMap[raftFinishY * sizes.lake + raftFinishY];

    if (waveNumberInFinishPoint < 0)
    {
        return path;
    }

    Point point = Point(raftFinishX, raftFinishY, waveNumberInFinishPoint);
    path.push(point);

    while (point.waveNumber > 0)
    {
        point = getPrevPoint(point, &lakeMap[0], sizes);
        path.push(point);
    }

    return path;
}

void searchPath(int lakeMap[], Sizes sizes)
{
    Directions directions[4] = {Directions::UP, Directions::RIGHT, Directions::DOWN, Directions::LEFT};
    queue<Point> points;
    bool isFinished = false;
    int waveNumber = 0;
    Point start = Point(0, 0, waveNumber);
    points.push(start);
    markLakeWithWaveNumber(start, &lakeMap[0], sizes);

    while (!points.empty() && !isFinished)
    {
        Point currentPoint = points.front();
        points.pop();

        if (raftInFinish(currentPoint, sizes))
        {
            isFinished = true;
        }

        for (const auto direction : directions)
        {
            if (canMove(currentPoint, direction, lakeMap, sizes))
            {
                Point nextPoint = getNextPoint(currentPoint, direction);
                points.push(nextPoint);
                markLakeWithWaveNumber(nextPoint, &lakeMap[0], sizes);
            }
        }
    }
}

int main()
{
    Timer timer;
    string inputFileName = "input.txt";
    string outputFileName = "output.txt";
    ifstream in;
    ofstream out;
    string line;

    in.open(inputFileName);
    Sizes sizes = readSizes(in);
    int lakeSize = sizes.lake;
    int lakeMap[lakeSize][lakeSize];

    readLakeMap(in, &lakeMap[0][0], lakeSize);
    in.close();

    searchPath(&lakeMap[0][0], sizes);

    out.open(outputFileName);
    writeLakeMap(out, &lakeMap[0][0], lakeSize);
    out << endl;
    writeResult(out, getPath(&lakeMap[0][0], sizes));
    out.close();

    printf("The time: %f seconds\n", timer.elapsed());
    printf("End of program\n");
    return 0;
}
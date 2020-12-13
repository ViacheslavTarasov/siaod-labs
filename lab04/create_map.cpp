
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
const char WATER_CHAR = '.';
const char ISLAND_CHAR = '@';

const int START = 0;
const int WATER = -1;
const int ISLAND = -2;

void createMap()
{
    string inputFileName = "input3.txt";
    ofstream out;
    int lakeSize = 303;
    int raftSize = 1;
    int counter = 1;
    out.open(inputFileName);

    out << lakeSize << " " << raftSize << endl;

    for (size_t i = 0; i < lakeSize; i++)
    {
        for (size_t j = 0; j < lakeSize; j++)
        {
            counter++;
            if (j % 2 == 0)
            {
                out << WATER_CHAR;
            }
            else
            {
                if (i == 0 && (counter - 1) % 4 == 0)
                {
                    out << WATER_CHAR;
                    continue;
                }

                if (i == lakeSize - 1 && (counter - 1) % 4 == 0)
                {
                    out << WATER_CHAR;
                    continue;
                }
                out << ISLAND_CHAR;
            }
        }
        out << endl;
    }
    out.close();
}

int main()
{
    createMap();
    printf("End of program\n");
    return 0;
}
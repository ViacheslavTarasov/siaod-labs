// #include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const string LEFT_QUEUE = "left the queue";
const string STARTED_OBSERVING = "started observing";
const string FINISHED_OBSERVING = "finished observing";
const string WENT_TO_THE_END = "went to the end of queue";

struct Spy
{
    string name;
    int observationTime;
    int waitingTime;

    Spy(string n, int o, int w) : name(n), observationTime(o), waitingTime(w)
    {
    }
};

class SpyQueue
{
private:
    struct Node
    {
        Spy spy;
        Node *next;
        Node(Spy s) : spy(s), next(NULL) {}
    };

    Node *head;
    Node *tail;

public:
    SpyQueue();
    ~SpyQueue();

    void push(Spy $spy);
    Spy pop();
    bool isEmpty();
};

SpyQueue::SpyQueue()
{
    head = NULL;
    tail = NULL;
}

SpyQueue::~SpyQueue()
{
    while (head != NULL)
    {
        Node *temp = head;
        head = head->next;
        delete temp;
    }
}

void SpyQueue::push(Spy spy)
{
    Node *node = new Node(spy);
    if (head == NULL)
    {
        head = node;
    }
    else
    {
        tail->next = node;
    }
    tail = node;
}

Spy SpyQueue::pop()
{
    if (head == NULL)
    {
        throw std::runtime_error("Empty queue!");
    }
    Spy spy = head->spy;
    Node *newHead = head->next;
    delete head;
    head = newHead;

    return spy;
}

bool SpyQueue::isEmpty()
{
    return !head;
}

SpyQueue initSpyQueue(string inputFileName)
{
    string name;
    int observationTime;
    int waitingTime;
    ifstream in(inputFileName);

    SpyQueue spyQueue;

    if (in.is_open())
    {
        while (in >> name >> observationTime >> waitingTime)
        {
            spyQueue.push(Spy(name, observationTime, waitingTime));
        }
    }
    in.close();

    return spyQueue;
}

string getLogLine(int passedMinutes, Spy spy, string action)
{
    return to_string(passedMinutes) + ": " + spy.name + " " + action;
}

int calculateObsrvationTime(Spy spy, int passedMinutes)
{
    int plannedEndOfObservation = passedMinutes + spy.observationTime;

    if (plannedEndOfObservation > spy.waitingTime)
    {
        return spy.waitingTime - passedMinutes;
    }

    return spy.observationTime;
}

void processSpyQueue(SpyQueue spyQueue, string outputFileName)
{
    ofstream out(outputFileName);
    int passedMinutes = 0;

    if (out.is_open())
    {
        while (!spyQueue.isEmpty())
        {
            Spy spy = spyQueue.pop();

            if (spy.waitingTime <= passedMinutes || spy.observationTime < 1)
            {
                out << getLogLine(passedMinutes, spy, LEFT_QUEUE) << endl;
                continue;
            }

            int observationTime = calculateObsrvationTime(spy, passedMinutes);

            out << getLogLine(passedMinutes, spy, STARTED_OBSERVING) << endl;
            passedMinutes += observationTime;
            out << getLogLine(passedMinutes, spy, FINISHED_OBSERVING) << endl;
            if (spy.waitingTime <= passedMinutes)
            {
                out << getLogLine(passedMinutes, spy, LEFT_QUEUE) << endl;
            }
            else
            {
                spyQueue.push(spy);
                out << getLogLine(passedMinutes, spy, WENT_TO_THE_END) << endl;
            }
            // out << spy.name << " " << spy.observationTime << " " << spy.waitingTime << endl;
        }
    }
    out.close();
}

int main()
{
    string inputFileName = "input.txt";
    string outputFileName = "output.txt";
    SpyQueue spyQueue = initSpyQueue(inputFileName);
    processSpyQueue(spyQueue, outputFileName);
    cout << "End of program" << std::endl;
    return 0;
}
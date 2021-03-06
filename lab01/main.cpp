/**
 * Линейные списки. 16 
 * У множества шпионов,  собравшихся вместе для наблюдения
 *  секретного     объекта,    имеется    единственный    бинокль.
 * Сформировалась очередь на этот  бинокль.  Для  каждого  шпиона
 * задан   период   наблюдения   в  минутах  и  предельное  время
 * нахождения в очереди. После наблюдения каждый шпион становится
 * снова  в  конец  очереди.  Как  только  для какого-либо шпиона
 * истекает предельное время нахождения в очереди, он покидает ее
 * (даже  если  в  этот момент владеет биноклем) и отправляется к
 * резиденту. Вывести протокол наблюдения шпионов за объектом
 **/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

    vector<Spy> filterByPassesdTime(int passedTime);
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

vector<Spy> SpyQueue::filterByPassesdTime(int passedTime)
{
    vector<Spy> outgoingSpyes;
    Node *previos = head;
    Node *current = head;
    Node *priorityHead = NULL;
    Node *priorityCurrent = NULL;

    while (current != NULL)
    {
        if (current->spy.waitingTime >= passedTime)
        {
            previos = current;
            current = current->next;
            continue;
        }

        Node *temp = current;

        if (previos == current)
        {
            head = head->next;
            previos = current->next;
        }
        else
        {
            previos->next = current->next;
        }

        current = current->next;

        if (priorityHead == NULL)
        {
            temp->next = NULL;
            priorityHead = temp;
        }
        else
        {
            priorityCurrent = priorityHead;
            Node *priorityPrevios = priorityHead;
            while (priorityCurrent != NULL)
            {
                if (temp->spy.waitingTime < priorityCurrent->spy.waitingTime)
                {
                    if (priorityCurrent == priorityHead)
                    {
                        temp->next = priorityHead;
                        priorityHead = temp;
                    }
                    else
                    {
                        temp->next = priorityCurrent;
                        priorityPrevios->next = temp;
                    }
                    break;
                }
                priorityPrevios = priorityCurrent;
                priorityCurrent = priorityCurrent->next;
            }
        }
    }

    while (priorityHead != NULL)
    {
        priorityCurrent = priorityHead;
        outgoingSpyes.push_back(priorityHead->spy);
        priorityHead = priorityHead->next;

        delete priorityCurrent;
    }

    return outgoingSpyes;
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

            vector<Spy> outgoingSpyes = spyQueue.filterByPassesdTime(passedMinutes);
            for (int i = 0; i < outgoingSpyes.size(); i++)
            {
                out << getLogLine(outgoingSpyes[i].waitingTime, outgoingSpyes[i], LEFT_QUEUE) << endl;
            }

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
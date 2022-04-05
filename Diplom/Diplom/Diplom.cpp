#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "json.hpp"
#include <fstream>
using namespace std;
using json = nlohmann::json;

class Entrant
{
private:
    vector<pair<string, vector<float>>> priorities;//Первый элемент пары - цифра направления, Второй элемент пары - строка с баллами (Нулевое число - сумма баллов, первое, второе, третье и так далее - баллы по предметам, последнее - доп баллы)
    int currentprior;//Текущий приоритет, типа сейчас мы смотрим 0 приоритет, значит текущий приоритет 0, если по нашему первому приоритету мы не прошли, тогда текущий приоритет будет 1 и так далее
    int PersonNumber;//Число, характеризующее абитуриента

public:
    vector<float> balls()
    {
        return priorities[currentprior].second;
    }

    void initialization(const int& IdentNumOfPer, const vector<pair<string, vector<float>>>& PrioritiesAndScores)
    {
        currentprior = 0;
        PersonNumber = IdentNumOfPer;
        priorities = PrioritiesAndScores;
    }

    int getCurrPrior()
    {
        return currentprior;
    }

    void increaseCurrentPrior()
    {
        ++currentprior;
        return;
    }

    int getPrioritiesSize()
    {
        return priorities.size();
    }

    string getCurrPriorNapravlenie()
    {
        return priorities[currentprior].first;
    }

    int getPersonNumber()
    {
        return PersonNumber;
    }

    bool operator >=(const Entrant& A)
    {
        for (size_t i = 0; i < this->priorities[currentprior].second.size(); ++i)
        {
            if (this->priorities[currentprior].second[i] > A.priorities[A.currentprior].second[i])
                return true;
            if (this->priorities[currentprior].second[i] < A.priorities[A.currentprior].second[i])
                return false;
        }
        return true;
    }
};

void InsertInDirection(vector<Entrant>& Entrants, const int& EntrantId, unordered_map<string, vector<int>>& direction, const string& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior);

void ThrowEntrantIdFurther(vector<Entrant>& Entrants, const int& EntrantId, unordered_map<string, vector<int>>& direction, const string& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior)
{
    //if (EntrantId == 0) cout << "Stakan " << EntrantId << endl;
    if (Entrants[EntrantId].getPrioritiesSize() - 1 == Entrants[EntrantId].getCurrPrior())
        return;
    if (Entrants[EntrantId].getCurrPrior() == NumOfCurrPrior)
    {
        Outsiders.push_back(EntrantId);
        Entrants[EntrantId].increaseCurrentPrior();
        return;
    }
    Entrants[EntrantId].increaseCurrentPrior();
    InsertInDirection(Entrants, EntrantId, direction, Entrants[EntrantId].getCurrPriorNapravlenie(), Outsiders, NumOfCurrPrior);
    return;
}



//Рекурсивная вставка абитуриентов по направлениям
void InsertInDirection(vector<Entrant>& Entrants, const int& EntrantId, unordered_map<string, vector<int>>& direction, const string& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior)//Вставляет id в направление в порядке убывания
{
    if (direction[DirectionId].size() == 0)
    {
        direction[DirectionId].push_back(EntrantId);
        return;
    }
    vector<int>::iterator itr = --direction[DirectionId].end();
    int NextEntrantId;
    while (itr != direction[DirectionId].begin())
    {
        if (Entrants[*itr] >= Entrants[EntrantId])
        {
            if (direction[DirectionId].size() == direction[DirectionId].capacity())
            {
                if (itr == --direction[DirectionId].end())
                {
                    ThrowEntrantIdFurther(Entrants, EntrantId, direction, DirectionId, Outsiders, NumOfCurrPrior);
                    return;
                }
                NextEntrantId = *(--direction[DirectionId].end());
                direction[DirectionId].pop_back();
                direction[DirectionId].insert(++itr, EntrantId);
                ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, DirectionId, Outsiders, NumOfCurrPrior);
                return;
            }
            direction[DirectionId].insert(++itr, EntrantId);
            return;
        }
        else
        {
            --itr;
        }
    }
     
    if (Entrants[*itr] >= Entrants[EntrantId])
    {
        if (direction[DirectionId].size() == direction[DirectionId].capacity())
        {
            NextEntrantId = *(--direction[DirectionId].end());
            direction[DirectionId].pop_back();
            direction[DirectionId].insert(++itr, EntrantId);
            ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, DirectionId, Outsiders, NumOfCurrPrior);
            return;
        }
        direction[DirectionId].insert(++itr, EntrantId);
    }
    else
    {
        if (direction[DirectionId].size() == direction[DirectionId].capacity())
        {
            NextEntrantId = *(--direction[DirectionId].end());
            direction[DirectionId].pop_back();
            direction[DirectionId].insert(itr, EntrantId);
            ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, DirectionId, Outsiders, NumOfCurrPrior);
            return;
        }
        direction[DirectionId].insert(itr, EntrantId);
    }

}

void DistributionOfEntrantsByDirections(vector<Entrant>& entrants, unordered_map<string, vector<int>>& direction, const int& PriorityesMaxCount)// Засовываю абитуриентов в их стаканы
{// В этих цеклах я прохожу по приориетам абитуриентов, типа вначале смотрю первый приоритет всех абитуриентов, потом второй и так далее
    vector<int> Outsiders;
    vector<int> OutsidersInPrevCircle;
    for (int j = 0; j < entrants.size(); ++j)
    {
        InsertInDirection(entrants, j, direction, entrants[j].getCurrPriorNapravlenie(), Outsiders, 0);
    }
    for (int i = 1; i < PriorityesMaxCount; ++i)
    {
        OutsidersInPrevCircle = Outsiders;
        Outsiders.clear();
        if (OutsidersInPrevCircle.size() == 0)
            return;
        for (int j = 0; j < OutsidersInPrevCircle.size(); ++j)
            InsertInDirection(entrants, OutsidersInPrevCircle[j], direction, entrants[OutsidersInPrevCircle[j]].getCurrPriorNapravlenie(), Outsiders, i);
    }
    return;
}

//При вводе номера приоритета, я уменьшаю его на 1 для удобной записи в вектор
void get_from_json(unordered_map<string, vector<int>>& direction, vector<Entrant>& entrants, int& maxCountOfPriorityes)
{
    json reader;
    std::ifstream cfgfile("inputfile.json");//Name of our file
    //ifstream - читать из файла
    cfgfile >> reader;
    for (auto i = reader["competitions"].begin(); i != reader["competitions"].end(); ++i)
    {
        vector<int> count;
        count.reserve((*i)["count"]);
        direction[(*i)["id"]] = move(count);
    }

    maxCountOfPriorityes = 0;
    entrants.reserve(reader["entrants"].size());
    for (auto i = reader["entrants"].begin(); i != reader["entrants"].end(); ++i)
    {
        int PersonNum = (*i)["id"];

        string s = "";
        vector<float> f;
        vector<pair<string, vector<float>>> Priorityes((*i)["competitions"].size(), std::make_pair(move(s), move(f)));

        if (maxCountOfPriorityes < (*i)["competitions"].size()) maxCountOfPriorityes = (*i)["competitions"].size();
        vector<int> arr;
        arr.reserve((*i)["competitions"].size());
        for (auto j = (*i)["competitions"].begin(); j != (*i)["competitions"].end(); ++j)
        {
            arr.push_back((*j)["priority"]);
        }
        sort(arr.begin(), arr.end());

        for (auto j = (*i)["competitions"].begin(); j != (*i)["competitions"].end(); ++j)
        {
            int num = 0;
            for (int i = 0; i < arr.size(); ++i)
            {
                if (arr[i] == (*j)["priority"])
                    num = i;
            }
            string NumOfDirections = (*j)["competition_id"];
            Priorityes[num].first = NumOfDirections;
            Priorityes[num].second.reserve((*j)["balls"].size());
            for (auto k = (*j)["balls"].begin(); k != (*j)["balls"].end(); ++k)
            {
                Priorityes[num].second.push_back(*k);
            }
        }
        Entrant e;
        e.initialization(PersonNum, Priorityes);
        //cout << PersonNum << " ";
        entrants.push_back(e);
        //cout << PersonNum << " ";
    }
}

void set_to_json(unordered_map<string, vector<int>>& direction, vector<Entrant>& entrants)
{
    json writer;
    json miniwriter;
    for (auto i = direction.begin(); i != direction.end(); ++i)
    {
        /*
        "9": [
        {
            "id": 1,
            "balls": [
                300,
                100,
                100,
                100,
                0,
                0,
            ],
            "priority": 3
        },
        ...
        ],

        */
        for (auto j = (*i).second.begin(); j != (*i).second.end(); ++j)
        {
            miniwriter["id"] = entrants[*j].getPersonNumber();

            miniwriter["balls"] = entrants[*j].balls();

            miniwriter["priority"] = entrants[*j].getCurrPrior() + 1;

            writer[i->first].push_back(miniwriter);

        }
        //writer[i->first].push_back(miniwriter);
    }
    std::ofstream o("outputfile.json");
    //std::setw(4) - выводит в красивом виде, как объекты
    o << std::setw(4) << writer;
}

int main()// 
{
    unordered_map<string, vector<int>> direction;
    vector<Entrant> entrants;
    int maxCountOfPriorityes;
    get_from_json(direction, entrants, maxCountOfPriorityes);


    /*
    int i, j;
    int DirectionCount;
    cin >> DirectionCount;//Количество направлений
    vector<vector<int>> Direction;//Массив направлений(стаканов), в которых будут храниться внутренний id абитуриентов
    Direction.reserve(DirectionCount);
    for (i = 0; i < DirectionCount; ++i)
    {
        int size;
        cin >> size;//Вводится количество мест на каждом направлении
        Direction[i].reserve(size);
    }
    int CountOfEntrants;//Количество передаваемых абитуриентов
    cin >> CountOfEntrants;
    vector<Entrant> Entrants;//Массив абитуриентов
    Entrants.reserve(CountOfEntrants);
    int maxCountOfPriorityes = 0;//Счётчик максимального количества направлений у одного абитуриента
    for (i = 0; i < CountOfEntrants; ++i)// Здесь происходит ввод внешнего персонального номера человека, количества его приоритетов, самих этих приоритетов, количество баллов для этих приоритетов(Для бакалавриата и специалитета будет вводиться 5 или 6, сумма предметов, сами эти предметы в приоритетном порядке, доп баллы и ещё можно в самый конец приоритетное право) и самих этих баллов
    {
        int PersonNum;
        cin >> PersonNum;
        vector<pair<int, vector<float>>> Priorityes;
        int PriorityesNum;
        cin >> PriorityesNum;// Сюда вводится количество приориететов
        if (PriorityesNum > maxCountOfPriorityes)
            maxCountOfPriorityes = PriorityesNum;
        Priorityes.reserve(PriorityesNum);
        for (j = 0; j < PriorityesNum; ++j)
        {
            int NumOfDirections;
            cin >> NumOfDirections;
            Priorityes[j].first = NumOfDirections;
            int NumOfScores;
            cin >> NumOfScores;
            Priorityes[j].second.reserve(NumOfScores);
            for (int k = 0; k < NumOfScores; ++k)
            {
                int Score;
                cin >> Score;
                Priorityes[j].second.push_back(Score);
            }
        }
        Entrants[i].initialization(PersonNum, Priorityes);
    }*/





    DistributionOfEntrantsByDirections(entrants, direction, maxCountOfPriorityes);

    /*for (auto i = direction["1"].begin(); i != direction["1"].end(); ++i)
    {
        cout << *i;
    }
    for (auto i = direction["2"].begin(); i != direction["2"].end(); ++i)
    {
        cout << *i;
    }
    for (auto i = direction["3"].begin(); i != direction["3"].end(); ++i)
    {
        cout << *i;
    }*/
    set_to_json(direction, entrants);
    
    return 0;
}
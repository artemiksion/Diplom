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
    vector<pair<int, vector<float>>> priorities;//Первый элемент пары - цифра направления, Второй элемент пары - строка с баллами (Нулевое число - сумма баллов, первое, второе, третье и так далее - баллы по предметам, последнее - доп баллы)
    int currentprior;//Текущий приоритет, типа сейчас мы смотрим 0 приоритет, значит текущий приоритет 0, если по нашему первому приоритету мы не прошли, тогда текущий приоритет будет 1 и так далее
    int PersonNumber;//Число, характеризующее абитуриента

public:
    vector<float> balls()
    {
        return priorities[currentprior].second;
    }

    void initialization(const int& IdentNumOfPer, const vector<pair<int, vector<float>>>& PrioritiesAndScores)
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

    int getCurrPriorNapravlenie()
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

    json get_competitions_json()
    {
        json competitions;
        for (int i = 0; i < this->priorities.size(); ++i)
        {
            json mini_comp;
            mini_comp["balls"] = priorities[i].second;
            mini_comp["priority"] = i + 1;
            mini_comp["competition_id"] = priorities[i].first;
            competitions.push_back(mini_comp);
        }
        return competitions;
    }
};

void InsertInDirection(vector<Entrant>& Entrants, const int& EntrantId, unordered_map<int, vector<int>>& direction, const int& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior, vector<int>& full_outsiders);

void ThrowEntrantIdFurther(vector<Entrant>& Entrants, const int& EntrantId, unordered_map<int, vector<int>>& direction, vector<int>& Outsiders, const int NumOfCurrPrior, vector<int>& full_outsiders)
{
    if (Entrants[EntrantId].getPrioritiesSize() - 1 == Entrants[EntrantId].getCurrPrior())
    {
        full_outsiders.push_back(EntrantId);
        return;
    }
    if (Entrants[EntrantId].getCurrPrior() == NumOfCurrPrior)
    {
        Outsiders.push_back(EntrantId);
        Entrants[EntrantId].increaseCurrentPrior();
        return;
    }
    Entrants[EntrantId].increaseCurrentPrior();
    if (Entrants[EntrantId].getCurrPriorNapravlenie() == 0) {
        cout << "We find person with 0 DirectionID: " << Entrants[EntrantId].getPersonNumber() << endl;
    }
    InsertInDirection(Entrants, EntrantId, direction, Entrants[EntrantId].getCurrPriorNapravlenie(), Outsiders, NumOfCurrPrior, full_outsiders);
    return;
}




void InsertInDirection(vector<Entrant>& Entrants, const int& EntrantId, unordered_map<int, vector<int>>& direction, const int& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior, vector<int>& full_outsiders)
{
    if (direction.count(DirectionId) == 0) {
        cout << "Function start with DirectionId == 0" << endl;
    }

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
                    ThrowEntrantIdFurther(Entrants, EntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
                    return;
                }
                NextEntrantId = *(--direction[DirectionId].end());
                direction[DirectionId].pop_back();
                direction[DirectionId].insert(++itr, EntrantId);
                ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
                return;
            }
            direction[DirectionId].insert(++itr, EntrantId);
            return;
        }
        else
            --itr;
    }
     
    if (Entrants[*itr] >= Entrants[EntrantId])
    {
        if (direction[DirectionId].size() == direction[DirectionId].capacity())
        {
            NextEntrantId = *(--direction[DirectionId].end());
            direction[DirectionId].pop_back();
            direction[DirectionId].insert(++itr, EntrantId);//ERROR Вернуть ++, когда уберу cout
            ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
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
            ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
            return;
        }
        direction[DirectionId].insert(itr, EntrantId);
    }

}

void DistributionOfEntrantsByDirections(vector<Entrant>& entrants, unordered_map<int, vector<int>>& direction, const int& PriorityesMaxCount, vector<int>& full_outsiders)
{
    vector<int> Outsiders;
    vector<int> OutsidersInPrevCircle;
    for (int j = 0; j < entrants.size(); ++j)
    {
        //entrants[j].getCurrPriorNapravlenie() должен существовать в direction
        if (direction.count(entrants[j].getCurrPriorNapravlenie()) != 0)
        {
            InsertInDirection(entrants, j, direction, entrants[j].getCurrPriorNapravlenie(), Outsiders, 0, full_outsiders);
        }
        else {
            entrants[j].increaseCurrentPrior();
        }
    }
    for (int i = 1; i < PriorityesMaxCount; ++i)
    {
        OutsidersInPrevCircle = Outsiders;
        Outsiders.clear();
        if (OutsidersInPrevCircle.size() == 0)
            return;
        for (int j = 0; j < OutsidersInPrevCircle.size(); ++j)
        {
            if (direction.count(entrants[OutsidersInPrevCircle[j]].getCurrPriorNapravlenie()) != 0)
            {
                InsertInDirection(entrants, OutsidersInPrevCircle[j], direction, entrants[OutsidersInPrevCircle[j]].getCurrPriorNapravlenie(), Outsiders, i, full_outsiders);
            }
            else {
                entrants[j].increaseCurrentPrior();
            }
        }
    }
    return;
}

//При вводе номера приоритета, я уменьшаю его на 1 для удобной записи в вектор
void get_from_json(unordered_map<int, vector<int>>& direction, vector<Entrant>& entrants, int& maxCountOfPriorityes)
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

        int s = 0;
        vector<float> f;
        vector<pair<int, vector<float>>> Priorityes((*i)["competitions"].size(), std::make_pair(move(s), move(f)));

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
            int NumOfDirections = (*j)["competition_id"];
            Priorityes[num].first = NumOfDirections;
            Priorityes[num].second.reserve((*j)["balls"].size());
            for (auto k = (*j)["balls"].begin(); k != (*j)["balls"].end(); ++k)
            {
                Priorityes[num].second.push_back(*k);
            }
        }
        Entrant e;
        e.initialization(PersonNum, Priorityes);
        entrants.push_back(e);
    }
}

void set_to_json(unordered_map<int, vector<int>>& direction, vector<Entrant>& entrants, vector<int>& full_outsiders)
{
    json writer;
    
    json macrowriter;
    for (auto i = direction.begin(); i != direction.end(); ++i)
    {
        json miniwriter;
        miniwriter["competition_id"] = i -> first;
        for (auto j = (*i).second.begin(); j != (*i).second.end(); ++j)
        {
            macrowriter["id"] = entrants[*j].getPersonNumber();

            macrowriter["balls"] = entrants[*j].balls();

            macrowriter["priority"] = entrants[*j].getCurrPrior() + 1;

            miniwriter["entrants"].push_back(macrowriter);
        }
        writer.push_back(miniwriter);
    }
    std::ofstream o("outputfile.json");
    o << std::setw(4) << writer;
    o.close();

    json outsiders;
    for (auto i = full_outsiders.begin(); i != full_outsiders.end(); ++i)
    {
        json mini_outsiders;
        mini_outsiders["id"] = entrants[*i].getPersonNumber();
        mini_outsiders["competitions"] = entrants[*i].get_competitions_json();

        outsiders.push_back(mini_outsiders);
    }
    std::ofstream f("outsiders_output_file.json");

    f << std::setw(4) << outsiders;
    f.close();
}

int main()
{
    unordered_map<int, vector<int>> direction;
    vector<Entrant> entrants;
    int maxCountOfPriorityes;
    vector<int> full_outsiders;
    get_from_json(direction, entrants, maxCountOfPriorityes);

    DistributionOfEntrantsByDirections(entrants, direction, maxCountOfPriorityes, full_outsiders);

    set_to_json(direction, entrants, full_outsiders);
    
    return 0;
}
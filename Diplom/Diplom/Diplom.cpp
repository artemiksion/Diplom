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
    vector<pair<int, unordered_map<string, int>>> priorities;//Первый элемент пары - цифра направления, Второй элемент пары - строка с баллами (Нулевое число - сумма баллов, первое, второе, третье и так далее - баллы по предметам, последнее - доп баллы)
    int currentprior;//Текущий приоритет, типа сейчас мы смотрим 0 приоритет, значит текущий приоритет 0, если по нашему первому приоритету мы не прошли, тогда текущий приоритет будет 1 и так далее
    int PersonNumber;//Число, характеризующее абитуриента

public:
    unordered_map<string, int> balls()
    {
        return priorities[currentprior].second;
    }

    void initialization(const int& IdentNumOfPer, const vector<pair<int, unordered_map<string, int>>>& PrioritiesAndScores)
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

    bool operator >=(Entrant& A)//Error here
    {
        if (this->priorities[currentprior].second["total_points"] > A.priorities[A.currentprior].second["total_points"]) {
            return true;
        }
        if (this->priorities[currentprior].second["total_points"] < A.priorities[A.currentprior].second["total_points"]) {
            return false;
        }


        if (this->priorities[currentprior].second["subject_total_points"] > A.priorities[A.currentprior].second["subject_total_points"]) {
            return true;
        }
        if (this->priorities[currentprior].second["subject_total_points"] < A.priorities[A.currentprior].second["subject_total_points"]) {
            return false;
        }


        if (this->priorities[currentprior].second["subject_1_points"] > A.priorities[A.currentprior].second["subject_1_points"]) {
            return true;
        }
        if (this->priorities[currentprior].second["subject_1_points"] < A.priorities[A.currentprior].second["subject_1_points"]) {
            return false;
        }


        if (this->priorities[currentprior].second["subject_2_points"] > A.priorities[A.currentprior].second["subject_2_points"]) {
            return true;
        }
        if (this->priorities[currentprior].second["subject_2_points"] < A.priorities[A.currentprior].second["subject_2_points"]) {
            return false;
        }


        if (this->priorities[currentprior].second["subject_3_points"] > A.priorities[A.currentprior].second["subject_3_points"]) {
            return true;
        }
        if (this->priorities[currentprior].second["subject_3_points"] < A.priorities[A.currentprior].second["subject_3_points"]) {
            return false;
        }

        if (this->priorities[currentprior].second["achievement_points"] > A.priorities[A.currentprior].second["achievement_points"]) {
            return true;
        }
        if (this->priorities[currentprior].second["achievement_points"] < A.priorities[A.currentprior].second["achievement_points"]) {
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
    InsertInDirection(Entrants, EntrantId, direction, Entrants[EntrantId].getCurrPriorNapravlenie(), Outsiders, NumOfCurrPrior, full_outsiders);
    return;
}

void InsertInDirection(vector<Entrant>& Entrants, const int& EntrantId, unordered_map<int, vector<int>>& direction, const int& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior, vector<int>& full_outsiders)
{
    if (Entrants[EntrantId].getPersonNumber() == 4529) {
        cout << "Entrant with id 4529 enter in function InsertInDirection with NumofCurrentPrior - " << NumOfCurrPrior << endl;
    }
    if (direction[DirectionId].size() == 0 && direction[DirectionId].capacity() == 0)
    {
        ThrowEntrantIdFurther(Entrants, EntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
        return;
    }
    if (direction[DirectionId].size() == 0)
    {
        direction[DirectionId].push_back(EntrantId);
        return;
    }
    //cout << "Trying get iterator. line 153" << endl;
    vector<int>::iterator itr = --direction[DirectionId].end();
    //cout << "Have got iterator - " << *itr << "  line 153" << endl;
    //cout << "Direction id: " << DirectionId << endl;
    //cout << "DirectionId size " << direction[DirectionId].size() << endl;
    //cout << "Direction --itr eq direction.begin " << (itr == direction[DirectionId].begin()) << endl;
    int NextEntrantId;
    while (itr != direction[DirectionId].begin())
    {
        if (Entrants[*itr] >= Entrants[EntrantId])
        {
            if (direction[DirectionId].size() == direction[DirectionId].capacity())
            {
                if (itr == --direction[DirectionId].end())
                {
                    if (Entrants[EntrantId].getPersonNumber() == 4529) {
                        cout << "Trying to throw further 4529 at line 179" << endl;
                    }
                    ThrowEntrantIdFurther(Entrants, EntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
                    return;
                }
                NextEntrantId = *(--direction[DirectionId].end());
                direction[DirectionId].pop_back();
                //cout << "Trying to insert iterator. line 170" << endl;
                if (Entrants[EntrantId].getPersonNumber() == 4529) {
                    cout << "Inserting Entrant 4529 at line 184" << endl;
                }
                direction[DirectionId].insert(++itr, EntrantId);
                //cout << "Has insert iterator. line 170" << endl;
                if (Entrants[NextEntrantId].getPersonNumber() == 4529) {
                    cout << "Trying to throw further 4529 at line 190" << endl;
                }
                ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
                return;
            }
            //cout << "Trying to insert iterator. line 176" << endl;
            if (Entrants[EntrantId].getPersonNumber() == 4529) {
                cout << "Inserting Entrant 4529 at line 196" << endl;
            }
            direction[DirectionId].insert(++itr, EntrantId);
            //cout << "Trying to insert iterator. line 176" << endl;
            return;
        }
        else
        {
            --itr;
        }
    }
    //cout << "Comparing Entrants " << (Entrants[*itr] >= Entrants[EntrantId]) << endl;
    //Try to check here, why itr not valid
    if (Entrants[*itr] >= Entrants[EntrantId])
    {
        if (direction[DirectionId].size() == direction[DirectionId].capacity())
        {
            NextEntrantId = *(--direction[DirectionId].end());
            direction[DirectionId].pop_back();
            //cout << "Trying to insert iterator. line 193" << endl;
            if (Entrants[EntrantId].getPersonNumber() == 4529) {
                cout << "Inserting Entrant 4529 at line 217" << endl;
            }
            direction[DirectionId].insert(++itr, EntrantId);
            //cout << "Has insert iterator. line 193" << endl;
            if (Entrants[NextEntrantId].getPersonNumber() == 4529) {
                cout << "Trying to throw further 4529 at line 217" << endl;
            }
            ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
            return;
        }
        //cout << "Trying to insert iterator. line 199" << endl;
        if (Entrants[EntrantId].getPersonNumber() == 4529) {
            cout << "Inserting Entrant 4529 at line 229" << endl;
        }
        direction[DirectionId].insert(++itr, EntrantId);
        //cout << "Trying to insert iterator. line 199" << endl;
    }
    else
    {
        if (direction[DirectionId].size() == direction[DirectionId].capacity())
        {
            NextEntrantId = *(--direction[DirectionId].end());
            direction[DirectionId].pop_back();
            //Error HERE
            //cout << "Trying to insert iterator. line 208" << endl;
            //Here we find not valid iterator
            //cout << *itr << "Number in Iterator" << endl;
            if (Entrants[EntrantId].getPersonNumber() == 4529) {
                cout << "Inserting Entrant 4529 at line 247" << endl;
            }
            direction[DirectionId].insert(itr, EntrantId);
            //cout << "Iterator has inserted. line 208" << endl;
            if (Entrants[NextEntrantId].getPersonNumber() == 4529) {
                cout << "Trying to throw further 4529 at line 239" << endl;
            }
            ThrowEntrantIdFurther(Entrants, NextEntrantId, direction, Outsiders, NumOfCurrPrior, full_outsiders);
            return;
        }
        //cout << "Trying to insert iterator. line 215" << endl;
        if (Entrants[EntrantId].getPersonNumber() == 4529) {
            cout << "Inserting Entrant 4529 at line 257" << endl;
            cout << "His current prior - " << Entrants[EntrantId].getCurrPrior() << endl;
            cout << "His current napravlenie - " << Entrants[EntrantId].getCurrPriorNapravlenie() << endl;
        }
        direction[DirectionId].insert(itr, EntrantId);
        if (Entrants[EntrantId].getPersonNumber() == 4529) {//I need to output persons in that direction
            cout << "Inserting Entrant 4529 at line 257" << endl;
            cout << "His current prior - " << Entrants[EntrantId].getCurrPrior() << endl;
            cout << "His current napravlenie - " << Entrants[EntrantId].getCurrPriorNapravlenie() << endl;
        }
        //cout << "Trying to insert iterator. line 215" << endl;
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
        /*if ((*i)["count"] == 0)
        {
            cout << (*i)["id"] << "Has 0 places for students";
            exit(1);
        }*/
        count.reserve((*i)["count"]);
        if (direction.count((*i)["id"]) != 0)
        {
            cout << "2 same directions with same id - " << (*i)["id"] << ", must be one" << endl;
            exit(2);
        }
        direction[(*i)["id"]] = move(count);
    }
    maxCountOfPriorityes = 0;
    entrants.reserve(reader["entrants"].size());
    for (auto i = reader["entrants"].begin(); i != reader["entrants"].end(); ++i)
    {
        int PersonNum = (*i)["id"];

        int s = 0;
        unordered_map<string, int> f;
        vector<pair<int, unordered_map<string, int>>> Priorityes((*i)["competitions"].size(), std::make_pair(move(s), move(f)));

        if (maxCountOfPriorityes < (*i)["competitions"].size()) maxCountOfPriorityes = (*i)["competitions"].size();
        vector<int> arr;
        arr.reserve((*i)["competitions"].size());
        for (auto j = (*i)["competitions"].begin(); j != (*i)["competitions"].end(); ++j)
        {
            if (std::find(arr.begin(), arr.end(), (*j)["priority"]) != arr.end())
            {
                cout << "2 same priorities " << (*j)["priority"] << endl;
                exit(3);
            }
            arr.push_back((*j)["priority"]);
        }
        sort(arr.begin(), arr.end());
        //TODO Here i must change main logic
        for (auto j = (*i)["competitions"].begin(); j != (*i)["competitions"].end(); ++j)
        {
            int num = 0;
            for (int i = 0; i < arr.size(); ++i)
            {
                if (arr[i] == (*j)["priority"])
                    num = i;
            }
            Priorityes[num].first = (*j)["competition_id"];
            Priorityes[num].second["total_points"] = (*j)["total_points"];
            Priorityes[num].second["subject_total_points"] = (*j)["subject_total_points"];
            Priorityes[num].second["subject_1_points"] = (*j)["subject_1_points"];
            Priorityes[num].second["subject_2_points"] = (*j)["subject_2_points"];
            Priorityes[num].second["subject_3_points"] = (*j)["subject_3_points"];
            Priorityes[num].second["achievement_points"] = (*j)["achievement_points"];
            /*for (auto k = (*j)["balls"].begin(); k != (*j)["balls"].end(); ++k)
            {
                Priorityes[num].second.push_back(*k);
            }*/
        }
        if ((*i)["id"] == 4529) {
            for (auto k = Priorityes.begin(); k != Priorityes.end(); ++k) {
                cout << (*k).first << endl;
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
    //TODO Here Error
    DistributionOfEntrantsByDirections(entrants, direction, maxCountOfPriorityes, full_outsiders);

    set_to_json(direction, entrants, full_outsiders);
    
    return 0;
}
#include <iostream>
#include <vector>
#include <array>
#include <set>
using namespace std;

class Entrant
{
private:
    vector<pair<int, vector<float>>> priorities;//Первый элемент пары - цифра направления, Второй элемент пары - строка с баллами (Нулевое число - сумма баллов, первое, второе, третье и так далее - баллы по предметам, последнее - доп баллы)
    int currentprior;//Текущий приоритет, типа сейчас мы смотрим 0 приоритет, значит текущий приоритет 0, если по нашему первому приоритету мы не прошли, тогда текущий приоритет будет 1 и так далее
    int PersonNumber;//Число, характеризующее абитуриента

public:
    void initialization(const int IdentNumOfPer, const vector<pair<int, vector<float>>>& PrioritiesAndScores)
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
        for (int i = 0; i < this->priorities[currentprior].second.size(); ++i)
        {
            if (this->priorities[currentprior].second[i] > A.priorities[A.currentprior].second[i])
                return true;
            if (this->priorities[currentprior].second[i] < A.priorities[A.currentprior].second[i])
                return false;
        }
        return true;
    }
};


void ThrowEntrantIdFurther(vector<Entrant>& Entrants, const int& EntrantId, vector<vector<int>>& Directions, const int& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior)
{
    if (Entrants[EntrantId].getPrioritiesSize() - 1 == Entrants[EntrantId].getCurrPrior())
        return;
    if (Entrants[EntrantId].getCurrPrior() == NumOfCurrPrior)
    {
        Outsiders.push_back(EntrantId);
        Entrants[EntrantId].increaseCurrentPrior();
        return;
    }
    Entrants[EntrantId].increaseCurrentPrior();
    InsertInDirection(Entrants, EntrantId, Directions, Entrants[EntrantId].getCurrPriorNapravlenie(), Outsiders, NumOfCurrPrior);
    return;
}



//Рекурсивная вставка абитуриентов по направлениям
void InsertInDirection(vector<Entrant>& Entrants, const int& EntrantId, vector<vector<int>>& Directions, const int& DirectionId, vector<int>& Outsiders, const int NumOfCurrPrior)//Вставляет id в направление в порядке убывания
{
    if (Directions[DirectionId].size() == 0)
    {
        Directions[DirectionId].push_back(EntrantId);
        return;
    }
    vector<int>::iterator itr = --Directions[DirectionId].end();
    int NextEntrantId;
    while (itr >= Directions[DirectionId].begin())
    {
        if (Entrants[*itr] >= Entrants[EntrantId])
        {
            if (Directions[DirectionId].size() == Directions[DirectionId].capacity())
            {
                if (itr == --Directions[DirectionId].end())
                {
                    ThrowEntrantIdFurther(Entrants, EntrantId, Directions, DirectionId, Outsiders, NumOfCurrPrior);
                    return;
                }
                NextEntrantId = *(--Directions[DirectionId].end());
                Directions[DirectionId].pop_back();
                Directions[DirectionId].insert(++itr, EntrantId);
                ThrowEntrantIdFurther(Entrants, NextEntrantId, Directions, DirectionId, Outsiders, NumOfCurrPrior);
                return;
            }
            Directions[DirectionId].insert(++itr, EntrantId);
            return;
        }
        else
        {
            if (itr != Directions[DirectionId].begin())
                --itr;
            else
            {
                if (Directions[DirectionId].size() == Directions[DirectionId].capacity())
                {
                    NextEntrantId = *(--Directions[DirectionId].end());
                    Directions[DirectionId].pop_back();
                    Directions[DirectionId].insert(Directions[DirectionId].begin(), EntrantId);
                    ThrowEntrantIdFurther(Entrants, NextEntrantId, Directions, DirectionId, Outsiders, NumOfCurrPrior);
                    return;
                }
                Directions[DirectionId].insert(Directions[DirectionId].begin(), EntrantId);
            }
        }
    }
}

void DistributionOfEntrantsByDirections(vector<Entrant>& Entrants, vector<vector<int>>& Directions, const int& PriorityesMaxCount)// Засовываю абитуриентов в их стаканы
{// В этих цеклах я прохожу по приориетам абитуриентов, типа вначале смотрю первый приоритет всех абитуриентов, потом второй и так далее
    vector<int> Outsiders;
    vector<int> OutsidersInPrevCircle;
    for (int j = 0; j < Entrants.size(); ++j)
    {
        InsertInDirection(Entrants, j, Directions, Entrants[j].getCurrPriorNapravlenie(), Outsiders, 0);
    }
    for (int i = 1; i < PriorityesMaxCount; ++i)
    {
        OutsidersInPrevCircle = Outsiders;
        Outsiders.clear();
        if (OutsidersInPrevCircle.size() == 0)
            return;
        for (int j = 0; j < OutsidersInPrevCircle.size(); ++j)
            InsertInDirection(Entrants, OutsidersInPrevCircle[j], Directions, Entrants[OutsidersInPrevCircle[j]].getCurrPriorNapravlenie(), Outsiders, i);
    }
    return;
}

int main()// 
{
    int DirectionCount;
    cin >> DirectionCount;//Количество направлений
    vector<vector<int>> Direction;//Массив направлений(стаканов), в которых будут храниться внутренний id абитуриентов
    Direction.reserve(DirectionCount);
    for (int i = 0; i < DirectionCount; ++i)
    {
        int size;
        cin >> size;//Вводится место на каждом направлении
        Direction[i].reserve(size);
    }
    int CountOfEntrants;//Количество передаваемых абитуриентов
    cin >> CountOfEntrants;
    vector<Entrant> Entrants;//Массив абитуриентов
    Entrants.reserve(CountOfEntrants);
    int maxCountOfPriorityes = 0;//Счётчик максимального количества направлений у одного абитуриента
    for (int i = 0; i < CountOfEntrants; ++i)// Здесь происходит ввод внешнего персонального номера человека, количества его приоритетов, самих этих приоритетов, количество баллов для этих приоритетов(Для бакалавриата и специалитета будет вводиться 5 или 6, сумма предметов, сами эти предметы в приоритетном порядке, доп баллы и ещё можно в самый конец приоритетное право) и самих этих баллов
    {
        int PersonNum;
        cin >> PersonNum;
        vector<pair<int, vector<float>>> Priorityes;
        int PriorityesNum;
        cin >> PriorityesNum;// Сюда вводится количество приориететов
        if (PriorityesNum > maxCountOfPriorityes)
            maxCountOfPriorityes = PriorityesNum;
        Priorityes.reserve(PriorityesNum);
        for (int j = 0; j < PriorityesNum; ++j)
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
    }





    DistributionOfEntrantsByDirections(Entrants, Direction, maxCountOfPriorityes);





    for (int i = 0; i < DirectionCount; ++i)//Вывод распределения абитуриентов(Их персональные номера) по направлениям
    {
        cout << i <<" - Направление\n";
        for (int j = 0; j < Direction.size(); ++j)
        {
            cout << Entrants[Direction[i][j]].getPersonNumber() << endl;
        }
        cout << endl;
    }

    return;
}
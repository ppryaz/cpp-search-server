// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:271

// Закомитьте изменения и отправьте их в свой репозиторий.

#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> vec;
    int counter = 0;
    for (int i = 0; i < 1000; ++i)
    {
        vec.push_back(i);
        if (vec[i]/100 == 3 || vec[i]%100/10 == 3 || vec[i]%10 == 3)
        {counter++;
        }
        
    }
    cout << counter << endl;

    return 0;
}
#include "LRU_k.h"
#include <iostream>
using namespace std;

int main()
{
    Lru_kCache<int, int>  myCache(10, 3, 5);
    while(1)
    {
        int key;
        cout << "enter the key" << endl;
        cin >> key;
        cout << "enter the value" << endl;
        int value;
        cin >> value;
        myCache.Lru_kPolicy(key, value);
    }
}



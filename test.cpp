#include "LRU_HashCache.h"
#include <iostream>
using namespace std;

int main()
{
    LRU_HashCache<int, int>  myCache(4, 2);
    while(1)
    {
        int key;
        cout << "enter the key" << endl;
        cin >> key;
        cout << "enter the value" << endl;
        int value;
        cin >> value;
        myCache.put(key, value);
    }
}



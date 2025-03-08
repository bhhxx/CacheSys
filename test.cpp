#include "LRU.h"
#include <iostream>
using namespace std;

int main()
{
    LruCache<int, int> myCache(10);
    while(1)
    {
        int key;
        cout << "enter the key" << endl;
        cin >> key;
        myCache.LruPolicy(key);
    }
}



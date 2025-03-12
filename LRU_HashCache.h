#ifndef LRU_HASHCACHE_H_
#define LRU_HASHCACHE_H_
#include "LRU.h"
#include <vector>
#include <thread>
#include <cmath>
using namespace std;
template<class Key, class Value>
class LRU_HashCache
{
private:
    size_t capacity_;
    int sliceNum_;
    vector<unique_ptr<LruCache<Key, Value>>> lruSliceCaches_;
    int Hash(Key key)
    {
        return hash<Key>{}(key);
    }
public:
    LRU_HashCache(size_t capacity, int sliceNum)
    : capacity_(capacity), sliceNum_(sliceNum > 0 ? sliceNum : thread::hardware_concurrency())
    {
        size_t sliceSize = ceil(capacity_ / static_cast<double>(sliceNum_));
        for (int i = 0; i < sliceNum_; ++i)
        {
            lruSliceCaches_.emplace_back(new LruCache<Key, Value>(sliceSize));
        }
    }
    void put(Key key, Value value)
    {
        int index = Hash(key) % sliceNum_;
        lruSliceCaches_[index]->LruPolicy(key, value);
        printHash();
    }
    Value get(Key key)
    {
        int index = Hash(key) % sliceNum_;
        return lruSliceCaches_[index]->get(key);
    }
    void printHash()
    {
        for (int i = 0; i < sliceNum_; ++i)
        {
            lruSliceCaches_[i]->print();
        }
    }
};
#endif
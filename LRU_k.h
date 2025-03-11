#ifndef LRU_K_H_
#define LRU_K_H_
#include "LRU.h"
using namespace std;
template <class Key, class Value>
class Lru_kCache : public LruCache<Key, Value>
{
private:
    int k_;
    unique_ptr<LruCache<Key, int>> historyList_;
public:
    Lru_kCache(int capacity, int historyCapacity, int k) 
    : LruCache<Key, Value>(capacity)
    , historyList_(make_unique<LruCache<Key, int>>(historyCapacity)) // 指向LRU缓存类的指针, 用来记录访问历史
    , k_(k)
    {}
    void Lru_kPolicy(Key &key, Value &value)
    {
        if (!LruCache<Key, Value>::isFull())
        {
            LruCache<Key, Value>::LruPolicy(key, value);
        }
        if (LruCache<Key, Value>::hasKey(key))
        {
            LruCache<Key, Value>::LruPolicy(key, value);
        }

        int historyCount = historyList_->get(key);
        historyList_->LruPolicy(key, ++historyCount);
        if (historyCount >= k_)
        {
            historyList_->remove(key);
            LruCache<Key, Value>::LruPolicy(key, value);
            
        }
        cout << "Cache: " << endl;
        LruCache<Key, Value>::print();
        cout << "History: " << endl;
        historyList_->print();
    }
};
#endif /* LRU_K_H_ */
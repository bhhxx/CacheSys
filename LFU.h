#ifndef LFU_H_
#define LFU_H_
#include <memory>
#include <unordered_map>
#include <mutex>
template <class Key, class Value> class LFUCache;
namespace Cache
{
    template <typename Key, typename Value>
    class CachePolicy
    {
    public:
        virtual ~CachePolicy() {};
        // 添加缓存接口
        virtual void put(Key key, Value value) = 0; // 虚函数 需要实现
        // key是传入参数  访问到的值以传出参数的形式返回 | 访问成功返回true
        virtual bool get(Key key, Value& value) = 0;
        // 如果缓存中能找到key，则直接返回value
        virtual Value get(Key key) = 0;
    };
} // namespace Cache

template <class Key, class Value>
class FreqList // 记录一个频率的节点有哪些  // 对与每一个频率 都有一个链表
// // 有节点类 // 节点记录了自己的频率、key、value
{
private:
    struct Node
    {
        Key key;
        Value value;
        int freq;
        std::shared_ptr<Node> next;
        std::shared_ptr<Node> prev;
        Node (Key k, Value v)
        : key(k), value(v), freq(1), prev(nullptr), next(nullptr){}
    };
    using NodePtr = std::shared_ptr<Node>;
    int freq_;
    NodePtr head_;
    NodePtr tail_;

public:
    FreqList(int n): freq_(n)
    {
        head_ = std::make_shared<Node>();
        tail_ = std::make_shared<Node>();
        head_->next = tail_;
        tail_->prev = head_;
    }

    bool isEmpty() const // 判断链表是否为空
    {
        return head_->next == tail_;
    }

    void addNode(NodePtr node) // 末尾加节点
    {
        node->prev = tail_->prev;
        node->next = tail_;
        tail_->prev->next = node;
        tail_->prev = node;
    }

    void removeNode(NodePtr node) // 删除指定节点
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    NodePtr getFirstNode() const { return head_->next; } // 获得头节点
    friend class LFUCache<Key, Value>;
};

template <class Key, class Value>
class LFUCache : public Cache::CachePolicy<Key, Value>
{
public:
    using Node = class FreqList<Key, Value>::Node;
    using NodePtr = std::shared_ptr<Node>;
    using NodeMap = std::unordered_map<Key, NodePtr>;
private:
    int capacity_;
    int minFreq_;
    NodeMap NodeMap_;
    std::mutex mutex_;
    int curAverageNum_;
    int maxAverageNum_;
    int curTotalNum_;
    std::unordered_map<int, FreqList<Key, Value>*> freqToFreqList_;
public:
    LFUCache(int capacity, int maxAverageNum = 10) :capacity_(capacity), minFreq_(INT8_MAX), maxAverageNum_(maxAverageNum), curAverageNum_(0), curTotalNum_(0){}
    ~LFUCache();
    bool get(Key key, Value &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = NodeMap_.find(key);
        if (it == NodeMap_.end())
        {
            return false; // 没找到 说明要插入新的
        }
        // 找到 说明频率要增加
        getInternal(it->second, value);
        return true;
    }

    Value get(Key key) // 读取key对应的value
    {
        Value value;
        get(key, value);
        return value;
    }
    void put(Key key, Value value) // 将key-value放入缓存
    {
        if (capacity_ == 0)
        {
            return;
        }
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = NodeMap_.find(key);
        if (it != NodeMap_.end()) // 找到 更新value 增加频率
        {
            it->second->value = value;
            getInternal(it->second, value);
            return;
        }
        // 没找到 插入新的
        putInternal(key, value);
    }
    void purge() // 清空缓存
    {
        NodeMap_.clear();
        freqToFreqList_.clear();
    }
private:
    void putInternal(Key key, Value &value); // 添加缓存
    void getInternal(NodePtr node, Value& value); // 获取缓存
    void kickOut(); // 移除缓存中过期数据
    void removeFromFreqList(NodePtr node);// 从频率列表中移除节点
    void addToFreqList(NodePtr node); // 添加到频率列表
    void addFreqNum(); // 增加平均访问频率
    void decreaseFreqNum(int num); // 减少平均访问频率
    void handleOverMaxAverageNum(); // 处理当前平均访问频率超过上限的情况
    void updateMinFreq();
};
template <typename Key, typename Value>
LFUCache<Key, Value>::~LFUCache() 
{
    // 释放动态分配的资源
    for (auto& [freq, freqList] : freqToFreqList_) 
    {
        delete freqList; // 删除动态分配的 FreqList 对象
    }
    freqToFreqList_.clear();
}

template <class Key, class Value>
void LFUCache<Key, Value>::getInternal(NodePtr node, Value &value)
{
    value = node->value; // 将node的value赋值给value
    removeFromFreqList(node); // 从原有频率列表中移除node
    node->freq++; // 增加node的频率
    addToFreqList(node); // 将node添加到新的频率列表中
    if (node->freq - 1 == minFreq_ && freqToFreqList_[minFreq_]->isEmpty())
    {
        minFreq_++;
    } // 更新最小频率
    addFreqNum();
}

template <class Key, class Value>
void LFUCache<Key, Value>::putInternal(Key key, Value &value)
{   // 插入新的key-value
    if (capacity_ == NodeMap_.size())
    {
        kickOut();
    }
    NodePtr node = std::make_shared<Node>(key, value);
    NodeMap_[key] = node;
    addToFreqList(node);
    addFreqNum();
    minFreq_ = 1;
}

template <class Key, class Value>
void LFUCache<Key, Value>::kickOut()
{
    NodePtr node = freqToFreqList_[minFreq_]->getFirstNode();
    removeFromFreqList(node);
    NodeMap_.erase(node->key);
    decreaseFreqNum(node->freq);
}

template <class Key, class Value>
void LFUCache<Key, Value>::removeFromFreqList(NodePtr node)
{
    // if (!node)
    //     return;
    auto freq = node->freq; // 根据频率找到链表 删除链表头部节点
    freqToFreqList_[freq]->removeNode(node);
}

template <class Key, class Value>
void LFUCache<Key, Value>::addToFreqList(NodePtr node)
{
    // if (!node)
    //     return;
    auto freq = node->freq; // 根据频率找到链表 在链表尾部插入节点
    if (freqToFreqList_.find(freq) == freqToFreqList_.end())
    {
        freqToFreqList_[freq] = new FreqList<Key, Value>(node->freq);
    }
    freqToFreqList_[freq]->addNode(node);
}

template <class Key, class Value>
void LFUCache<Key, Value>::addFreqNum()
{
    curTotalNum_++;
    curAverageNum_ = curTotalNum_ / NodeMap_.size();
    if (curAverageNum_ > maxAverageNum_)
    {
        handleOverMaxAverageNum();
    }
}

template <class Key, class Value>
void LFUCache<Key, Value>::decreaseFreqNum(int num)
{
    curTotalNum_ -= num;
    curAverageNum_ = curTotalNum_ / NodeMap_.size();
}

template <class Key, class Value>
void LFUCache<Key, Value>::handleOverMaxAverageNum()
{   // 处理超过限定平均值的情况
    for (auto it = NodeMap_.begin(); it != NodeMap_.end(); ++it)
    {
        if (!it->second)
            continue;
        // 将存在的节点的频率减半 然后放到减半的频率链表中
        NodePtr node = it->second;
        removeFromFreqList(node);
        node->freq -= maxAverageNum_ / 2;
        if (node->freq < 1)
        {
            node->freq = 1;
        }
        addToFreqList(node);
    }
    // 更新最小频率
    updateMinFreq();
}

template<typename Key, typename Value>
void LFUCache<Key, Value>::updateMinFreq() 
{
    minFreq_ = INT8_MAX;
    curTotalNum_ = 0;
    curAverageNum_ = 0;
    for (const auto& pair : freqToFreqList_) 
    {
        if (pair.second && !pair.second->isEmpty()) 
        {
            minFreq_ = std::min(minFreq_, pair.first);
            curTotalNum_ += pair.first;
        }
    }
    curAverageNum_ = curTotalNum_ / NodeMap_.size();
    if (minFreq_ == INT8_MAX) 
        minFreq_ = 1;
}

#endif /* LFU_H_ */
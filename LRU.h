#ifndef LRU_H_
#define LRU_H_
#include <unordered_map>
#include <iostream>
#include <memory>
using namespace std;
template <class Key, class Value> class LruCache;
template<class Key, class Value>
class LruNode
{
private:
    Key key_;
    Value value_;
    shared_ptr<LruNode<Key, Value>> prev_;
    shared_ptr<LruNode<Key, Value>> next_;
public:
    LruNode(): prev_(nullptr), next_(nullptr){}
    LruNode(Key key, Value value)
        :key_(key),
         value_(value),
         prev_(nullptr),
         next_(nullptr){}
    Key getKey() const {return key_;}
    Value getValue() const {return value_;}
    // void setValue(const Value& value) {value_ = value;}
    friend class LruCache<Key, Value>;
};


template<class Key, class Value>
class LruCache
{
public:
    using LruNodeType = LruNode<Key, Value>;
    using NodePtr = shared_ptr<LruNode<Key, Value>>;
    using NodeMap = unordered_map<Key, NodePtr>;
    LruCache(int capacity)
    {
        L_ = make_shared<LruNodeType>();
        R_ = make_shared<LruNodeType>();
        L_->next_ = R_;
        R_->prev_ = L_;
        capacity_ = capacity;
        nodeCount_ = 0;
    }
    ~LruCache(){}
    // 我们对于Lru是先去根据key找，key在hash表找不到，则需要根据key去其他地方找
    void LruPolicy(Key &key, Value &value)
    {
        if (hasKey(key))
        {
            // 将该节点从链表中取出，放到末尾
            nodeMap_[key]->prev_->next_ = nodeMap_[key]->next_;
            nodeMap_[key]->next_->prev_ = nodeMap_[key]->prev_;
            nodeMap_[key]->value_ = value;
            InsertNode(nodeMap_[key]);
            nodeCount_--;
        }
        else
        {
            if (isFull())
                removeLeft();
            NodePtr temNode(new LruNodeType(key, value));
            InsertNode(temNode);
        }
    }

    // 将节点放到末尾
    void InsertNode(NodePtr temNode)
    {
        // 插入到最后一个节点
        R_->prev_->next_ = temNode;
        temNode->prev_ = R_->prev_;
        R_->prev_ = temNode;
        temNode->next_ = R_;
        nodeMap_.insert({temNode->getKey(), temNode});
        nodeCount_++;
    }
    // 去除最远的
    void removeLeft()
    {
        nodeMap_.erase(L_->next_->key_);
        L_->next_ = L_->next_->next_;
        L_->next_->prev_ = L_;
        nodeCount_--;
    }
    void remove(Key &key)
    { // 删除指定key的节点
        auto it = nodeMap_.find(key);
        nodeMap_[key]->prev_->next_ = nodeMap_[key]->next_;
        nodeMap_[key]->next_->prev_ = nodeMap_[key]->prev_;
        nodeMap_.erase(key);
        nodeCount_--;
    }
    // 寻找key // 找不到则返回false
    bool hasKey(Key &key)
    {
        auto it = nodeMap_.find(key); // 查找键为2的元素
        if (it != nodeMap_.end()) {
            return true;
        }
        else
            return false;
    }
    bool isFull() const {return nodeCount_ >= capacity_;}// 判断是不是满的
    Value get(Key &key) // 通过key获取value
    {
        Value value{};
        auto it = nodeMap_.find(key);
        if (it != nodeMap_.end())
        {
            value = it->second->value_;
        }
        return value;
    }
    void print()
    {
        NodePtr tem = L_; // 从头节点开始
        while (tem != nullptr) {       // 遍历到链表末尾
            std::cout << tem->key_ << ":" << tem->value_ << ", ";
            tem = tem->next_;          // 移动到下一个节点
        }
        cout << endl;
    }
private:
    NodePtr L_;
    NodePtr R_;
    int capacity_;
    int nodeCount_;
    NodeMap nodeMap_;
};
#endif
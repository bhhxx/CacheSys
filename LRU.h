#ifndef LRU_H_
#define LRU_H_
#include <unordered_map>
#include <iostream>
using namespace std;
template<class Key, class Value>
class LruNode
{
public:
    Key key_;
    Value value_;
    LruNode *prev_;
    LruNode *next_;
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
};


template<class Key, class Value>
class LruCache
{
public:
    LruNode<Key, Value> *L_;
    LruNode<Key, Value> *R_;
    int capacity_;
    int nodeCount_;
    std::unordered_map<Key, LruNode<Key, Value>*> nodeMap_;
public:
    LruCache(int capacity)
    {
        L_ = new LruNode<Key, Value>();
        R_ = new LruNode<Key, Value>();
        L_->next_ = R_;
        R_->prev_ = L_;
        capacity_ = capacity;
        nodeCount_ = 0;
    }
    ~LruCache()
    {
        while (L_) {
            LruNode<Key, Value>* temp = L_;
            L_ = L_->next_;
            delete temp;
        }
    }
    // 我们对于Lru是先去根据key找，key在hash表找不到，则需要根据key去其他地方找
    void LruPolicy(Key &key)
    {
        if (hasKey(key))
        {
            // 将该节点从链表中取出，放到末尾
            nodeMap_[key]->prev_->next_ = nodeMap_[key]->next_;
            nodeMap_[key]->next_->prev_ = nodeMap_[key]->prev_;
            InsertNode(nodeMap_[key]);
            nodeCount_--;
        }
        else
        {
            // 从外部读取到
            Value value = getIOValue(key);
            if (isFull())
                removeLeft();
            LruNode<Key, Value>* temNode = new LruNode<Key, Value>(key, value);
            InsertNode(temNode);
        }
        cout << "Now the cache have " << nodeCount_ << " nodes: ";
        LruNode<Key, Value>* tem = L_; // 从头节点开始
        while (tem != nullptr) {       // 遍历到链表末尾
            std::cout << tem->key_ << " "; // 打印当前节点的值
            tem = tem->next_;          // 移动到下一个节点
        }
        cout << endl;
    }
    Value getIOValue(Key &key)
    {
        Value tem;
        cout << "enter the value" << endl;
        std::cin >> tem;
        return tem;
    }

    // 将节点放到末尾
    void InsertNode(LruNode<Key, Value>* temNode)
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
        delete L_->next_->prev_;
        L_->next_->prev_ = L_;
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
};
#endif
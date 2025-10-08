#pragma once

#include "DynamicArray.h"
#include "SimpleString.h"
#include "Iterable.h"
#include "Iterator.h"

// ========== 哈希映射节点类 ==========
template<typename K, typename V>
class HashMapNode {
public:
    K key;
    V value;
    HashMapNode<K, V>* next;

    HashMapNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
};

// ========== 哈希映射迭代器类 ==========
template<typename K, typename V>
class HashMapIterator : public Iterator<HashMapNode<K, V>> {
private:
    DynamicArray<HashMapNode<K, V>*>* buckets_;
    size_t current_bucket_;
    HashMapNode<K, V>* current_node_;
    size_t elements_traversed_;
    size_t total_elements_;

    // 找到下一个非空桶
    void findNextBucket() {
        while (current_bucket_ < buckets_->size() && current_node_ == nullptr) {
            current_node_ = (*buckets_)[current_bucket_];
            current_bucket_++;
        }
    }

public:
    HashMapIterator(DynamicArray<HashMapNode<K, V>*>* buckets, size_t total_elements)
        : buckets_(buckets), current_bucket_(0), current_node_(nullptr),
          elements_traversed_(0), total_elements_(total_elements) {
        findNextBucket();
    }

    void First() override {
        current_bucket_ = 0;
        current_node_ = nullptr;
        elements_traversed_ = 0;
        findNextBucket();
    }

    void Next() override {
        if (isDone()) {
            return;
        }
        
        if (current_node_ != nullptr) {
            current_node_ = current_node_->next;
            elements_traversed_++;
            
            if (current_node_ == nullptr) {
                findNextBucket();
            }
        }
    }

    bool isDone() const override {
        return elements_traversed_ >= total_elements_;
    }

    HashMapNode<K, V> CurrentItem() const override {
        if (isDone() || current_node_ == nullptr) {
            throw std::out_of_range("HashMap iterator out of range");
        }
        return *current_node_;
    }
};

// ========== 哈希映射类 ==========
template<typename K, typename V, typename Hash = SimpleStringHash>
class HashMap : public Iterable<HashMapNode<K, V>> {
private:
    DynamicArray<HashMapNode<K, V>*> buckets_;
    size_t size_;
    size_t capacity_;
    double load_factor_;
    Hash hash_func_;

    // 哈希函数
    size_t hash(const K& key) const {
        return hash_func_(key) % capacity_;
    }

    // 重新哈希
    void rehash(size_t new_capacity) {
        DynamicArray<HashMapNode<K, V>*> old_buckets = std::move(buckets_);
        
        capacity_ = new_capacity;
        buckets_ = DynamicArray<HashMapNode<K, V>*>(capacity_, nullptr);
        size_ = 0;

        // 重新插入所有元素
        for (size_t i = 0; i < old_buckets.size(); i++) {
            HashMapNode<K, V>* node = old_buckets[i];
            while (node != nullptr) {
                HashMapNode<K, V>* next = node->next;
                putNode(node);
                node = next;
            }
        }
    }

    // 插入节点（不复制）
    void putNode(HashMapNode<K, V>* node) {
        size_t index = hash(node->key);
        
        // 插入到链表头部
        node->next = buckets_[index];
        buckets_[index] = node;
        size_++;
    }

public:
    // 构造函数
    HashMap(size_t initial_capacity = 16, double load_factor = 0.75)
        : capacity_(initial_capacity), size_(0), load_factor_(load_factor) {
        buckets_ = DynamicArray<HashMapNode<K, V>*>(capacity_, nullptr);
    }

    // 拷贝构造函数
    HashMap(HashMap& other)
        : capacity_(other.capacity_), size_(0),
          load_factor_(other.load_factor_), hash_func_(other.hash_func_) {
        buckets_ = DynamicArray<HashMapNode<K, V>*>(capacity_, nullptr);
        
        // 深拷贝所有节点
        Iterator<HashMapNode<K, V>>* it = other.createIterator();
        for (it->First(); !it->isDone(); it->Next()) {
            HashMapNode<K, V> node = it->CurrentItem();
            put(node.key, node.value);
        }
        delete it;
    }

    // 移动构造函数
    HashMap(HashMap&& other) noexcept
        : buckets_(std::move(other.buckets_)), capacity_(other.capacity_),
          size_(other.size_), load_factor_(other.load_factor_),
          hash_func_(std::move(other.hash_func_)) {
        other.capacity_ = 0;
        other.size_ = 0;
    }

    // 析构函数
    ~HashMap() {
        clear();
    }

    // 拷贝赋值
    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            capacity_ = other.capacity_;
            load_factor_ = other.load_factor_;
            hash_func_ = other.hash_func_;
            buckets_ = DynamicArray<HashMapNode<K, V>*>(capacity_, nullptr);
            
            // 深拷贝所有节点
            Iterator<HashMapNode<K, V>>* it = other.createIterator();
            for (it->First(); !it->isDone(); it->Next()) {
                HashMapNode<K, V> node = it->CurrentItem();
                put(node.key, node.value);
            }
            delete it;
        }
        return *this;
    }

    // 移动赋值
    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) {
            clear();
            buckets_ = std::move(other.buckets_);
            capacity_ = other.capacity_;
            size_ = other.size_;
            load_factor_ = other.load_factor_;
            hash_func_ = std::move(other.hash_func_);
            
            other.capacity_ = 0;
            other.size_ = 0;
        }
        return *this;
    }

    // ========== 基本操作 ==========

    // 插入键值对
    void put(const K& key, const V& value) {
        // 检查是否需要扩容
        if (static_cast<double>(size_ + 1) / capacity_ > load_factor_) {
            rehash(capacity_ * 2);
        }

        size_t index = hash(key);
        
        // 检查键是否已存在
        HashMapNode<K, V>* node = buckets_[index];
        while (node != nullptr) {
            if (node->key == key) {
                // 键已存在，更新值
                node->value = value;
                return;
            }
            node = node->next;
        }

        // 插入新节点到链表头部
        HashMapNode<K, V>* newNode = new HashMapNode<K, V>(key, value);
        newNode->next = buckets_[index];
        buckets_[index] = newNode;
        size_++;
    }

    // 获取值
    V get(const K& key) const {
        size_t index = hash(key);
        HashMapNode<K, V>* node = buckets_[index];
        
        while (node != nullptr) {
            if (node->key == key) {
                return node->value;
            }
            node = node->next;
        }
        
        throw SimpleException("Key not found");
    }

    // 检查键是否存在
    bool contains(const K& key) const {
        size_t index = hash(key);
        HashMapNode<K, V>* node = buckets_[index];
        
        while (node != nullptr) {
            if (node->key == key) {
                return true;
            }
            node = node->next;
        }
        
        return false;
    }

    // 删除键值对
    void remove(const K& key) {
        size_t index = hash(key);
        HashMapNode<K, V>* node = buckets_[index];
        HashMapNode<K, V>* prev = nullptr;
        
        while (node != nullptr) {
            if (node->key == key) {
                if (prev == nullptr) {
                    // 删除链表头节点
                    buckets_[index] = node->next;
                } else {
                    prev->next = node->next;
                }
                
                delete node;
                size_--;
                return;
            }
            
            prev = node;
            node = node->next;
        }
    }

    // ========== 容量操作 ==========
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    void clear() {
        for (size_t i = 0; i < capacity_; i++) {
            HashMapNode<K, V>* node = buckets_[i];
            while (node != nullptr) {
                HashMapNode<K, V>* next = node->next;
                delete node;
                node = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }

    // ========== 迭代器方法 ==========
    Iterator<HashMapNode<K, V>>* createIterator(){
        return new HashMapIterator<K, V>(&buckets_, size_);
    }

    // ========== 工具函数 ==========
    void swap(HashMap& other) noexcept {
        buckets_.swap(other.buckets_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(load_factor_, other.load_factor_);
        std::swap(hash_func_, other.hash_func_);
    }
};

// 全局 swap 函数
template<typename K, typename V, typename Hash>
void swap(HashMap<K, V, Hash>& lhs, HashMap<K, V, Hash>& rhs) noexcept {
    lhs.swap(rhs);
}
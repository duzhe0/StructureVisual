#include <iostream>
#include "HashMap.h"
#include "SimpleString.cpp"
// Integer hash function
struct IntHash {
    size_t operator()(int key) const {
        return static_cast<size_t>(key);
    }
};

// Test function declarations
void testBasicOperations();
void testIterator();
void testStringKeys();
void testEdgeCases();

int main() {
    std::cout << "=== HashMap Test Started ===" << std::endl;
    
    try {
        testBasicOperations();
        testIterator();
        testStringKeys();
        testEdgeCases();
        
        std::cout << "=== All Tests Passed ===" << std::endl;
    } catch (const SimpleException& e) {
        std::cout << "Test Failed: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Standard Exception: " << e.what() << std::endl;
    }
    
    return 0;
}

void testBasicOperations() {
    std::cout << "\n--- Testing Basic Operations ---" << std::endl;
    
    HashMap<int, SimpleString, IntHash> map;
    
    // Test insertion
    map.put(1, "One");
    map.put(2, "Two");
    map.put(3, "Three");
    map.put(13, "Thirteen"); // Test collision handling
    
    std::cout << "Size after inserting 4 elements: " << map.size() << std::endl;
    
    // Test retrieval
    if (map.get(1) == "One" && map.get(2) == "Two" && 
        map.get(3) == "Three" && map.get(13) == "Thirteen") {
        std::cout << "Retrieval operations test passed" << std::endl;
    } else {
        throw SimpleException("Retrieval operations test failed");
    }
    
    // Test existence check
    if (map.contains(1) && map.contains(2) && map.contains(13) && !map.contains(4)) {
        std::cout << "Existence check test passed" << std::endl;
    } else {
        throw SimpleException("Existence check test failed");
    }
    
    // Test update
    map.put(2, "TwoUpdated");
    if (map.get(2) == "TwoUpdated") {
        std::cout << "Update operation test passed" << std::endl;
    } else {
        throw SimpleException("Update operation test failed");
    }
    
    // Test deletion
    map.remove(1);
    if (!map.contains(1) && map.size() == 3) {
        std::cout << "Delete operation test passed" << std::endl;
    } else {
        throw SimpleException("Delete operation test failed");
    }
    
    // Test deletion from collision chain
    map.remove(13);
    if (!map.contains(13) && map.size() == 2) {
        std::cout << "Delete from collision chain test passed" << std::endl;
    } else {
        throw SimpleException("Delete from collision chain test failed");
    }
}

void testIterator() {
    std::cout << "\n--- Testing Iterator ---" << std::endl;
    
    HashMap<int, SimpleString, IntHash> map;
    
    // Insert test data
    map.put(10, "Ten");
    map.put(20, "Twenty");
    map.put(30, "Thirty");
    map.put(26, "TwentySix"); // Should go to same bucket as 10 in some cases
    
    // Use iterator to traverse
    Iterator<HashMapNode<int, SimpleString>>* it = map.createIterator();
    int count = 0;
    
    std::cout << "Iterator traversal results:" << std::endl;
    for (it->First(); !it->isDone(); it->Next()) {
        HashMapNode<int, SimpleString> node = it->CurrentItem();
        std::cout << "Key: " << node.key << ", Value: " << node.value << std::endl;
        count++;
    }
    
    if (count == 4) {
        std::cout << "Iterator traversal test passed, found " << count << " elements" << std::endl;
    } else {
        throw SimpleException("Iterator traversal test failed");
    }
    
    delete it;
    
    // Test iterator for empty map
    HashMap<int, SimpleString, IntHash> emptyMap;
    Iterator<HashMapNode<int, SimpleString>>* emptyIt = emptyMap.createIterator();
    emptyIt->First();
    if (emptyIt->isDone()) {
        std::cout << "Empty map iterator test passed" << std::endl;
    } else {
        throw SimpleException("Empty map iterator test failed");
    }
    delete emptyIt;
}

void testStringKeys() {
    std::cout << "\n--- Testing String Keys ---" << std::endl;
    
    HashMap<SimpleString, int> map;
    
    // Insert string key-value pairs
    map.put(SimpleString("apple"), 100);
    map.put(SimpleString("banana"), 200);
    map.put(SimpleString("cherry"), 300);
    
    std::cout << "String key insertion test passed, size: " << map.size() << std::endl;
    
    // Test retrieval
    if (map.get(SimpleString("apple")) == 100 && 
        map.get(SimpleString("banana")) == 200) {
        std::cout << "String key retrieval test passed" << std::endl;
    } else {
        throw SimpleException("String key retrieval test failed");
    }
    
    // Test iterator
    Iterator<HashMapNode<SimpleString, int>>* it = map.createIterator();
    int count = 0;
    
    std::cout << "String key iterator traversal:" << std::endl;
    for (it->First(); !it->isDone(); it->Next()) {
        HashMapNode<SimpleString, int> node = it->CurrentItem();
        std::cout << "Key: " << node.key.c_str() << ", Value: " << node.value << std::endl;
        count++;
    }
    
    if (count == 3) {
        std::cout << "String key iterator test passed" << std::endl;
    } else {
        throw SimpleException("String key iterator test failed");
    }
    
    delete it;
}

void testEdgeCases() {
    std::cout << "\n--- Testing Edge Cases ---" << std::endl;
    
    HashMap<int, SimpleString, IntHash> map;
    
    // Test empty map
    if (map.empty() && map.size() == 0) {
        std::cout << "Empty map test passed" << std::endl;
    } else {
        throw SimpleException("Empty map test failed");
    }
    
    // Test clear operation
    map.put(1, "One");
    map.put(2, "Two");
    map.clear();
    
    if (map.empty() && map.size() == 0 && !map.contains(1)) {
        std::cout << "Clear operation test passed" << std::endl;
    } else {
        throw SimpleException("Clear operation test failed");
    }
    
    // Test capacity and resizing
    HashMap<int, SimpleString, IntHash> smallMap(4, 0.5); // Small capacity, low load factor
    
    for (int i = 0; i < 10; i++) {
        smallMap.put(i, SimpleString("Value") + SimpleString(i));
    }
    
    std::cout << "Size after resizing: " << smallMap.size() << ", Capacity: " << smallMap.capacity() << std::endl;
    if (smallMap.size() == 10 && smallMap.capacity() >= 10) {
        std::cout << "Auto-resizing test passed" << std::endl;
    } else {
        throw SimpleException("Auto-resizing test failed");
    }
    
    // Test copy constructor
    HashMap<int, SimpleString, IntHash> original;
    original.put(1, "One");
    original.put(2, "Two");
    
    HashMap<int, SimpleString, IntHash> copy = original;
    
    if (copy.size() == original.size() && copy.get(1) == "One" && copy.get(2) == "Two") {
        std::cout << "Copy constructor test passed" << std::endl;
    } else {
        throw SimpleException("Copy constructor test failed");
    }
    
    // Test that they are independent
    original.put(3, "Three");
    if (!copy.contains(3)) {
        std::cout << "Copy independence test passed" << std::endl;
    } else {
        throw SimpleException("Copy independence test failed");
    }
    
    // Test move constructor
    HashMap<int, SimpleString, IntHash> moved = std::move(original);
    
    if (moved.size() == 3 && original.size() == 0) {
        std::cout << "Move constructor test passed" << std::endl;
    } else {
        throw SimpleException("Move constructor test failed");
    }
    
    // Test collision handling
    HashMap<int, SimpleString, IntHash> collisionMap(5); // Small capacity to force collisions
    collisionMap.put(1, "One");
    collisionMap.put(6, "Six"); // Should collide with 1 in a table of size 5
    collisionMap.put(11, "Eleven"); // Should also collide
    
    if (collisionMap.get(1) == "One" && collisionMap.get(6) == "Six" && collisionMap.get(11) == "Eleven") {
        std::cout << "Collision handling test passed" << std::endl;
    } else {
        throw SimpleException("Collision handling test failed");
    }
}
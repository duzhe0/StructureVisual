#include "simplestring.h"
#include <iostream>
#include <cassert>
using namespace std;

void test_constructors_string() {
    std::cout << "Testing constructors..." << std::endl;

    // Default constructor
    SimpleString s1;
    assert(s1.empty());
    assert(s1.length() == 0);
    assert(s1.c_str()[0] == '\0');

    // C-string constructor
    SimpleString s2("hello");
    assert(s2.length() == 5);
    assert(s2 == "hello");

    // Copy constructor
    SimpleString s3 = s2;
    assert(s3 == "hello");
    assert(s3 == s2);

    // Move constructor
    SimpleString s4 = std::move(SimpleString("world"));
    assert(s4 == "world");

    std::cout << "Constructor tests passed!" << std::endl;
}

void test_assignment() {
    std::cout << "Testing assignment operations..." << std::endl;

    SimpleString s1("original");
    SimpleString s2;

    // Copy assignment
    s2 = s1;
    assert(s2 == "original");

    // Self-assignment
    s2 = s2;
    assert(s2 == "original");

    // C-string assignment
    s2 = "new value";
    assert(s2 == "new value");

    // Move assignment
    s2 = SimpleString("moved");
    assert(s2 == "moved");

    std::cout << "Assignment tests passed!" << std::endl;
}

void test_comparison() {
    std::cout << "Testing comparison operations..." << std::endl;

    SimpleString s1("apple");
    SimpleString s2("apple");
    SimpleString s3("banana");

    assert(s1 == s2);
    assert(s1 != s3);
    assert(s1 < s3);
    assert(s3 > s1);
    assert(s1 <= s2);

    std::cout << "Comparison tests passed!" << std::endl;
}

void test_modification() {
    std::cout << "Testing modification operations..." << std::endl;

    SimpleString s1("hello");

    // Test append
    s1.append(" world");
    assert(s1 == "hello world");

    // Test clear
    s1.clear();
    assert(s1.empty());
    assert(s1.length() == 0);

    // Test reassignment
    s1 = "new string";
    assert(s1 == "new string");

    std::cout << "Modification tests passed!" << std::endl;
}

void test_hash() {
    std::cout << "Testing hash functions..." << std::endl;

    SimpleString s1("hello");
    SimpleString s2("hello");
    SimpleString s3("world");

    // Same strings should have same hash values
    assert(s1.hash() == s2.hash());

    // Different strings should have different hash values
    // Note: Hash collisions are theoretically possible but unlikely
    assert(s1.hash() != s3.hash());

    // Hash function object test
    SimpleStringHash hasher;
    assert(hasher(s1) == s1.hash());

    std::cout << "Hash function tests passed!" << std::endl;
}

void test_edge_cases() {
    std::cout << "Testing edge cases..." << std::endl;

    // Empty string
    SimpleString s1("");
    assert(s1.empty());
    assert(s1.length() == 0);

    // nullptr construction
    SimpleString s2(nullptr);
    assert(s2.empty());

    // Long string
    SimpleString s3("this is a very long string that might require reallocation");
    assert(s3.length() > 0);

    // Self-append
    SimpleString s4("test");
    s4.append(s4.c_str());  // Self-append
    cout << s4 << endl;
    assert(s4 == "testtest");

    std::cout << "Edge case tests passed!" << std::endl;
}

// int main() {
//     std::cout << "Starting SimpleString tests..." << std::endl;

//     test_constructors_string();
//     test_assignment();
//     test_comparison();
//     test_modification();
//     test_hash();
//     test_edge_cases();

//     std::cout << "All SimpleString tests passed!" << std::endl;
//     return 0;
// }

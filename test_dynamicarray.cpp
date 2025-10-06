#include "dynamicarray.h"
#include <iostream>
#include <cassert>

// Test class for verifying copy, move semantics, etc.
class TestItem {
public:
    int value;
    static int constructor_count;
    static int destructor_count;
    static int copy_count;
    static int move_count;

    TestItem() : value(0) {
        constructor_count++;
    }

    explicit TestItem(int v) : value(v) {
        constructor_count++;
    }

    TestItem(const TestItem& other) : value(other.value) {
        copy_count++;
        constructor_count++;
    }

    TestItem(TestItem&& other) noexcept : value(other.value) {
        other.value = -1; // Mark as moved
        move_count++;
        constructor_count++;
    }

    TestItem& operator=(const TestItem& other) {
        if (this != &other) {
            value = other.value;
            copy_count++;
        }
        return *this;
    }

    TestItem& operator=(TestItem&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = -1; // Mark as moved
            move_count++;
        }
        return *this;
    }

    ~TestItem() {
        destructor_count++;
    }

    static void reset_counters() {
        constructor_count = 0;
        destructor_count = 0;
        copy_count = 0;
        move_count = 0;
    }
};

// Static member initialization
int TestItem::constructor_count = 0;
int TestItem::destructor_count = 0;
int TestItem::copy_count = 0;
int TestItem::move_count = 0;

// Test function declarations
void test_constructors_array();
void test_accessors();
void test_capacity();
void test_modifiers();
void test_copy_move_semantics();
void test_iterators();
void test_exceptions();
void test_complex_type();

// int main() {
//     std::cout << "Starting DynamicArray tests...\n" << std::endl;

//     try {
//         test_constructors_array();
//         test_accessors();
//         test_capacity();
//         test_modifiers();
//         test_copy_move_semantics();
//         test_iterators();
//         test_exceptions();
//         test_complex_type();

//         std::cout << "\nAll tests passed!" << std::endl;
//     } catch (const DynamicArrayException& e) {
//         std::cerr << "Test failed with exception: " << e.what() << std::endl;
//         return 1;
//     } catch (const std::exception& e) {
//         std::cerr << "Test failed with standard exception: " << e.what() << std::endl;
//         return 1;
//     } catch (...) {
//         std::cerr << "Test failed with unknown exception" << std::endl;
//         return 1;
//     }

//     return 0;
// }

void test_constructors_array() {
    std::cout << "=== Testing Constructors ===" << std::endl;

    // Test default constructor
    DynamicArray<int> arr1;
    assert(arr1.size() == 0);
    assert(arr1.capacity() >= 1);
    assert(arr1.empty());

    // Test constructor with initial capacity
    DynamicArray<int> arr2(32);
    assert(arr2.size() == 0);
    assert(arr2.capacity() == 32);

    // Test constructor with initial size and value
    DynamicArray<int> arr3(5, 42);
    assert(arr3.size() == 5);
    assert(arr3.capacity() == 5);
    for (size_t i = 0; i < arr3.size(); ++i) {
        assert(arr3[i] == 42);
    }

    // Test zero-size initialization
    DynamicArray<int> arr4(0, 100);
    assert(arr4.size() == 0);
    assert(arr4.capacity() == 1); // Minimum capacity is 1

    std::cout << "Constructor tests passed" << std::endl;
}

void test_accessors() {
    std::cout << "\n=== Testing Accessors ===" << std::endl;

    DynamicArray<int> arr;
    for (int i = 0; i < 5; ++i) {
        arr.push_back(i * 10);
    }

    // Test operator[]
    assert(arr[0] == 0);
    assert(arr[2] == 20);
    assert(arr[4] == 40);

    // Test at()
    assert(arr.at(1) == 10);
    assert(arr.at(3) == 30);

    // Test front() and back()
    assert(arr.front() == 0);
    assert(arr.back() == 40);

    // Test data()
    int* data_ptr = arr.data();
    assert(data_ptr[0] == 0);
    assert(data_ptr[4] == 40);

    // Test const versions
    const DynamicArray<int>& const_arr = arr;
    assert(const_arr[0] == 0);
    assert(const_arr.at(1) == 10);
    assert(const_arr.front() == 0);
    assert(const_arr.back() == 40);
    assert(const_arr.data()[2] == 20);

    std::cout << "Accessor tests passed" << std::endl;
}

void test_capacity() {
    std::cout << "\n=== Testing Capacity Operations ===" << std::endl;

    DynamicArray<int> arr;

    // Initial state
    assert(arr.empty());
    assert(arr.size() == 0);

    // Add elements
    arr.push_back(1);
    arr.push_back(2);
    assert(!arr.empty());
    assert(arr.size() == 2);

    // Test reserve
    size_t old_capacity = arr.capacity();
    arr.reserve(old_capacity + 10);
    assert(arr.capacity() >= old_capacity + 10);
    assert(arr.size() == 2); // Size should not change

    // Test shrink_to_fit
    arr.resize(5);
    size_t capacity_before_shrink = arr.capacity();
    arr.shrink_to_fit();
    assert(arr.capacity() == arr.size()); // Capacity should equal size
    assert(arr.capacity() <= capacity_before_shrink);

    std::cout << "Capacity operations tests passed" << std::endl;
}

void test_modifiers() {
    std::cout << "\n=== Testing Modifiers ===" << std::endl;

    DynamicArray<int> arr;

    // Test push_back
    for (int i = 0; i < 10; ++i) {
        arr.push_back(i);
        assert(arr.size() == static_cast<size_t>(i + 1));
        assert(arr.back() == i);
    }

    // Test pop_back
    for (int i = 9; i >= 0; --i) {
        assert(arr.back() == i);
        arr.pop_back();
        assert(arr.size() == static_cast<size_t>(i));
    }
    assert(arr.empty());

    // Test clear
    for (int i = 0; i < 5; ++i) {
        arr.push_back(i);
    }
    assert(arr.size() == 5);
    arr.clear();
    assert(arr.empty());
    assert(arr.size() == 0);

    // Test resize (enlarge)
    arr.resize(5, 99);
    assert(arr.size() == 5);
    for (size_t i = 0; i < arr.size(); ++i) {
        assert(arr[i] == 99);
    }

    // Test resize (shrink)
    arr.resize(2);
    assert(arr.size() == 2);
    assert(arr[0] == 99);
    assert(arr[1] == 99);

    std::cout << "Modifier tests passed" << std::endl;
}

void test_copy_move_semantics() {
    std::cout << "\n=== Testing Copy and Move Semantics ===" << std::endl;

    // Test copy constructor
    DynamicArray<int> original;
    for (int i = 0; i < 5; ++i) {
        original.push_back(i * 2);
    }

    DynamicArray<int> copy_constructed(original);
    assert(copy_constructed.size() == original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        assert(copy_constructed[i] == original[i]);
    }

    // Test copy assignment operator
    DynamicArray<int> copy_assigned;
    copy_assigned = original;
    assert(copy_assigned.size() == original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        assert(copy_assigned[i] == original[i]);
    }

    // Test move constructor
    DynamicArray<int> to_move;
    for (int i = 0; i < 3; ++i) {
        to_move.push_back(i + 10);
    }
    size_t old_size = to_move.size();
    size_t old_capacity = to_move.capacity();

    DynamicArray<int> move_constructed(std::move(to_move));
    assert(move_constructed.size() == old_size);
    assert(move_constructed.capacity() == old_capacity);
    assert(to_move.size() == 0); // Moved object should be empty
    assert(to_move.capacity() == 0);

    // Test move assignment operator
    DynamicArray<int> to_move2;
    for (int i = 0; i < 4; ++i) {
        to_move2.push_back(i + 20);
    }
    old_size = to_move2.size();
    old_capacity = to_move2.capacity();

    DynamicArray<int> move_assigned;
    move_assigned = std::move(to_move2);
    assert(move_assigned.size() == old_size);
    assert(move_assigned.capacity() == old_capacity);
    assert(to_move2.size() == 0);
    assert(to_move2.capacity() == 0);

    // Test self-assignment
    DynamicArray<int> self_assign;
    self_assign.push_back(1);
    self_assign.push_back(2);
    self_assign = self_assign; // Should be safe
    assert(self_assign.size() == 2);
    assert(self_assign[0] == 1);
    assert(self_assign[1] == 2);

    std::cout << "Copy and move semantics tests passed" << std::endl;
}

void test_iterators() {
    std::cout << "\n=== Testing Iterators ===" << std::endl;

    DynamicArray<int> arr;
    for (int i = 0; i < 5; ++i) {
        arr.push_back(i * 10);
    }

    // Test non-const iterators
    int expected = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        assert(*it == expected);
        expected += 10;
    }

    // Test const iterators
    expected = 0;
    for (auto it = arr.cbegin(); it != arr.cend(); ++it) {
        assert(*it == expected);
        expected += 10;
    }

    // Test range-based for loop
    expected = 0;
    for (int value : arr) {
        assert(value == expected);
        expected += 10;
    }

    // Test modifying elements
    for (auto& value : arr) {
        value += 1;
    }
    assert(arr[0] == 1);
    assert(arr[4] == 41);

    std::cout << "Iterator tests passed" << std::endl;
}

void test_exceptions() {
    std::cout << "\n=== Testing Exceptions ===" << std::endl;

    DynamicArray<int> arr;

    // Test front() and back() on empty array
    try {
        arr.front();
        assert(false); // Should throw exception
    } catch (const DynamicArrayException& e) {
        assert(std::string(e.what()) == "Array is empty");
    }

    try {
        arr.back();
        assert(false); // Should throw exception
    } catch (const DynamicArrayException& e) {
        assert(std::string(e.what()) == "Array is empty");
    }

    // Add some elements
    arr.push_back(1);
    arr.push_back(2);

    // Test out-of-bounds access
    try {
        arr.at(5);
        assert(false); // Should throw exception
    } catch (const DynamicArrayException& e) {
        assert(std::string(e.what()) == "Index out of bounds");
    }

    try {
        arr[5];
        assert(false); // Should throw exception
    } catch (const DynamicArrayException& e) {
        assert(std::string(e.what()) == "Index out of bounds");
    }

    // Test reallocate exception
    DynamicArray<int> arr2(3);
    arr2.push_back(1);
    arr2.push_back(2);
    arr2.push_back(3);

    try {
        arr2.reserve(2); // New capacity less than current size
        assert(false); // Should throw exception
    } catch (const DynamicArrayException& e) {
        assert(std::string(e.what()) == "New capacity cannot be less than current size");
    }

    std::cout << "Exception tests passed" << std::endl;
}

void test_complex_type() {
    std::cout << "\n=== Testing Complex Type ===" << std::endl;

    TestItem::reset_counters();

    {
        DynamicArray<TestItem> arr;

        // Test push_back with copy semantics
        TestItem item1(100);
        arr.push_back(item1);
        assert(TestItem::copy_count >= 1);

        // Test push_back with move semantics
        arr.push_back(TestItem(200));
        assert(TestItem::move_count >= 1);

        // Test resize
        arr.resize(5, TestItem(300));

        // Test copy construction
        DynamicArray<TestItem> arr2 = arr;
        assert(arr2.size() == arr.size());

        // Test move construction
        DynamicArray<TestItem> arr3 = std::move(arr);
        assert(arr3.size() == 5);
        assert(arr.size() == 0); // Should be empty after move
    }

    // Check all objects were properly destructed
    assert(TestItem::constructor_count == TestItem::destructor_count);

    std::cout << "Complex type tests passed" << std::endl;
    std::cout << "Constructor count: " << TestItem::constructor_count << std::endl;
    std::cout << "Destructor count: " << TestItem::destructor_count << std::endl;
    std::cout << "Copy count: " << TestItem::copy_count << std::endl;
    std::cout << "Move count: " << TestItem::move_count << std::endl;
}

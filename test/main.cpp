#include <iostream>
#include <vector.hpp>
#include <array.hpp>
#include <memory.hpp>
#include <list.hpp>
#include <deque.hpp>
using namespace std;
int main() {
    // tiny_stl::array<T, n> Tests
    tiny_stl::array<int, 10> arr = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }
    cout << "Array size: " << arr.size() << endl;
    cout << "Array Elements:" << endl;
    for (auto i : arr) {
        cout << i << " ";
    }
    cout << endl;
    // tiny_stl::vector<T, Alloc> Tests
    tiny_stl::vector<int> vec(arr.begin(), arr.end());
    cout << "Vector capacity: " << vec.capacity() << endl;
    cout << "Vector size: " << vec.size() << endl;
    cout << "Vector Elements:" << endl;
    for (auto i : vec) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Adding an element at back..." << endl;
    vec.push_back(10);
    cout << "Vector capacity: " << vec.capacity() << endl;
    cout << "Vector size: " << vec.size() << endl;
    cout << "Vector elements:" << endl;
    for (auto i : vec) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Adding an element at front..." << endl;
    vec.insert(vec.begin(), -1);
    cout << "Vector capacity: " << vec.capacity() << endl;
    cout << "Vector size: " << vec.size() << endl;
    cout << "Vector elements:" << endl;
    for (auto i : vec) {
        cout << i << " ";
    }
    cout << endl;
    // tiny_stl::list<T, Alloc> Tests
    tiny_stl::list<int> list(arr.begin(), arr.end());
    cout << "List size: " << list.size() << endl;
    for (auto it = list.begin(); it != list.end();) {
        cout << *it;
        if (++it != list.end()) {
            cout << " --> ";
        }
    }
    cout << endl;
    // tiny_stl::deque<T, Alloc, buffer> Tests
    tiny_stl::deque<int> deq(arr.begin(), arr.end());
    cout << "Deque size: " << deq.size() << endl;
    cout << "Deque Elements:" << endl;
    for (auto i : deq) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Adding an element at back..." << endl;
    deq.push_back(10);
    cout << "Deque size: " << deq.size() << endl;
    cout << "Deque elements:" << endl;
    for (auto i : deq) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Adding an element at front..." << endl;
    deq.push_front(-1);
    cout << "Deque size: " << deq.size() << endl;
    cout << "Deque elements:" << endl;
    for (auto i : deq) {
        cout << i << " ";
    }
    cout << endl;
    // tiny_stl::unique_ptr<T> Tests
    tiny_stl::unique_ptr<int> uptr1 = tiny_stl::make_unique<int>(arr[5]);
    cout << "Unique pointer 1 value: " << *uptr1 << endl;
    tiny_stl::unique_ptr<int> uptr2 = tiny_stl::move(uptr1);
    cout << "Unique pointer 2 value: " << *uptr2 << endl;
    if (uptr1) {
        cout << "Unique pointer 1 is not released" << endl;
    } else {
        cout << "Unique pointer 1 is released" << endl;
    }
    // tiny_stl::shared_ptr<T> Tests
    tiny_stl::shared_ptr<int> sptr1 = tiny_stl::make_shared<int>(arr[6]);
    cout << "Shared pointer 1 value: " << *sptr1 << endl;
    cout << "Shared pointer 1 count: " << sptr1.use_count() << endl;
    tiny_stl::shared_ptr<int> sptr2 = sptr1;
    cout << "Shared pointer 2 value: " << *sptr2 << endl;
    cout << "Shared pointer 2 count: " << sptr2.use_count() << endl;
    return 0;
}

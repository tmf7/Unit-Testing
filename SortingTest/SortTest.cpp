#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <time.h>		// time()
#include <chrono>

#include <string>
#include <memory>		// std::unique_ptr<type>
#include "Sort.h"

typedef struct test_s {
	std::string name;
	int buffer1 = 5555;
	int buffer2 = 6666;
	int buffer3 = 7777;
	int buffer4 = 88888;
	int buffer5 = 9999;
	long long buffer6 = 0xfffffffff;
	double buffer7 = 543.3423404;
	int priority;
	test_s() : name("DEFAULT"), priority(-1) {};
	test_s(int priority) : name("TEST"), priority(priority) {};
/*
	test_s(const test_s & other) : name(other.name), priority(other.priority) {};
	test_s(test_s && other) : name(std::move(other.name)), priority(other.priority) {};
	test_s & operator=(test_s && other) noexcept { 
		std::swap(name, other.name); 
		std::swap(priority, other.priority); 
		return *this; 
	};
	test_s & operator=(const test_s & other) { 
		name = other.name;
		priority = other.priority;
		return *this; 
	};*/
} test_t;

#define TEST test_t // std::unique_ptr<int> // int
#define NUM_ELEMENTS 65536
TEST ary[NUM_ELEMENTS];
TEST ary2[NUM_ELEMENTS];

//////////////////////////////////////////////////////////////////////
// BEGIN http://coliru.stacked-crooked.com/a/63da145bb26167ee test
#include <algorithm>    // min_element, iter_swap, 
						// upper_bound, rotate, 
						// nth_element, partition, 
						// inplace_merge,
						// make_heap, sort_heap, push_heap, pop_heap,
						// is_heap, is_sorted
#include <cassert>      // assert
#include <functional>   // less
#include <iterator>     // distance, next

#include <iostream>     // cout
#include <ios>          // boolalpha
#include <vector>       // vector

template<class BiDirIt, class Compare = std::less<>>
void merge_sort(BiDirIt first, BiDirIt last, Compare cmp = Compare{})
{
	auto const N = std::distance(first, last);
	if (N <= 1) return;
	auto const middle = std::next(first, N / 2);
	merge_sort(first, middle, cmp); // assert(std::is_sorted(first, middle, cmp));
	merge_sort(middle, last, cmp);  // assert(std::is_sorted(middle, last, cmp));
	std::inplace_merge(first, middle, last, cmp); // assert(std::is_sorted(first, last, cmp));
}

// END http://coliru.stacked-crooked.com/a/63da145bb26167ee test
//////////////////////////////////////////////////////////////////////

// unit test: test passed using int, std::unique_ptr<int>, and std::string (in a struct sorted by priority)
// Quicksort with the insertion-sort optimization is about equal to the seletion-sort optimization
// My bottom-up natural mergesort is about even with the top-down mergesort using std::inplace_merge
// NOTE: side-by-side timing was biased based on execution order, tested individually and averaged run times
int main() {

	srand(time(NULL));

//	auto random = std::vector<TEST>{};
//	random.reserve(NUM_ELEMENTS);
	for (int i = 0; i < NUM_ELEMENTS; i++)
		ary[i] = TEST(rand() % NUM_ELEMENTS);
//		random.push_back(std::move(TEST(rand() % NUM_ELEMENTS)));

	// side-by-side sort test
	for (int i = 0; i < NUM_ELEMENTS; i++)
		ary2[i] = ary[i];


	// side-by-side quicksort optimization test (insertion sort optimization wins 5/6 by 5+ ms for large objects)
	auto t1 = std::chrono::high_resolution_clock::now();
	//	QuickSort_Insert(ary2, NUM_ELEMENTS, [](TEST & a, TEST & b) { if (a.priority < b.priority) return -1; else if (a.priority > b.priority) return 1; return 0; });
	MergeSort(ary2, NUM_ELEMENTS, [](TEST & a, TEST & b) { return a.priority <= b.priority; });
	auto t2 = std::chrono::high_resolution_clock::now();
	printf("Sorted-MY-Merge: %s\n", std::is_sorted(std::begin(ary2), std::end(ary2), [](TEST & a, TEST & b) { return a.priority < b.priority; }) ? "TRUE" : "FALSE");
	printf("%I64i milliseconds\n\n", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

	auto t3 = std::chrono::high_resolution_clock::now();

	// 1M ints 650 ms, 65536 std::vector<int> 45 ms
	// 1M std::unique_ptr<int> 600 ms, 65536 std::unique_ptr<int> 44 ms [in a c-style array]
	// 65536 std::string 215 ms [in c-style array of structs]
	merge_sort(std::begin(ary), std::end(ary), [](const TEST & a, const TEST & b) { return a.priority < b.priority; });

	// 1M ints 700 ms, 65536 ints 19 ms
	// 65536 std::unique_ptr<int> 85 ms
	// 65536 std::string 240 ms
//	HeapSort(ary, NUM_ELEMENTS, [](TEST & a, TEST & b) { return a.priority < b.priority; });

	// 1M ints 375 ms, 65536 ints 19 ms
	// 65536 std::unique_ptr<int> 70 ms
	// 65536 std::string 175 ms
//	MergeSort(ary, NUM_ELEMENTS, [](TEST & a, TEST & b) { return a.priority >= b.priority; });

	// 1M ints 180 ms, 65536 ints (15ms w/o seconday sort optimization) (12 ms with it [triggered only for smaller lists])
	// 65536 std::unique_ptr<int> 28 ms
	// 65536 std::string (65 ms w/selection-sort) (65 ms with insertion-sort) [however, non-negligible difference for object size]
//	QuickSort(ary, NUM_ELEMENTS, [](TEST & a, TEST & b) { if (a.priority < b.priority) return -1; else if (a.priority > b.priority) return 1; return 0; });

//	SelectionSort(ary, NUM_ELEMENTS, [](int a, int b) { return a > b; });// 1M ints more than 2min, 65536 ints 8500 ms
//	InsertionSort(ary, NUM_ELEMENTS, [](int a, int b) { return a > b; });// 1M ints more than 2min, 65536 ints 3000 ms
	auto t4 = std::chrono::high_resolution_clock::now();

	// NOTE: st::is_sorted only checks for ascending order, unless a compare function is defined
	printf("Sorted-STD-Merge: %s\n", std::is_sorted(std::begin(ary), std::end(ary), [](TEST & a, TEST & b) { return a.priority < b.priority; }) ? "TRUE" : "FALSE");
	printf("%I64i milliseconds\n\n", std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count());

	return 0;
}
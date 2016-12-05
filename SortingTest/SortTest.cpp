#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <time.h>		// time()
#include <chrono>

#include <string>
#include <memory>		// std::unique_ptr<type>
#include "Sort.h"

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
#define NUM_ELEMENTS 65536
int ary[NUM_ELEMENTS];

int main() {
	
	srand(time(NULL));
		
//	auto random = std::vector<int>{};
//	random.reserve(NUM_ELEMENTS);
	for (int i = 0; i < NUM_ELEMENTS; i++)
		ary[i] = rand() % NUM_ELEMENTS;
//		random.push_back(std::move(rand() % NUM_ELEMENTS));
	
	auto   t1 = std::chrono::high_resolution_clock::now();
//	merge_sort(std::begin(random), std::end(random));					// 1,000,000 ints 650 ms, 65536 ints 45 ms (release build)
	HeapSort(ary, NUM_ELEMENTS, [](int a, int b) { return a < b; });	// 1,000,000 ints 700 ms, 65536 ints 19 ms (release build)
//	MergeSort(ary, NUM_ELEMENTS, [](int a, int b) { return a <= b; });	// 1,000,000 ints < 400 ms, 65536 ints 19 ms (release build)
	auto   t2 = std::chrono::high_resolution_clock::now();
	printf("%I64i milliseconds\n", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
	return 0;
}
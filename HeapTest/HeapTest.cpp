#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <string>
#include <memory>		// std::unique_ptr<type>
#include "Heap.h"

int GUID = 0;
typedef struct test_s {
	int priority;
	int id;
	int value;
	test_s(const int priority, const int value) : priority(priority), id(GUID++), value(value) {};
	test_s(const int priority) : priority(priority), id(GUID++), value() {};
	test_s() : priority(-1), id(-1), value() {};
} test_t;

// use a lambda function to define the comparison used by the template
// every lambda closure has a unique type, regardless of content
// only swaps if greater (not if equal) to generate a MaxHeap (simply reverse for MinHeap)
auto max = [](test_t & a, test_t & b) { return b.priority > a.priority; };
//	auto copy = max;	// works in place of max for template type definition and instance copy
//	auto dupe = [](test_t a, test_t b) { return b.priority > a.priority; }; // doesn't work in place of max
//	auto min = [](test_t a, test_t b) { return b.priority < a.priority; };	// ditto

void PrintHeap(eHeap<test_t, decltype(max)> & heap) {
	int i;

	i = 0;
	printf("\nSize == %i\n\n", heap.Size());
	while (i < heap.Size()) {
		printf("HEAP[%i] = {%i,\t%i,\t%i}\n", i, heap[i]->priority, heap[i]->id, heap[i]->value);
		i++;
	}
}

void FillHeapWithInts(eHeap<test_t, decltype(max)> & heap, const int num, const int seed, bool rvalues) {
	test_t * lValues;
	int i;

	lValues = new test_t[num];
	srand(seed);
	i = 0;
	printf("ADDING ");
	if (!rvalues) {
		printf("(L-VALUES):\n");
		while (i < num) {
			lValues[i] = { rand() % 10, i + 1 * 5 };
			heap.PushHeap(lValues[i++]);
		}
	} else {
		printf("(R-VALUES):\t");
		while (i < num) {
			heap.PushHeap(test_t{ rand() % 10, i++ + 1 * 10 });
		}
	}
	PrintHeap(heap);
	delete[] lValues;
}

// unit test
// all tests confirmed with POD types
// TODO: test with std::string, and std::unique_ptr<int>
int main() {
	eHeap<test_t, decltype(max)> first(std::move(max));		// lvalue eHeap ("default" constructor)

	FillHeapWithInts(first, 10, 1, false);

	auto eqls = [](test_t & a, int b) { return a.id == b; };
	const test_t * result = first.FindByKey(7, eqls);
	if (result != nullptr)
		printf("\n\nFound test_t (GUID 7): {%i\t%i\t%i}\n\n", result->priority, result->id, result->value);


//	eHeap<test_t, decltype(max)> second = first;			// copy ctor: works
//	eHeap<test_t, decltype(max)> second = std::move(first);	// move ctor: works
//	first.~eHeap();											// moved out of an lvalue, destroy it
	eHeap<test_t, decltype(max)> second(max);				// "default" ctor
	second = first;											// copy assignemnt: works
//	second = std::move(first);								// move assignement: works
//	first.~eHeap();											// moved out of lvalue, destroy it

//	second.Clear();											// works (nothing to print, nothing to pop)
	second.ReplaceRoot(test_t{999, 999});					// emplace and move: works
	second.ReplaceRoot(test_t{ -1, -1 });

	test_t lValue{666,666};									// copy replaceroot: works
	first.ReplaceRoot(lValue);

	auto third = first + second;							// merge (heapify, heapify copy ctor): works

	eHeap<test_t, decltype(max)> fourth(max);

	fourth.PushHeap(test_t(456, 456));
	fourth += third;										// meld: works

	auto eqls2 = [](test_t & a, int b) { return a.value == b; };
	fourth.RemoveByKey(666, eqls2);							// remove key: works

	fourth.ReplaceByKey(5, -9, eqls);						// emplace replace: works
	test_t lValue2{15, 777};
	fourth.ReplaceByKey(7, lValue2, eqls);					// copy replace: works

	PrintHeap(first);
	PrintHeap(second);
	PrintHeap(third);
	PrintHeap(fourth);

	while(!fourth.IsEmpty()) {
		printf("Top (Priority, GUID, Value): %i, %i, %i\nRoot Popped\n\n", fourth.PeekRoot()->priority, fourth.PeekRoot()->id, fourth.PeekRoot()->value);
		fourth.PopRoot();
	}

	return 0;
}
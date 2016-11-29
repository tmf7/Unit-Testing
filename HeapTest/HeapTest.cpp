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
auto max = [](test_t a, test_t b) { return b.priority > a.priority; };
//	auto dupe = [](test_t a, test_t b) { return b.priority > a.priority; };
//	auto min = [](test_t a, test_t b) { return b.priority < a.priority; };

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

int main() {
	eHeap<test_t, decltype(max)> first(std::move(max));		// lvalue eHeap ("default" constructor)

	FillHeapWithInts(first, 10, 26, false);

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

	PrintHeap(first);
	PrintHeap(second);

	while(!second.IsEmpty()) {
		printf("Top (Priority, GUID, Value): %i, %i, %i\nRoot Popped\n\n", second.PeekRoot()->priority, second.PeekRoot()->id, second.PeekRoot()->value);
		second.PopRoot();
	}

	return 0;
}
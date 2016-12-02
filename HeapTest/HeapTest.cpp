#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <string>
#include <memory>		// std::unique_ptr<type>
#include "Heap.h"

int GUID = 0;
typedef struct test_s {
	int						priority;
	int						id;
	std::unique_ptr<int>	value;

	test_s(const int priority, std::unique_ptr<int> value) : priority(priority), id(GUID++) { std::swap(this->value, value); };
	test_s(const int priority) : priority(priority), id(GUID++), value(std::make_unique<int>(rand() % 100)) {};
	test_s() : priority(-1), id(-1), value(nullptr) {};
//	test_s(const test_s & other) : priority(other.priority), id(GUID++), value(other.value) {};
	test_s(test_s && other) : priority(other.priority), id(GUID++), value(std::move(other.value)) {};
	test_s & operator=(test_s other) {
		std::swap(priority, other.priority);
		std::swap(id, other.id);
		std::swap(value, other.value);
		return *this;
	};
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

	printf("\nSize == %i\n\n", heap.Size());
	for (i = 0; i < heap.Size(); i++)
		printf("HEAP[%i] = {%i,\t%i,\t%i}\n", i, heap[i]->priority, heap[i]->id, *heap[i]->value);
}

void FillHeapWithInts(eHeap<test_t, decltype(max)> & heap, const int num, const int seed, bool rvalues) {
//	test_t * lValues;
	int i;

//	lValues = new test_t[num];
	srand(seed);
	i = 0;
	printf("ADDING ");
	if (!rvalues) {
		printf("(L-VALUES):\n");
		while (i < num) {
//			lValues[i] = { rand() % 10, std::make_unique<int>(i + 1 * 5) };
//			heap.PushHeap(lValues[i++]);
		}
	} else {
		printf("(R-VALUES):\t");
		while (i < num) {
			heap.PushHeap(test_t{ rand() % 10, std::make_unique<int>(i++) });
		}
	}
	PrintHeap(heap);
//	delete[] lValues;
}

// unit test
// all tests confirmed with POD types, std::string, std::unique_ptr<int>
int main() {
	eHeap<test_t, decltype(max)> first(max);		// lvalue eHeap ("default" constructor)

	FillHeapWithInts(first, 10, 1, true);

//////////////////////////////////////
// BEGIN std::string use case
/*
	// push (copy/move): 
	test_t lValues[] = 	{ 
		{ 0, "This first one has a bunch of characters, like alot..." },
		{ 1, "One" },
		{ 2, "simple text" },
		{ 3, "one two three four five six seven eight nine ten" },
		{ 4, "Four" }, 
	};

	first.PushHeap(lValues[0]);
	first.PushHeap(test_t{ rand() % 50, "This second one is more than alot more characters" });
	first.PushHeap(lValues[1]);
	first.PushHeap(test_t{ rand() % 50, "less than "});
	first.PushHeap(lValues[2]);
	first.PushHeap(test_t{ rand() % 50, "simple text2" });
	first.PushHeap(lValues[3]);
	first.PushHeap(test_t{ rand() % 50, " ten"});
	first.PushHeap(lValues[4]);
*/
// END std::string use case
///////////////////////////////////////

	auto eqls = [](test_t & a, int b) { return a.priority == b; };
	const test_t * result = first.FindByKey(7, eqls);
	if (result != nullptr)
		printf("\n\nFound test_t (priority 7): {%i\t%i\t%i}\n\n", result->priority, result->id, *result->value);


//	eHeap<test_t, decltype(max)> second = first;			// copy ctor: works


//	eHeap<test_t, decltype(max)> second = std::move(first);	// move ctor: works
//	first.~eHeap();											// moved out of an lvalue, destroy it
	eHeap<test_t, decltype(max)> second(max);				// "default" ctor
//	second = first;											// copy assignemnt: works
//	second = std::move(first);								// move assignement: works
//	first.~eHeap();											// moved out of lvalue, destroy it

//	second.Clear();											// works (nothing to print, nothing to pop)
	second.ReplaceRoot(test_t{ 999, std::make_unique<int>(999) });		// emplace and move: works
	second.ReplaceRoot(test_t{ -1, std::make_unique<int>(-999) });

//	test_t sLValue{ 666, std::make_unique<int>(666) };		// copy replaceroot: works
//	first.ReplaceRoot(sLValue);

	auto third = std::move(first) + std::move(second);		// merge (heapify, heapify copy ctor): works
	first.~eHeap();
	second.~eHeap();

	eHeap<test_t, decltype(max)> fourth(max);

	fourth.PushHeap(test_t{ 456, std::make_unique<int>(456) });
	fourth += std::move(third);								// meld: works
	third.~eHeap();

	auto eqls2 = [](test_t & a, const int b) { return *a.value == b; };
	fourth.RemoveByKey(-999, eqls2);						// remove key: works

	fourth.ReplaceByKey(4, -9, eqls);						// emplace replace: works
//	test_t lValue2{ 15, std::make_unique<int>(777) };
//	fourth.ReplaceByKey(7, lValue2, eqls);					// copy replace: works
/**/
	PrintHeap(first);
	PrintHeap(second);
	PrintHeap(third);
	PrintHeap(fourth);

	while(!fourth.IsEmpty()) {
		printf("Top (Priority, GUID, Value): %i, %i, %i\nRoot Popped\n\n", fourth.PeekRoot()->priority, fourth.PeekRoot()->id, *fourth.PeekRoot()->value);
		fourth.PopRoot();
	}

	return 0;
}
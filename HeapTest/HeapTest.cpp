#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <string>
#include <memory>		// unique_ptr<type>
#include <chrono>
#include "Heap.h"

int GUID = 0;
typedef struct test_s {
	int priority;
	int id;
	std::string value;
	test_s(const int priority) : priority(priority), id(GUID++), value() {};
	test_s() : priority(-1), id(-1), value() {};
} test_t;

int main() {
	int i;
	// use a lambda function to define the comparison used by the template
	// only swaps if greater (not if equal) to generate a MaxHeap (simply reverse for MinHeap)
	auto max = [](test_t a, test_t b) { return b.priority > a.priority; };
	eHeap<test_t, decltype(max)> testHeap(std::move(max));
	test_t lValues[10];

	srand(2);
	i = 0;
	while (i < 10)
		lValues[i++] = rand() % 10;

	lValues[0].value = "Once";
	lValues[1].value = "upon";
	lValues[2].value = "a";
	lValues[3].value = "time in the land of twenty 4 characters";
	lValues[4].value = "the lonely _underscore_ doth...";
	lValues[5].value = "read";
	lValues[6].value = "a";
	lValues[7].value = "blah blah. The END";
	lValues[8].value = "TWICE";
	lValues[9].value = "THREEEEE TIMES A LADyeee";

	printf("Initial non-heapified data\n");
	for (i = 0; i < 10; i++)
		printf("lValues[%i]: %i\n", i, lValues[i].priority);

	testHeap.PushHeap(lValues[0]);
	testHeap.PushHeap(lValues[1]);
	testHeap.PushHeap(lValues[2]);
	testHeap.PushHeap(lValues[3]);
	testHeap.PushHeap(lValues[4]);
	testHeap.PushHeap(lValues[5]);
	testHeap.PushHeap(lValues[6]);
	testHeap.PushHeap(lValues[7]);
	testHeap.PushHeap(lValues[8]);
	testHeap.PushHeap(lValues[9]);

	testHeap.ReplaceRoot(lValues[0]);
	testHeap.ReplaceRoot(lValues[0]);

	printf("\nNew Heap Size: %i\n\n", testHeap.Size());
	while(!testHeap.IsEmpty()) {
		printf("Top (Priority, GUID, Value): %i, %i, %s\nRoot Popped\n\n", testHeap.PeekRoot()->priority, testHeap.PeekRoot()->id, testHeap.PeekRoot()->value.c_str());
		testHeap.PopRoot();
	}

	return 0;
}
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <string>
#include <memory>
#include "Deque.h"

#define TEST_TYPE std::unique_ptr<int>
#define UPTR(x) std::make_unique<int>(x)

////////////////////////////////////////////////////////////////////////
// BEGIN TEST
int GUID = 0;		

typedef struct T_s {
	const int value;
	int id;

	T_s(const int && value) : value(std::move(value)), id(GUID++) {
		printf("MOVE CONSTRUCT\t(GUID %i): %i\tat address %p\n\n", id, this->value, &(this->value));
	};

	T_s(const int & value) : value(value), id(GUID++) {
		printf("COPY CONSTRUCT\t(GUID %i): %i\tat address %p\n\n", id, this->value, &(this->value)); 
	};

	~T_s() { 
		printf("DESTRUCT\t(GUID %i): %i\tat address %p\n\n", id, this->value, &(this->value)); 
	};
} T;

void DestructorTest() {
	T lValue{ 10 };
	// default construction of T, copy constructon of unique_ptr (or something)
	std::unique_ptr<T> D(std::make_unique<T>(lValue));
	printf("D->value ==\t(GUID %i): %i\tat address %p\n\n", D->id, D->value, &(D->value));

	// default construction of T, copy constructon of unique_ptr (or something)
	std::unique_ptr<T> A(new T(0));		
	printf("A->value ==\t(GUID %i): %i\tat address %p\n\n", A->id, A->value, &(A->value));

	// default construction of T, copy construction of unique_ptr
	std::unique_ptr<T> B = std::make_unique<T>(T(1));		
	printf("B->value ==\t(GUID %i): %i\tat address %p\n\n", B->id, B->value, &(B->value));

	// default construction of T, move assignemnt of unique_ptr
	B = std::make_unique<T>(T(2)); 
	printf("B->value ==\t(GUID %i): %i\tat address %p\n\n", B->id, B->value, &(B->value));
}
// END TEST
/////////////////////////////////////////////////////////////////////////

// iterate and print back to front
template<class type>
void PrintDeque(const eDeque<type> & de, const char * name) {
	eNode<TEST_TYPE> * iterator;
	int i;
	
	printf("%s size == %i\n", name, de.Size());
	for (i = 0, iterator = de.Back(); iterator != nullptr; iterator = iterator->Next())
		printf("node %i == %i\n", i++, *iterator->Data());
	printf("\n\n");
}

// iterate and increment by amount back to front
template<class type>
void IncrementData(const eDeque<type> & de, int amount) {
	eNode<TEST_TYPE> * iterator;

	for (iterator = de.Back(); iterator != nullptr; iterator = iterator->Next())
		*iterator->Data() += amount;
}

// iterate and append the given data back to front
template<class type>
void AppendToString(const eDeque<type> & de, const char * append) {
	eNode<TEST_TYPE> * iterator;

	for (iterator = de.Back(); iterator != nullptr; iterator = iterator->Next())
		iterator->Data() += append;	// DEBUG: only used for std::string
}

// unit test
int main() {
	TEST_TYPE lValues[] = { UPTR(0), UPTR(1), UPTR(2), UPTR(3), UPTR(4), UPTR(5), UPTR(6), UPTR(7), UPTR(8), UPTR(9) };
//	TEST_TYPE lValues[] = { "t h e", " quick ", "_br own__", "fo!x (you may need more than 24 characters)", "jUmpeD", "Ov er", "the", "lazy", "dog." };
	printf("BEGIN eDeque COPY TESTS\n\n");

	eDeque<TEST_TYPE> original;
/*
	// illegal with std::unique_ptr
	original.PushFront(lValues[0]);
	original.PushFront(lValues[1]);
	original.PushFront(lValues[2]);
	original.PushFront(lValues[3]);
	original.PushFront(lValues[4]);
	original.PushFront(lValues[5]);
	original.PushBack(lValues[6]);
*/
	original.PushFront(UPTR(0));
	original.PushFront(UPTR(1));
	original.PushFront(UPTR(2));
	original.PushFront(UPTR(3));
	original.PushFront(UPTR(4));
	original.PushFront(UPTR(5));
	original.PushBack(UPTR(6));
	original.PopBack();
	original.PopFront();
	PrintDeque(original, "original");
/*
	eDeque<TEST_TYPE> A(original);									// copy constructor test
	printf("A(original)\n");
	PrintDeque(A, "A result");
*/
	eDeque<TEST_TYPE> B;
	printf("B.Size() < original.Size()\n");
	PrintDeque(B, "B_before_copyAssign");
	B = std::move(original);										// copy assignment test with B.Size() < original.Size()
	printf("B = std::move(original)\n");
	PrintDeque(B, "B result");

	eDeque<TEST_TYPE> C;
	// emplace and move calls
/*
	C.PushBack("once the quick brown fox jumped over the lazy dog");
	C.PushFront("the upon brown fox jumped over the lazy dog");
	C.PushBack("the quick a fox jumped over the laxy dog");
	C.PushFront("time");
	C.PushBack("in the quick brown fox");
	C.PushFront("the");
	C.PushBack("land");
	C.PushFront("of");
*/
	C.PushBack(std::move(lValues[0]));
	C.PushFront(std::move(lValues[1]));
	C.PushBack(std::move(lValues[2]));
	C.PushFront(std::move(lValues[3]));
	C.PushBack(std::move(lValues[4]));
	C.PushFront(std::move(lValues[5]));
	C.PushBack(std::move(lValues[6]));
	C.PushFront(std::move(lValues[7]));
	printf("C.Size() > original.Size()\n");
	PrintDeque(C, "C_before_moveAssign");
//	C = original;										// copy assignment test with C.Size() > original.Size()
//	printf("C = original\n");
//	PrintDeque(C, "C result");

	// verify the original is intact
	printf("original deque intact\n");
	PrintDeque(original, "original");
/*
//	IncrementData(C, 5);
//	printf("incremented each node of C by 5\n");
	AppendToString(C, "_ added");
	printf("appended each node of C with \"_ added\"\n");
	PrintDeque(C, "C result");

	printf("B.Size() == C.Size()\n");
	B = C;												// copy assignment test with B.Size() == C.Size()
	printf("B = C\n");
	PrintDeque(B, "B result");
*/
///////////////////////////////////////
// BEGIN eDeque move semantics testing							
	eDeque<TEST_TYPE> G = eDeque<TEST_TYPE>(std::move(B));// copy construct an rvalue eDeque, then move construct G (actually calls the cctor...)
	printf("eDeque<int> G = eDeque<int>(B) copy construction\n");
	PrintDeque(G, "G result");

	IncrementData(B, 10);
	printf("incremented each node of B by 10\n");
//	AppendToString(B, " another _");
//	printf("appended each node of B with \" another _\"\n");
	PrintDeque(B, "B result");

//	eDeque<TEST_TYPE> I;								// DEBUG: default construct line
	G = eDeque<TEST_TYPE>(std::move(C));				// move assign an rvalue eDeque, then move assign G (C becomes undefined/unspecified)
	printf("G = eDeque<int>(C) move assignment (C becomes unspecified)\n");
	PrintDeque(G, "G result");
	PrintDeque(C, "C result");

//	TEST_TYPE lValue = "you might need more than 24 characters here";
	TEST_TYPE lValue = UPTR(666);
	printf("G.PushBack(std::move(lValue == 666)); G.PushFront(777) emplace and move\n");
	G.PushBack(std::move(lValue));						// emplace and move
	G.PushFront(UPTR(777));								// emplace and move
	PrintDeque(G, "G result");
// END eDeque move semantics testing
/////////////////////////////////////

	printf("END eDeque COPY TESTS\n\n");
	printf("BEGIN eNODE COPY TESTS\n\n");

	// DEBUG: how to break a unique_ptr like a sophisticated noob
//	int * testPtr = &(*std::unique_ptr<int>(new int(123456)));
//	*testPtr = 7890;

	eNode<TEST_TYPE> * newData = new eNode<TEST_TYPE>(TEST_TYPE(new int(999)));//("hello world, how are you today?");
	eNode<TEST_TYPE> * D = G.Front()->Prev();			// overwrite the node data just behind the front
	(*D) = std::move(*newData);							// node copy assignment test (DEBUG: illegal with unique_ptr<type> as it should be)
	printf("(*D) == (*G.Font()->Prev()) = *newData\n");
	PrintDeque(G, "modified G result");

	eNode<TEST_TYPE> E(std::move(*D));					// node cctor test (DEBUG: illegal with unique_ptr<type> as it should be)
//	delete newData;										// E is still defined because it made a copy of the newData's data
	printf("E(*D), result: E.Data() == %i\n", *E.Data());
	
/*
	//  DEBUG: copy construction illegal with unique_ptr<type> as it should be
	eNode<TEST_TYPE> F(*D);								// node cctor test (copies source node data, not source's next/prev)
	printf("F(*D) result: F.Data() == %i, F.Next() == %p, F.Prev() == %p\n\n", *F.Data(), F.Next(), F.Prev());
*/
/////////////////////////////////////
// BEGIN eNode move semantics testing
	eNode<TEST_TYPE> H = eNode<TEST_TYPE>(UPTR(123));//("YOU DEfinitely may need move than 24 ch4r4ct3rs h1r1!!");		
														// move construction
	printf("eNode<int> H = eNode<int>(123) move construction result: H.Data() == %i\n\n", *H.Data());


	H = eNode<TEST_TYPE>(UPTR(456));					// move assignment
	printf("H = eNode<int>(456) move assignment result: H.Data() == %i, H.Next() == %p, H.Prev() == %p\n\n", *H.Data(), H.Next(), H.Prev());


	*G.Front()->Prev() = std::move(H);					// DEBUG: try to move something into the moved-out-of slot of G (second to last)
	PrintDeque(G, "G result");
// END eNode move semantics testing
/////////////////////////////////////
	DestructorTest();
/**/
	printf("END eNODE COPY TESTS\n\n");
	return 0;
}




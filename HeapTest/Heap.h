#ifndef EVIL_HEAP_H
#define EVIL_HEAP_H

#include <new.h>		// std::move
#include <utility>		// std::swap

#define DEFAULT_HEAP_SIZE 1024
#define DEFAULT_HEAP_GRANULARITY 1024


// TODO: use lazy deletion for IncreaseKey/DecreaseKey: insert new data at the bottom and heapify, as well as
// mark data as "visited" then pop it when it reaches the top)

// TODO: this class will be used to implement two priority queues of eTiles, each eTile will have:
// A float for its weight (for A* search) (one way of ordering a heap)
// An int for its z-depth (for draw order heapsort) (another way of ordering, then sorting, a heap)

// TODO: generalize things to be drawn in renderEntity_t structs such that a tile has one and an entity has one, and when
// they're ready to be drawn they 

// TODO (draw order): only create a heap (and heapsort) renderEntity_ts ready to be drawn and that are visible in
// on the backbuffer (ie nothing should change about them once ready to draw [no increase/decreasekey]).
// Further, this heap should be cleared (not the memory freed) each frame

// TODO (A* search): heap can be cleared once the search is complete (not the memory freed)
// AND after its list of best waypoints has been pushed onto a bestPath deque

// TODO (eDeque/eHeap): add "noexcept" after all functions that use std::swap (move semantics exception safety guarantee)
// DEBUG: the template compare function a user creates dictates if this is a MaxHeap or a MinHeap
// DEBUG: this heap contains copies of the source data, thus changing the source data will not affect this heap and vis vesa
// TODO: fix function header comments
template <class type, class lambdaCompare>
class eHeap {
public:

	typedef eHeap<type, lambdaCompare> heap_t;

							eHeap() = delete;									// relaced default constructor
							eHeap(lambdaCompare compare, const int initialHeapSize = DEFAULT_HEAP_SIZE);

//							eHeap(const type * data);							// heapify copy constructor
							eHeap(const eHeap<type, lambdaCompare> & other);	// copy constructor
							eHeap(eHeap<type, lambdaCompare> && other);			// move constructor
							~eHeap();											// destructor

	heap_t &				operator=(eHeap<type, lambdaCompare> other);		// copy and swap assignement
//	heap_t					operator+(const eHeap & one, const eHeap & two);	// global merge returns a new heap (move this out of class declaration)
//	heap_t &				operator+=(const eHeap other);						// class meld returns *this changed heap (uses copy-and-swap idiom to use move or copy semantics with one function call)
	const type * const		operator[](const int index) const;					// DEBUG: to print the heap contents in arbirary order

	const type * const		PeekRoot() const;
	void					PushHeap(const type & data);
	void					PushHeap(type && data);
	void					PopRoot();
	void					ReplaceRoot(const type & data);
	void					ReplaceRoot(type && data);
//	type					ExtractRoot();										// returns a copy of the root, deletes the root (possible copy exceptions)

/*
	// TODO: potentially template these search functions to take 
	// a different data type that'll be searched by,
	// which should be part of a larger type (eg: test_t::id, test_t::value, test_t::priority, etc)
	// DEBUG: these would be universal references (forwarding references)
	template<typename dataMember>
	bool					FindKey(dataMember && key, type & result) const;	// determine if the key is in the heap
																				// FIXME/BUG: this assumes that the complete data spec is known, 
																				// but what if just a key is known (eg "graphics/door.bmp") which 
	template<typename dataMember>												// will return the complete data spec (via the partial spec)
	bool					Delete(dataMember && key);							// remove an arbitrary node (same FIXME as FindKey)

	template<typename dataMember>
	void					ReplaceKey(dataMember && oldKey, dataMember && newKey);	// replace an arbitrary key (then if (compare('parent', 'newKey')){ SiftUp(index); } else { SiftDown(index); })
																				// DEBUG: current intention to use "lazy" deletion where the 
																				// newKey is just pushed and the oldKey is marked and poped 
*/																				// when it reaches the top 
//	void					Heapify(type * data);								// global heapify (do not make a copy?) (move this out of class declaration)

	int						Size() const;
	bool					IsEmpty() const;

	// memory management
	size_t					Allocated() const;
	void					SetGranularity(const int newGranularity);
	void					Free();
	void					Resize(const int newHeapSize);
	void					Clear();
	void					ClearAndResize(const int newHeapSize);

private:
	
	void					Allocate(const int newHeapSize);
	void					SiftUp(const int index);
	void					SiftDown(const int index);

private:

	type *					heap;			
	lambdaCompare			compare;
	int						numElements;
	int						granularity;
	int						heapSize;
};

// default constructor, doesn't allocate until the first insert
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare>::eHeap(lambdaCompare compare, const int initialHeapSize) 
	: compare(compare),
	  numElements(0),
	  granularity(DEFAULT_HEAP_GRANULARITY),
	  heapSize(initialHeapSize),
	  heap(nullptr) {
}

/*
// FIXME: this has no way of knowing how big the given data set is
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare>::eHeap(const type * data) {
	// TODO: if this were to do a swap of the data pointer (**assumed** head of a c-style array)
	// then that wouldn't guarantee that the old array would be freed properly...itd be the users responsibility
	// though that isn't obvious
}
*/

// copy constructor
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare>::eHeap(const eHeap<type, lambdaCompare> & other) 
	: compare(other.compare),
	  numElements(other.numElements),
	  granularity(other.granularity),
	  heapSize(other.heapSize) 
{
	heap = new type[heapSize];
	memcpy(heap, other.heap, other.heapSize * sizeof(other.heap[0]));
}

// move constructor
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare>::eHeap(eHeap<type, lambdaCompare> && other) 
	: compare(other.compare),
	  numElements(0),
	  granularity(0),
	  heapSize(0),
	  heap(nullptr) 
{
	std::swap(numElements, other.numElements);
	std::swap(granularity, other.granularity);
	std::swap(heapSize, other.heapSize);
	std::swap(heap, other.heap);
}

// copy and swap assignement
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare> & eHeap<type, lambdaCompare>::operator=(eHeap<type, lambdaCompare> other) {
	std::swap(numElements, other.numElements);
	std::swap(granularity, other.granularity);
	std::swap(heapSize, other.heapSize);
	std::swap(heap, other.heap);
	// DEBUG: compare closure swap omitted because both eHeaps already contain the relevant instance,
	// as well as to avoid invoking deleted default lambda constructor
	return *this;
}

// arbitrary indexing operator
template<class type, class lambdaCompare>
inline const type * const eHeap<type, lambdaCompare>::operator[](const int index) const {
	return (heap == nullptr || index >= numElements || index < 0) ? nullptr : &heap[index];
}

// destructor
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare>::~eHeap() {
	Free();
}

// peek
template<class type, class lambdaCompare>
inline const type * const eHeap<type, lambdaCompare>::PeekRoot() const {
	return heap == nullptr ? nullptr : &heap[0];
}

// copy push
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::PushHeap(const type & data) {
	if (heap == nullptr)
		Allocate(heapSize);
	else if (numElements + 1 >= heapSize)
		Resize(heapSize + 1);

	heap[numElements] = data;
	SiftUp(numElements);
	numElements++;
}

// emplace and move push
// FIXME/BUG: this assumes the template type has a proper move assignment operator
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::PushHeap(type && data) {
	if (heap == nullptr)
		Allocate(heapSize);
	else if (numElements + 1 >= heapSize)
		Resize(heapSize + 1);

	heap[numElements] = std::move(data);
	SiftUp(numElements);
	numElements++;
}

// pop
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::PopRoot() {
	if (heap == nullptr) {
		return;
	} else if (numElements == 1) {
		Clear();
		return;
	}
	numElements--;
	heap[0] = heap[numElements];
	SiftDown(0);
}

// copy replace
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::ReplaceRoot(const type & data) {
	if (heap == nullptr) {
		PushHeap(data);
		return;
	} 
	heap[0] = data;
	SiftDown(0);
}

// emplace and move replace
// FIXME/BUG: this assumes the template type has a move assignment operator (default or otherwise)
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::ReplaceRoot(type && data) {
	if (heap == nullptr) {
		PushHeap(std::move(data));
		return;
	}
	heap[0] = std::move(data);
	SiftDown(0);
}


// size
template<class type, class lambdaCompare>
inline int eHeap<type, lambdaCompare>::Size() const {
	return numElements;
}

// isEmpty
template<class type, class lambdaCompare>
inline bool eHeap<type, lambdaCompare>::IsEmpty() const {
	return numElements == 0;
}

// SiftUp
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::SiftUp(const int index) {
	int parent;
	int child;

	if (heap == nullptr)
		return;

	child = index;
	while (child > 0) {
		parent = (child - 1) / 2;
		if (compare(heap[parent], heap[child])) {
			std::swap(heap[parent], heap[child]);
			child = parent;
		} else {
			break;
		}
	}
}

// SiftDown
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::SiftDown(const int index) {
	int		parent;
	int		left;
	int		right;
	int		target;
	bool	leftExists;
	bool	rightExists;

	if (heap == nullptr || numElements <= 1)
		return;

	parent		= index;	
	left		= 2 * parent + 1;
	right		= 2 * parent + 2;
	leftExists	= left < numElements;
	rightExists = right < numElements;

	// DEBUG: a right child's existance implies that a left child exists, but not vis versa
	while (leftExists || rightExists) {

		target = !rightExists || compare(heap[right], heap[left]) ? left : right;
		if (compare(heap[parent], heap[target])) {
			std::swap(heap[parent], heap[target]);
			parent = target;
		} else {
			return;
		}
		left		= 2 * parent + 1;
		right		= 2 * parent + 2;
		leftExists	= left < numElements;
		rightExists = right < numElements;
	}
}

// Allocate
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::Allocate(const int newHeapSize) {
	Free();
	heapSize = newHeapSize;
	heap = new type[heapSize];					// FIXME/BUG: sizeof(type) may change (as bug below using an instance std::string)
	memset(heap, 0, heapSize * sizeof(heap[0]));// FIXME/BUG: same problem here
}

// Allocated
template<class type, class lambdaCompare>
inline size_t eHeap<type, lambdaCompare>::Allocated() const {
	return heapSize * sizeof(heap[0]);
}

// SetGranularity
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::SetGranularity(const int newGranularity) {
	granularity = newGranularity;
}

template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::Free() {
	if (heap != nullptr) {
		delete[] heap;
		heap = nullptr;
	}
	numElements = 0;
}

template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::Resize(const int newHeapSize) {
	type * oldHeap;
	int mod, newSize;

	if (newHeapSize <= heapSize)
		return;

	mod = newHeapSize % granularity;
	if (!mod)
		newSize = newHeapSize;
	else
		newSize = newHeapSize + granularity - mod;

	if (heap == nullptr) {
		heapSize = newSize;
		return;
	}

	oldHeap = heap;
	heap = new type[newSize];	// FIXME/BUG: sizeof(type) for a given instance could vary for example if it contains std::string longer than std::string's basic 28 byte allocation (24 characters)
	memcpy(heap, oldHeap, heapSize * sizeof(type));						// FIXME/BUG: same problem here
	memset(heap + heapSize, 0, (newSize - heapSize) * sizeof(type));	// FIXME/BUG: and here
	delete[] oldHeap;
	heapSize = newSize;
}

template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::Clear() {
	if (heap != nullptr) {
		memset(heap, 0, heapSize * sizeof(heap[0]));
		numElements = 0;
	}
}

template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::ClearAndResize(const int newHeapSize) {
	Free();
	heapSize = newHeapSize;
}

#endif /* EVIL_HEAP_H */
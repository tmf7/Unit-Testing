#ifndef EVIL_HEAP_H
#define EVIL_HEAP_H

#include <new.h>		// std::move
#include <utility>		// std::swap
//#include <math.h>		// floorf

#define DEFAULT_HEAP_SIZE 2
#define DEFAULT_HEAP_GRANULARITY 2


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
// TODO: abstract this for inheritance, then have each derived class define a comparison method? (see doom queue code)
// DEBUG: the template compare function a user creates dictates if this is a MaxHeap or a MinHeap
// DEBUG: this heap contains copies of the source data, thus changing the source data will not affect this heap and vis vesa
template <class type, class lambdaCompare>
class eHeap {			// TODO: include granularity for resizing? (like dooms HashIndex class)
public:						// otherwise put protocols for overfill

							eHeap(lambdaCompare && compare);			// create heap (empty)
//							eHeap(const type * data);					// heapify constructor
//							eHeap(const eHeap<type, compare> & other);	// copy constructor
//							eHeap(eHeap<type, compare> && other);		// move constructor
							~eHeap();									// destructor

	const type * const		PeekRoot() const;
	void					PushHeap(const type & data);				// push to the next available child slot, then SiftUp
//	void					PushHeap(type && data);						// move semantics Insert
	void					PopRoot();
//	type					ExtractRoot();								// returns a copy of the root, deletes the root (possible copy exceptions)
	void					ReplaceRoot(const type & data);
//	void					ReplaceRoot(type && data);					// move semantics ReplaceRoot

//	const type &			FindKey(const int index) const;					// return a specific value (search by index)
//	const type &			FindKey(const type & data) const;				// return a specific value (searh by value) ???
//	void					Delete(const int index);						// remove an arbitrary node, move the last node there, then SiftUp or SiftDown
//	void					IncreaseKey(const int index, const type & newKey);		// modfy a specific key/index
//	void					DecreaseKey(const int index, const type & newKey);		// ditto

//	static void				Heapify(type * data);							// arrange the elements of the given array into a binary heap order (for heapsort)
//	static eHeap<type, lambdaCompare>		Merge(const eHeap<type, lambdaCompare> & a, const eHeap<type, lambdaCompare> & b);	// make a new heap preserve original heaps
//	eHeap<type> &			Meld(eHeap<type, lambdaCompare> & other);		// create a heap from the two, destroy the old
//	eHeap<type> &			Meld(eHeap<type, lambdaCompare> && other);		// create a heap from the two, destroy the old (move semantics)

	int						Size() const;
	bool					IsEmpty() const;

	size_t					Allocated() const;							// TODO: test
	void					SetGranularity(const int newGranularity);	// TODO: test
	void					Free();
	void					Resize(const int newHeapSize);				// TODO: test
	void					Clear();
	void					ClearAndResize(const int newHeapSize);		// TODO: test

private:
	
	void					Init(const int initialHeapSize);
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
inline eHeap<type, lambdaCompare>::eHeap(lambdaCompare && compare) : compare(std::move(compare)) {
	Init(DEFAULT_HEAP_SIZE);
}

/*
// FIXME: this has no way of knowing how big the given data set is
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare>::eHeap(const type * data) {
	// TODO: if this were to do a swap of the data pointer (assumed head of a c-style array)
	// then that wouldn't guarantee that the old array would be freed properly...itd be the users responsibility
	// though that isn't obvious
}

// copy constructor
template<class type, class lambdaCompare>
inline eHeap<type, lambdaCompare>::eHeap(const eHeap<type, lambdaCompare> & other) {
}

// move constructor
template<class type, class compare>
inline eHeap<type, lambdaCompare>::eHeap(eHeap<type, lambdaCompare> && other) {
}
*/

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

/*
// emplace and move push
// FIXME/BUG: this assumes the template type has a proper move assignment operator
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::PushHeap(const type && data) {
	if (heap == nullptr)
		Allocate(heapSize);
	else if (numElements + 1 >= heapSize)
		Resize(heapSize + 1);

	heap[numElements++] = std::move(data);
	SiftUp(numElements);
}
*/

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

/*
// emplace and move replace
// FIXME/BUG: this assumes the template type has a move assignment operator
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::ReplaceRoot(const type && data) {
	if (heap == nullptr) {
		PushHeap(data);
		return;
	}
	heap[0] = std::move(data);
	SiftDown(0);
}
*/

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


/////////////////////////////////////////////////////////////////////////////
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::Init(const int initialHeapSize) {
	heapSize = initialHeapSize;
	heap = nullptr;
	granularity = DEFAULT_HEAP_GRANULARITY;
}

// Allocate
template<class type, class lambdaCompare>
inline void eHeap<type, lambdaCompare>::Allocate(const int newHeapSize) {
	Free();
	heapSize = newHeapSize;
	heap = new type[heapSize];
	memset(heap, 0, heapSize * sizeof(heap[0]));
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
	heap = new type[newSize];
	memcpy(heap, oldHeap, heapSize * sizeof(heap[0]));
	memset(heap + heapSize, 0, (newSize - heapSize) * sizeof(heap[0]));
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
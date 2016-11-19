#ifndef EVIL_DEQUE_H
#define EVIL_DEQUE_H

#include <new.h>
#include <utility>

template<class type>
class eDeque;
//*************************************************
//				eNode
// to be used with friend class eDeque<type>
//*************************************************
template<class type>
class eNode {
	
	friend class eDeque<type>;

public :

					eNode()							 : prev(nullptr), next(nullptr), data() {};							// default constructor
	explicit		eNode(const type & data)		 : prev(nullptr), next(nullptr), data(data) {};						// copy constructor (using data)
	explicit		eNode(type && data)				 : prev(nullptr), next(nullptr), data(std::move(data)) {};			// move constructor (using data)
					eNode(eNode<type> && other)		 : prev(nullptr), next(nullptr), data(std::move(other.data)) {};	// move constructor (using node)
					eNode(const eNode<type> & other) : prev(nullptr), next(nullptr), data(other.data) {};				// copy constructor (using node)
					// DEBUG: destructor omitted because no heap allocation occurs

	eNode<type> &	operator=(eNode<type> other) {			// copy and swap (defers to appropriate ctor) assignment
						std::swap(data, other.data);
						return *this;
					};

/*
	eNode<type> &	operator=(const eNode<type> & other) {																// copy assignment
												  data.~type(); 
												  new (&data) type(other.data); 
												  return *this;};	
	eNode<type> &	operator=(eNode<type> && other) {																		// move assignment
										//		  data.~type();			// DEBUG: std::unique_ptr calls its destructor (and deallocates its target)
																		// in its operator=(unique_ptr &&) function call
																		// so this would cause an assertion failure/crash.
																		// However, does omitting it then expose other problems
																		// like, the pointed-to object's destructor not getting called
												  std::swap(data, other.data); 
												  return *this;};
*/
	const type &	Data() const				{ return data; };
	type &			Data()						{ return data; };
	eNode<type> *	Prev() const				{ return prev; };
	eNode<type> *	Next() const				{ return next; };

private:

	eNode<type> *	prev;
	eNode<type> *	next;
	type			data;
};

//*************************************************
//				eDeque
// uses heap memory to manage copies of pushed data
// user code must check if deque is empty before accessing data
// TODO: replace the global new memory allocation with
// a custom heap memory block allocator (aligned) that has a templated/settable
// granularity and overridden new operator (to reduce the allocation overhead)
// DEBUG: will crash if it fails any new allocation (eg: PushFront, PushBack, operator=, cctor)
//*************************************************
template <class type>
class eDeque {
public:

						eDeque();									// default constructor
						eDeque(const eDeque<type> & other);			// copy constructor
						eDeque(eDeque<type> && other);				// move constructor
					   ~eDeque();									// destructor
			
	eDeque<type> &		operator=(const eDeque<type> & other);		// copy assignment
	eDeque<type> &		operator=(eDeque<type> && other);			// move assignment

	void				PushFront(const type & data);
	void				PushBack(const type & data);
	void				PushFront(type && data);					// emplace and move
	void				PushBack(type && data);						// emplace and move

	void				PopFront();
	void				PopBack();

	eNode<type> *		Back() const;
	eNode<type> *		Front() const;
	eNode<type> * 		FromFront(int index) const;
	eNode<type> * 		FromBack(int index) const;

	void				Clear();
	int					Size() const;
	bool				IsEmpty() const;

private:

	eNode<type> *		front;
	eNode<type> *		back;
	int					nodeCount;
};

//******************
// eDeque::eDeque
// default constructor empty eDeque
//******************
template <class type>
inline eDeque<type>::eDeque() : nodeCount(0), front(nullptr), back(nullptr) {
}

//******************
// eDeque::eDeque
// copy constructor
//******************
template <class type>
inline eDeque<type>::eDeque(const eDeque<type> & other) {
	eNode<type> * otherIterator;

	for (otherIterator = other.back; otherIterator != nullptr; otherIterator = otherIterator->next)
		PushFront(otherIterator->data);
}

///////////////////////////////////////////////////////////////
// BEGIN new move semantics code

//******************
// eDeque::eDeque
// move constructor
//******************
template <class type>
inline eDeque<type>::eDeque(eDeque<type> && other) : nodeCount(0), front(nullptr), back(nullptr) {
	std::swap(nodeCount, other.nodeCount);
	std::swap(front, other.front);
	std::swap(back, other.back);
}

//******************
// eDeque::operator=
// move assignment
//******************
template <class type>
inline eDeque<type> & eDeque<type>::operator=(eDeque<type> && other) {
	Clear();
	std::swap(nodeCount, other.nodeCount);
	std::swap(front, other.front);
	std::swap(back, other.back);
	return *this;
}

//******************
// eDeque::PushFront
// emplace and move
//******************
template <class type>
inline void eDeque<type>::PushFront(type && data) {
	eNode<type> * newFront;

	newFront = new eNode<type>(std::move(data));
	if (front == nullptr) {
		back = newFront;
		front = newFront;
	} else {
		newFront->prev = front;
		front->next = newFront;
		front = newFront;
	}
	nodeCount++;
}

//******************
// eDeque::PushBack
// emplace and move
//******************
template <class type>
inline void eDeque<type>::PushBack(type && data) {
	eNode<type> * newBack;

	newBack = new eNode<type>(std::move(data));
	if (back == nullptr) {
		back = newBack;
		front = newBack;
	}
	else {
		newBack->next = back;
		back->prev = newBack;
		back = newBack;
	}
	nodeCount++;
}

// END new move semantics code
/////////////////////////////////////////////////////////////////

//******************
// eDeque::~eDeque
//******************
template <class type>
inline eDeque<type>::~eDeque() {
	Clear();
}

//******************
// eDeque::operator=
// deep copy back to front
//******************
template <class type>
inline eDeque<type> & eDeque<type>::operator=(const eDeque<type> & other) {
	eNode<type> * thisIterator;
	eNode<type> * otherIterator;
	eNode<type> * newFront;


	// QUESTION: would this be faster than what I do in this function? Or, is heap deallocation/allocation more expensive?
//	Clear();
//	for (otherIterator = other.back; otherIterator != nullptr; otherIterator = otherIterator->next)
//		PushFront(otherIterator->data);
	

	// destroy and reconstruct pre-allocated memory if *this already has some
	for (thisIterator	= back,				  otherIterator	 = other.back; 
		 thisIterator  != nullptr	  &&	  otherIterator != nullptr;
		 thisIterator	= thisIterator->next, otherIterator  = otherIterator->next)
		(*thisIterator) = (*otherIterator);		// DEBUG: eNode assignemnt, becomes move assignment for rvalues/unique_ptr
	
	// establish a newFront if *this had more nodes than other
	newFront = nullptr;
	if (thisIterator != nullptr && otherIterator == nullptr)
		newFront = thisIterator->prev;

	// if *this had more nodes, pop all the nodes beyond other's size, down to newFront
	while (thisIterator != newFront) {
		PopFront();
		thisIterator = front;
	}

	// if *this had fewer nodes, push all the data remaining in other onto *this
	for (/*continue moving*/; otherIterator != nullptr; otherIterator = otherIterator->next)
		PushFront(otherIterator->data);

	return *this;
}

//******************
// eDeque::PushFront
// copies the data into a new node and links it to the front
//******************
template <class type>
inline void eDeque<type>::PushFront(const type & data) {
	eNode<type> * newFront;

	newFront = new eNode<type>(data);
	if (front == nullptr) {
		front = newFront;
		back = newFront;
	} else {
		newFront->prev = front;
		front->next = newFront;
		front = newFront;
	}
	nodeCount++;
}

//******************
// eDeque::PushBack
// copies the data into a new node and links it to the back
//******************
template <class type>
inline void eDeque<type>::PushBack(const type & data) {
	eNode<type> * newBack;

	newBack = new eNode<type>(data);
	if (back == nullptr) {
		back = newBack;
		front = newBack;
	} else {
		newBack->next = back;
		back->prev = newBack;
		back = newBack;
	}
	nodeCount++;
}

//******************
// eDeque::PopFront
// unlinks and deletes the front node
//******************
template <class type>
inline void eDeque<type>::PopFront() {
	eNode<type> * newFront;
	eNode<type> * oldFront;
	
	if (front->prev == nullptr) {			// last node in the deque
		delete front;
		front = nullptr;
		back = nullptr;
	} else {								// more than one node in the deque
		oldFront = front;
		newFront = front->prev;
		newFront->next = nullptr;
		front = newFront;
		delete oldFront;
	}
	nodeCount--;
}

//******************
// eDeque::PopBack
// unlinks and deletes the back node
//******************
template <class type>
inline void eDeque<type>::PopBack() {
	eNode<type> * newBack;
	eNode<type> * oldBack;

	if (back->next == nullptr) {			// last node in the deque
		delete back;
		front = nullptr;
		back = nullptr;
	} else {								// more than one node in the deque
		oldBack = back;
		newBack = back->next;
		newBack->prev = nullptr;
		back = newBack;
		delete oldBack;
	}
	nodeCount--;
}

//******************
// eDeque::Front
//******************
template <class type>
inline eNode<type> * eDeque<type>::Front() const {
	return front;
}

//******************
// eDeque::Back
//******************
template <class type>
inline eNode<type> * eDeque<type>::Back() const {
	return back;
}

//******************
// eDeque::FromFront
// returns the node "index" nodes behind the front
// index between [ 0, Size() - 1 ]
// returns nullptr for out-of-bounds index or empty deque
//******************
template <class type>
inline eNode<type> * eDeque<type>::FromFront(int index) const {
	eNode<type> * iterator;
	int i;

	if (index >= nodeCount || index < 0)
		return nullptr;

	if (index == nodeCount - 1)
		return back;

	i = 0;
	iterator = front;
	while (i++ < index)
		iterator = iterator->prev;

	return iterator;
}

//******************
// eDeque::FromBack
// returns the node "index" nodes ahead of the back
// index between [ 0, Size() - 1 ]
// returns nullptr for out-of-bounds index or empty deque
//******************
template <class type>
inline eNode<type> * eDeque<type>::FromBack(int index) const {
	eNode<type> * iterator;
	int i;

	if (index >= nodeCount || index < 0)
		return nullptr;
	
	if (index == nodeCount - 1)
		return front;

	i = 0;
	iterator = back;
	while (i++ < index)
		iterator = iterator->next;

	return iterator;
}

//******************
// eDeque::Clear
// unlinks and deletes all nodes front to back
//******************
template <class type>
inline void eDeque<type>::Clear() {
	while (!IsEmpty())
		PopFront();
}

//******************
// eDeque::Size
// current number of nodes
//******************
template <class type>
inline int eDeque<type>::Size() const {
	return nodeCount;
}

//******************
// eDeque::IsEmpty
// returns true for front == nullptr
//******************
template <class type>
inline bool eDeque<type>::IsEmpty() const {
	return front == nullptr;
}

#endif /* EVIL_DEQUE_H */
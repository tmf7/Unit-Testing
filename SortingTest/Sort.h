#ifndef EVIL_SORT_H
#define EVIL_SORT_H

// global heapsort
// only works on contiguous memory
// lambdaCompare defines the closure type of a lambda function used for sort order
template<class type, class lambdaCompare>
inline void HeapSort(const type * data, const int numElements, lambdaCompare & compare) {
	int	parent;
	int	child;
	int i;

	// heapify
	for (i = numElements / 2; i > 0; i--) {
		for (parent = i - 1, child = 2 * parent + 1; child < numElements; child = 2 * parent + 1) {
			if (child + 1 < numElements && compare(data[child], data[child + 1]))
				child++;
			if (!compare(data[parent], data[child]))
				return;

			std::swap(data[parent], data[child]);
			parent = child;
		}
	}

	// sort
	for (i = numElements - 1; i > 0; i--) {
		std::swap(data[0], data[i]);
		// SiftDown using fewer elements
		for (parent = i, child = 2 * parent + 1; child < i; child = 2 * parent + 1) {
			if (child + 1 < i && compare(data[child], data[child + 1]))
				child++;
			if (!compare(data[parent], data[child]))
				break;

			std::swap(data[parent], data[child]);
			parent = child;
		}
	}
}

#endif /* EVIL_SORT_H */

#ifndef EVIL_SORT_H
#define EVIL_SORT_H

// global heapsort
// only works on contiguous memory
// lambdaCompare defines the closure type of a lambda function used for sort order
template<class type, class lambdaCompare>
void HeapSort(type * data, const int numElements, lambdaCompare & compare) {
	int	parent;
	int	child;
	int i;

	// heapify
	for (i = numElements / 2; i > 0; i--) {
		for (parent = i - 1, child = 2 * parent + 1; child < numElements; child = 2 * parent + 1) {
			if (child + 1 < numElements && compare(data[child], data[child + 1]))
				child++;
			if (!compare(data[parent], data[child]))
				break;

			std::swap(data[parent], data[child]);
			parent = child;
		}
	}

	// sort
	for (i = numElements - 1; i > 0; i--) {
		std::swap(data[0], data[i]);
		// SiftDown using fewer elements
		for (parent = 0, child = 2 * parent + 1; child < i; child = 2 * parent + 1) {
			if (child + 1 < i && compare(data[child], data[child + 1]))
				child++;
			if (!compare(data[parent], data[child]))
				break;

			std::swap(data[parent], data[child]);
			parent = child;
		}
	}

}

// global mergesort
// only works on contiguous memory
// lambdaCompare defines the closure type of a lambda function used for sort order
// breaks data into sublists of naturally occuring sorted runs
template<class type, class lambdaCompare>
void MergeSort(type * data, const int numElements, lambdaCompare & compare) {
	// FIXME/BUG: this will cause a stack overflow for too large of sets > 100000
	// SOLUTION: either distribute the allocations through the function
	// or use a std::vector<type> to offload resource management
	type *	aux			= new type[numElements];
	int	*	lowerIndex	= new int[numElements];
	int	*	upperIndex	= new int[numElements];

	int		maxIndex;
	int		i, left, right;
	int		offset, listNum;
	int		numSublists, maxSublist;
	type *	read;
	type *	write;

	if (numElements <= 1)
		return;

	// break data into sublists according to already sorted runs
	// and use the range [lowerIndex, upperIndex) in all future checks
	i = 0;
	listNum = 0;
	do {
		lowerIndex[listNum] = i;
		while (i + 1 < numElements && compare(data[i], data[i + 1])) { i++; }
		upperIndex[listNum] = ++i;
		listNum++;
	} while (i < numElements);		// DEBUG: last upperIndex == numElements

	maxSublist = listNum;

	// mergesort
	read = data;
	write = aux;
	offset = 1;
	numSublists = maxSublist;
	while (numSublists > 1) {
		for (listNum = 0; listNum < maxSublist; listNum += 2 * offset) {

			// define the bounds
			i = lowerIndex[listNum];
			left = lowerIndex[listNum];

			// account for an odd number of lists
			if (listNum + offset < maxSublist) {
				right = lowerIndex[listNum + offset];
				maxIndex = upperIndex[listNum + offset];
				numSublists--;
			} else {
				right = upperIndex[listNum];		// DEBUG: == numElements
				maxIndex = upperIndex[listNum];
			}

			while (left < upperIndex[listNum] && right < maxIndex) {
				write[i++] = compare(read[left], read[right]) 
										? std::move(read[left++]) 
										: std::move(read[right++]);
			}
			while (left < upperIndex[listNum]) { write[i++] = read[left++]; }
			while (right < maxIndex) { write[i++] = read[right++]; }
			
			// adjust the left list's bounds, and ignore the now-mergerd
			// right list bounds in future iterations (using offset)
			upperIndex[listNum] = maxIndex;	
		}
		std::swap(read, write);
		offset *= 2;
	}

	// ensure data has the sorted list
	// DEBUG: the logic of write == aux is swapped because the read and write were just swapped
	if (write == data) {
		for (i = 0; i < numElements; i++)
			data[i] = std::move(aux[i]);
	}
	delete[] aux;
	delete[] lowerIndex;
	delete[] upperIndex;
}

#endif /* EVIL_SORT_H */

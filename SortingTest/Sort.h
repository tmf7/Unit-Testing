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
	// and use [lowerIndex, upperIndex) in all future checks
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

			// merge
			while (left < upperIndex[listNum] && right < maxIndex) {
				write[i++] = compare(read[left], read[right]) 
										? std::move(read[left++]) 
										: std::move(read[right++]);
			}
			while (left < upperIndex[listNum]) { write[i++] = std::move(read[left++]); }
			while (right < maxIndex) { write[i++] = std::move(read[right++]); }
			
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

template <class type, class lambdaCompare>
void QuickSort(type * data, const int numElements, lambdaCompare & compare) {

	if (numElements <= 1)
		return;

	// NOTE: do not "recurse" more than this many times
	// to avoid some slowing/malicious data sets (eg: organ pipe)
	const int MAX_LEVELS = 128;	
	const int SELECTION_SORT_THRESHOLD = 4;		// DEBUG: larger thrsholds result in too much selection sorting
	int lowerIndex[MAX_LEVELS];
	int upperIndex[MAX_LEVELS];

	lowerIndex[0] = 0;
	upperIndex[0] = numElements - 1;
	for (int level = 0; level >= 0; /*conditional increment/decrement in loop*/) {

		int leftIndex = lowerIndex[level];
		int rightIndex = upperIndex[level];

		if (((rightIndex - leftIndex) >= SELECTION_SORT_THRESHOLD) && (level < (MAX_LEVELS - 1))) {
			// midpoint of range on each iterations
			// use a reference to avoid copying a potentially large object
			// and to give data[upperIndex[level]] a more explicit name
			int middleIndex = (leftIndex + rightIndex) / 2;
			std::swap(data[middleIndex], data[rightIndex]);				// move the pivot element to the end
			type & pivot = data[rightIndex--];							// start rightIndex immediatly to the left of the pivot

			// partition left and right sublists (may wind up uselessly swapping two duplicates of pivot value)
			do {
				// NOTE: explicitly checking <= or >= has a notable impact 
				// on speed compared to < or > alone, for large data sets
				while (compare(data[leftIndex], pivot) < 0) { if (++leftIndex >= rightIndex) break; };
				while (compare(data[rightIndex], pivot) > 0) { if(--rightIndex <= leftIndex) break; };
				if (leftIndex >= rightIndex)
					break;
				std::swap(data[leftIndex], data[rightIndex]);
			} while (++leftIndex < --rightIndex);

			// account for any further duplicates 
			// immediatly beside the current leftIndex/rightIndex
			// and dont bother sorting this "third" partition (dutch national flag)
			// rightIndex steps into strictly "less than" list, leftIndex steps into strictly "greater than" list
			// one of them will already be where it need to be unless there's duplicates
			// NOTE: rightIndex may go to 0 and leftIndex may go to the pivot's location
			// which means the pivot will swap with itself
			// but it also means that the 0-length lists are avoided on the next iteration
			while (compare(data[leftIndex], pivot) <= 0 && leftIndex < upperIndex[level]) { leftIndex++; };
			while (compare(data[rightIndex], pivot) >= 0 && rightIndex > lowerIndex[level]) { rightIndex--; };

			// put pivot in its final place
			// don't avoid comparing against it again
			// because the increment costs too much compared to 
			// a comparision, and the comparison will increment 
			// past this pivot anyway
			std::swap(pivot, data[leftIndex]);

			// reset the bounds for the adjacent lists
			// used in future iterations
//			assert(level < MAX_LEVELS - 1);
			lowerIndex[level + 1] = leftIndex;
			upperIndex[level + 1] = upperIndex[level];
			upperIndex[level] = rightIndex;			// DEBUG: rightIndex != leftIndex (which is where the perma-placed pivot just landed)
			level++;
		} else {

			// selection-sort the remaining elements
			for (/*leftIndex alreay initialized*/; rightIndex > leftIndex; rightIndex--) {
				int l = leftIndex;
				for (int r = leftIndex + 1; r <= rightIndex; r++) {
					if (compare(data[l], data[r]) < 0) {
						l = r;
					}
				}
				std::swap(data[l], data[rightIndex]);
			}
			level--;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
template <class type, class lambdaCompare>
void QuickSort_Insert(type * data, const int numElements, lambdaCompare & compare) {

	if (numElements <= 1)
		return;

	// NOTE: do not "recurse" more than this many times
	// to avoid some slowing/malicious data sets (eg: organ pipe)
	const int MAX_LEVELS = 128;	
	const int SELECTION_SORT_THRESHOLD = 4;		// DEBUG: larger thrsholds result in too much selection sorting
	int lowerIndex[MAX_LEVELS];
	int upperIndex[MAX_LEVELS];

	lowerIndex[0] = 0;
	upperIndex[0] = numElements - 1;
	for (int level = 0; level >= 0; /*conditional increment/decrement in loop*/) {

		int leftIndex = lowerIndex[level];
		int rightIndex = upperIndex[level];

		if (((rightIndex - leftIndex) >= SELECTION_SORT_THRESHOLD) && (level < (MAX_LEVELS - 1))) {
			// midpoint of range on each iterations
			// use a reference to avoid copying a potentially large object
			// and to give data[upperIndex[level]] a more explicit name
			int middleIndex = (leftIndex + rightIndex) / 2;
			std::swap(data[middleIndex], data[rightIndex]);				// move the pivot element to the end
			type & pivot = data[rightIndex--];							// start rightIndex immediatly to the left of the pivot

			// partition left and right sublists (may wind up uselessly swapping two duplicates of pivot value)
			do {
				// NOTE: explicitly checking <= or >= has a notable impact 
				// on speed compared to < or > alone, for large data sets
				while (compare(data[leftIndex], pivot) < 0) { if (++leftIndex >= rightIndex) break; };
				while (compare(data[rightIndex], pivot) > 0) { if(--rightIndex <= leftIndex) break; };
				if (leftIndex >= rightIndex)
					break;
				std::swap(data[leftIndex], data[rightIndex]);
			} while (++leftIndex < --rightIndex);

			// account for any further duplicates 
			// immediatly beside the current leftIndex/rightIndex
			// and dont bother sorting this "third" partition (dutch national flag)
			// rightIndex steps into strictly "less than" list, leftIndex steps into strictly "greater than" list
			// one of them will already be where it need to be unless there's duplicates
			// NOTE: rightIndex may go to 0 and leftIndex may go to the pivot's location
			// which means the pivot will swap with itself
			// but it also means that the 0-length lists are avoided on the next iteration
			while (compare(data[leftIndex], pivot) <= 0 && leftIndex < upperIndex[level]) { leftIndex++; };
			while (compare(data[rightIndex], pivot) >= 0 && rightIndex > lowerIndex[level]) { rightIndex--; };

			// put pivot in its final place
			// don't avoid comparing against it again
			// because the increment costs too much compared to 
			// a comparision, and the comparison will increment 
			// past this pivot anyway
			std::swap(pivot, data[leftIndex]);

			// reset the bounds for the adjacent lists
			// used in future iterations
//			assert(level < MAX_LEVELS - 1);
			lowerIndex[level + 1] = leftIndex;
			upperIndex[level + 1] = upperIndex[level];
			upperIndex[level] = rightIndex;			// DEBUG: rightIndex != leftIndex (which is where the perma-placed pivot just landed)
			level++;
		} else {

			// insertion-sort the remaining elements
			for (int current = leftIndex + 1; current <= rightIndex; current++) {
				type temp = std::move(data[current]);
				int checkLeft = current - 1;
				while (checkLeft >= leftIndex && compare(data[checkLeft], temp) >= 0) {
					data[checkLeft + 1] = std::move(data[checkLeft]);
					checkLeft--;
				}
				data[checkLeft + 1] = std::move(temp);
			}
			level--;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////

template <class type, class lambdaCompare>
void SelectionSort(type * data, const int numElements, lambdaCompare & compare) {

	for (int i = 0; i < numElements; i++) {
		int left = i;
		for (int right = i + 1; right < numElements; right++) {
			if (compare(data[left], data[right])) {
				left = right;
			}
		}
		std::swap(std::move(data[i]), std::move(data[left]));
	}
}

// insertion sort will do fewer comparisons 
// than selection sort before item gets to its final spot,
// but that's irrelevant for very small lists
// insertion sort will also do more (potentially large) object copies/moves
// as opposed to selection sort which will just find the right int index and swap once
template <class type, class lambdaCompare>
void InsertionSort(type * data, const int numElements, lambdaCompare & compare) {

	// do a run of overwrites to the right
	// until temp has reached its correct position
	for (int current = 0; current < numElements; current++) {
		type temp = std::move(data[current]);
		int left = current - 1;
		while (left >= 0 && compare(data[left], temp)) {
			data[left + 1] = std::move(data[left]);
			left--;
		}
		data[left + 1] = std::move(temp);
	}
}

#endif /* EVIL_SORT_H */

#ifndef EVIL_HASH_INDEX_H_
#define EVIL_HASH_INDEX_H_

#include <vector>

//************************************
//			HashIndex
//	Fast hash table for indexes and arrays
//	uses std::vector<int> for memory management
//************************************
class eHashIndex {
public:
					
						eHashIndex();
	explicit			eHashIndex(int initialHashSize);

						template <class ForwardIterator, class HashFunction>
						eHashIndex(ForwardIterator begin, ForwardIterator end, HashFunction hasher);

						template <class ForwardIterator, class HashFunction>
	void				Rebuild(ForwardIterator begin, ForwardIterator end, HashFunction hasher);

	void				Add(const int hashkey, const int index);
	void				Remove(const int hashkey, const int index);
	int					First(const int hashkey) const;
	int					Next(const int index) const;

	void				InsertIndex(const int hashkey, const int index);
	void				RemoveIndex(const int hashkey, const int index);
	void				Clear();
	void				ClearAndResize(int newHashSize);
	int					NumUniqueKeys() const;
	int					NumDuplicateKeys() const;
	int					GetSpread() const;

	// slots allocated by std::vector<int>
	size_t				HashCapacity() const;
	size_t				IndexCapacity() const;

private:

	std::vector<int>	hash;
	std::vector<int>	indexChain;
	int					hashMask;

	static const int	defaultHashSize = 1024;
	static const int	INVALID_INDEX = -1;
};

//*******************
// eHashIndex::eHashIndex
//*******************
inline eHashIndex::eHashIndex() : eHashIndex(defaultHashSize) {
}

//*******************
// eHashIndex::eHashIndex
// initialHashSize will resize to the next closest power of 2
// to improve hash key spread
//*******************
inline eHashIndex::eHashIndex(int initialHashSize) {
	int power = 0;
	while (initialHashSize >> ++power)
		;
	initialHashSize = 1 << power;

	hash.reserve(initialHashSize);
	indexChain.reserve(initialHashSize);
	hash.assign(initialHashSize, INVALID_INDEX);
	indexChain.assign(initialHashSize, INVALID_INDEX);
	hashMask = hash.size() - 1;
}

//*******************
// eHashIndex::eHashIndex
// build using the source array
// ensuring each index is registered once
//*******************
template <class ForwardIterator, class HashFunction>
inline eHashIndex::eHashIndex(ForwardIterator begin, ForwardIterator end, HashFunction hasher) : eHashIndex(std::distance(begin, end)) {
	for (ForwardIterator i = begin; i != end; i++)
		Add(hasher(*i), std::distance(begin, i));
}

//*******************
// eHashIndex::Rebuild
// clear, resize, and build using the source array
// ensuring each index is registered once
//*******************
template <class ForwardIterator, class HashFunction>
inline void eHashIndex::Rebuild(ForwardIterator begin, ForwardIterator end, HashFunction hasher) {
	ClearAndResize(std::distance(begin, end));
	for (ForwardIterator i = begin; i != end; i++)
		Add(hasher(*i), std::distance(begin, i));
}

//*******************
// eHashIndex::Add
// add an index to the hash
// --only add unique indexes--
// sets this most recently added index as the First() one to be viewed
// DEBUG: assert (index >= 0)
//*******************
inline void eHashIndex::Add(const int hashkey, const int index) {
	if (index >= indexChain.size())		// DEBUG: std::vector may allocate more than max-signed-int values, but not for my purposes
		indexChain.resize(index + 1);	// DEBUG: indexChain.size() need not be a power of 2, it doesn't affect hashkey spread

	int k = hashkey & hashMask;
	indexChain[index] = hash[k];
	hash[k] = index;
}

//*******************
// eHashIndex::Remove
// remove an index from the hash
// --ensure the input key/index pair exists (even if a duplicate)--
// DEBUG: assert( index >= 0 && index < indexChain.size() )
//*******************
inline void eHashIndex::Remove(const int hashkey, const int index) {
	int k;

	if (hash.empty())
		return;
	
	k = hashkey & hashMask;
	if (hash[k] == index) {
		hash[k] = indexChain[index];
	} else {
		for (int i = hash[k]; i != INVALID_INDEX; i = indexChain[i]) {
			if (indexChain[i] == index) {
				indexChain[i] = indexChain[index];
				break;
			}
		}
	}
	indexChain[index] = INVALID_INDEX;
}

//*******************
// eHashIndex::First
// get the first index from the hash, returns -1 if empty hash entry
//*******************
inline int eHashIndex::First(const int hashkey) const {
	return hash[hashkey & hashMask];
}

//*******************
// eHashIndex::Next
// get the next index with the same hashkey, returns -1 if at the end of a chain
// DEBUG: assert( index >= 0 && index < indexSize )
//*******************
inline int eHashIndex::Next(const int index) const {
	return indexChain[index];
}


//*******************
// eHashIndex::Insertindex
// insert an new index into the indexChain and add it to the hash, increasing all indexes >= index
// useful for dynamically-sized array indexing
// --ensure the source array has actually resized by one, making the key/index pair valid--
// DEBUG: assert( index >= 0 )
//*******************
inline void eHashIndex::InsertIndex(const int hashkey, const int index) {
	int max = index;
	for (size_t i = 0; i < hash.size(); i++) {
		if (hash[i] >= index) {
			hash[i]++;
			if (hash[i] > max) {
				max = hash[i];
			}
		}
	}
	for (size_t i = 0; i < indexChain.size(); i++) {
		if (indexChain[i] >= index) {
			indexChain[i]++;
			if (indexChain[i] > max) {
				max = indexChain[i];
			}
		}
	}
	if (max >= indexChain.size()) {
		indexChain.resize(max + 1);		// DEBUG: indexChain.size() need not be a power of 2, it doesn't affect hashkey spread
	}
	for (int i = max; i > index; i--) {
		indexChain[i] = indexChain[i - 1];
	}
	indexChain[index] = INVALID_INDEX;
	Add(hashkey, index);
}

//*******************
// eHashIndex::RemoveIndex
// remove an index from the indexChain and remove it from the hash, decreasing all indexes >= index
// useful for dynamically-sized array indexing 
// ensure the target index is actually removed from the source array first
// DEBUG: assert( index >= 0 && index < indexChain.size() )
//*******************
inline void eHashIndex::RemoveIndex(const int hashkey, const int index) {
	Remove(hashkey, index);
	int max = index;
	for (size_t i = 0; i < hash.size(); i++) {
		if (hash[i] >= index) {
			if (hash[i] > max) {
				max = hash[i];
			}
			hash[i]--;
		}
	}
	for (size_t i = 0; i < indexChain.size(); i++) {
		if (indexChain[i] >= index) {
			if (indexChain[i] > max) {
				max = indexChain[i];
			}
			indexChain[i]--;
		}
	}
	for (int i = index; i < max; i++) {
		indexChain[i] = indexChain[i + 1];
	}
	indexChain[max] = INVALID_INDEX;
}

//*******************
// eHashIndex::Clear
//*******************
inline void eHashIndex::Clear() {
	hash.assign(hash.capacity(), INVALID_INDEX);
	indexChain.assign(hash.capacity(), INVALID_INDEX);
}

//*******************
// eHashIndex::ClearAndResize
// newHashSize will resize to the next closest power of 2
// to improve hash key spread
//*******************
inline void eHashIndex::ClearAndResize(int newHashSize) {
	int power = 0;
	while (newHashSize >> ++power)
		;
	newHashSize = 1 << power;

	hash.resize(newHashSize);
	indexChain.resize(newHashSize);
	hash.assign(hash.capacity(), INVALID_INDEX);
	indexChain.assign(hash.capacity(), INVALID_INDEX);
	hashMask = hash.size() - 1;
}

//*******************
// eHashIndex::NumUniqueKeys
// returns the number of unique keys in use
// DEBUG: these are stored in the primary hash vector
//*******************
inline int eHashIndex::NumUniqueKeys() const {
	int uniqueCount = 0;
	for (size_t i = 0; i < hash.size(); i++) {
		if (hash[i] != -1)
			uniqueCount++;
	}
	return uniqueCount;
}

//*******************
// eHashIndex::NumDuplicateKeys
// returns the current number of indexes with duplicate keys
// DEBUG: these are stored in the indexChain vector
//*******************
inline int eHashIndex::NumDuplicateKeys() const {
	int duplicateCount = 0;
	for (size_t i = 0; i < indexChain.size(); i++) {
		if (indexChain[i] != -1)
			duplicateCount++;
	}
	return duplicateCount;
}

//*******************
// eHashIndex::HashCapacity
//*******************
inline size_t eHashIndex::HashCapacity() const {
	return hash.capacity();
}

//*******************
// eHashIndex::IndexCapacity
//*******************
inline size_t eHashIndex::IndexCapacity() const {
	return indexChain.capacity();
}

//*******************
// eHashIndex::GetSpread
// returns int in the range [0-100] representing the spread over the hash table
// TODO: move this out of the header
//*******************
inline int eHashIndex::GetSpread() const {
	int i, index, totalItems;
	std::vector<int> numHashItems;
	int average, error, e;

	const int hashSize = hash.size();

	if (NumUniqueKeys() == 0) {
		return 100;
	}

	totalItems = 0;
	numHashItems.reserve(hashSize);
	numHashItems.assign(hashSize, 0);
	for (i = 0; i < hashSize; i++) {
		for (index = hash[i]; index != INVALID_INDEX; index = indexChain[index]) {
			numHashItems[i]++;
		}
		totalItems += numHashItems[i];
	}
	// if no items in hash
	if (totalItems <= 1) {
		return 100;
	}
	average = totalItems / hashSize;
	error = 0;
	for (i = 0; i < hashSize; i++) {
		e = abs(numHashItems[i] - average);
		if (e > 1) {
			error += e - 1;
		}
	}
	return 100 - (error * 100 / totalItems);
}

#endif /* EVIL_HASH_INDEX_H_ */
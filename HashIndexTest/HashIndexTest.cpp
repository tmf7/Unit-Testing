#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <chrono>
#include <memory>
#include <string>
#include "HashIndex.h"
#define UPTR(x) std::make_unique<int>(x)

// unit test with int, std::string, std::unique_ptr<int> using both c-style array and dynamic array (std::vector<int>)
// passed all tests. (future project: HashTable using pointers)
int main() {
	const int numElements = 15;
	int ab[numElements] = { 4, 1, 7, 2, 1, 2, 2, 7, -1, 12, 566, 2, 567, 89, 2 };
//	int a[numElements] = { 0, 1, 2, 2, 4, 5, 6, 7 };	// spread test
	std::vector<int> a{ std::cbegin(ab), std::cend(ab) };
/*
	std::string a[numElements] = {
		"one",
		"two",
		"three four five six seven eight nine ten",
		"little test\n",
		"on asdfli3 __0",
		"asdfk337",
		"123",
		"two",
		"tow",
		"one",
		"owt",
		"__.__",
		"hello world\n",
		"how are your\n",
		"little test\n"
	};

	std::unique_ptr<int> a[numElements] = { UPTR(4), UPTR(1), UPTR(7), UPTR(2), UPTR(8), UPTR(2), 
											UPTR(2), UPTR(7), UPTR(-1), UPTR(3), UPTR(566), 
											UPTR(2), UPTR(567), UPTR(89), UPTR(2) };
*/
	eHashIndex hash;	// default constructed size of 8: works, initialHashSize(32) works

	auto t1 = std::chrono::high_resolution_clock::now();

	// generate random keys and put them into array "a"
	// then add them to the eHashIndex
	for (int i = 0; i < numElements; i++)
		hash.Add(hash.GenerateKey(a[i]), i);

	// duplicate key/index test (in-bounds, with same value): produces an inf-loop on Remove()
	// wrong key/index test (in-bounds, with different array value): no inf-loop, but may skip avoid correct key/index pairs
	// out-of-bounds key/index test: undefined behavior (indexes beyond the viewed array)
	// tested checking for pre-registered index, and it slowed release-build runtime by 6 ms on 65536 indexes
	// conclusion: be responsible about adding indexes to an eHashIndex (only uniques)
//	hash.Add(hash.GenerateKey(2), 6);

	// TODO: test insertIndex/removeIndex

	eHashIndex hash_a = hash;				// deault copy construction: worked
//	eHashIndex hash_a = std::move(hash);	// default move construction: worked

	// search for pre-existing value: worked
	// count how many with similar keys there are: worked
	const int target = 2;	// this is what's sought
	int iter = 0, found = 0;
	int key = hash_a.GenerateKey(target);	

	// removing an invalid key/index pair will corrupt the eHashIndex 
	// (ie unable to find some indexes because indexChain is broken)
	// conclusion: be responsible about index removal
//	hash_a.Remove(key, 14);
//	hash_a.Remove(key, 7);	

	a.insert(a.begin() + 2, 777);
	hash_a.InsertIndex(hash_a.GenerateKey(a[2]), 2);	// insertIndex for dynamic array: worked

	a.erase(a.begin() + 4);
	hash_a.RemoveIndex(hash_a.GenerateKey(a[4]), 4);	// removeIndex for dynamic array: worked

	for (int i = hash_a.First(key); i != -1; i = hash_a.Next(i)) {
		iter++;
		if (a[i] == target) {
			printf("Found: %i\n", a[i]);
			found++;
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	printf("%I64i milliseconds\n\n", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
	printf("%i of them.\nIn %i iterations.\n\n", found, iter);
	printf("There are %i unique keys\n", hash_a.NumUniqueKeys());
	printf("hash capacity: %i.\n\n", hash_a.HashCapacity());

	printf("indexChain holds %i duplicate keys.\n", hash_a.NumDuplicateKeys());
	printf("indexChain capacity: %i.\n\n", hash_a.IndexCapacity());
	
	printf("Spread: %i\n", hash_a.GetSpread());
	return 0;
}

#include <stdlib.h>
#include "bitArray.h"

bitBlock_t *btArAlloc(long bitCount) {
	int blockCount = bitCount/bitsPerBitBlock;
	int minBlockCount = sizeof(int)/sizeof(bitBlock_t); // because popcount looks at int
	if (blockCount < minBlockCount) blockCount = minBlockCount;
	return calloc(blockCount, sizeof(bitBlock_t));
}
bool btArRead(bitBlock_t *array, long index) {
	return (array[index/bitsPerBitBlock] >> index%bitsPerBitBlock) & 1;
}
void btArSet1(bitBlock_t *array, long index) {
	array[index/bitsPerBitBlock] |= 1 << index%bitsPerBitBlock;
}
void btArSet0(bitBlock_t *array, long index) {
	array[index/bitsPerBitBlock] |= ~(1 << index%bitsPerBitBlock);
}
void btArTogl(bitBlock_t *array, long index) {
	array[index/bitsPerBitBlock] ^= 1 << index%bitsPerBitBlock;
}

long btArCountSet(bitBlock_t *array, long bitCount) {
	long setCount = 0;
	int *intBlocks = (int*)array;
	const int bitsPerInt = sizeof(int)*8;
	for (int i = 0; i*bitsPerInt < bitCount+bitsPerInt; i++) {
		setCount += __builtin_popcount(intBlocks[i]);
	}
	return setCount;
}

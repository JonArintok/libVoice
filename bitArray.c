
#include <stdlib.h>
#include "bitArray.h"
#include "misc.h"

bitBlock_t *btArAlloc(long bitCount) {
	int blockCount = bitCount/bitsPerBitBlock + 1;
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
	long total = 0;
	fr (b, bitCount) total += btArRead(array, b);
	return total;
}

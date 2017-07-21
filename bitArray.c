
#include <stdlib.h>
#include "bitArray.h"

bitBlock_t *btArAlloc(long bitCount) {
	return malloc(bitCount/bitsPerByte);
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

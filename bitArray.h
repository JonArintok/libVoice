
#include <stdint.h>
#include <stdbool.h>

#define bitBlock_t uint8_t
#define bitsPerBitBlock 8

bitBlock_t *btArAlloc(long bitCount); // free with regular free()
bool btArRead(bitBlock_t *array, long index);
void btArSet1(bitBlock_t *array, long index);
void btArSet0(bitBlock_t *array, long index);
void btArTogl(bitBlock_t *array, long index);
long btArCountSet(bitBlock_t *array, long bitCount);

#include <stdbool.h>

#define fr(i, bound) for (int i = 0; i < (bound); i++)

bool allEq(const float *l, const float *r, int c);

#define tau 6.28318530717958647692528676655900576839433879875
double sinTau(double n);
double fractionalPart(double n);
double lerp(double l, double r, double n);

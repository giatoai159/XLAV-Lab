#include "lib.h"

uchar min(uchar a, uchar b)
{

	if ((int)a < (int)b)
		return a;
	return b;
}

uchar min3var(uchar a, uchar b, uchar c)
{
	return (min(min(a, b), c));
}

uchar max(uchar a, uchar b)
{
	if ((int)a > (int)b)
		return a;
	return b;
}

uchar max3var(uchar a, uchar b, uchar c)
{
	return (max(max(a, b), c));
}

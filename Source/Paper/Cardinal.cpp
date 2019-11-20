#include "Cardinal.h"

void FCardinal::Set(unsigned char d, bool b)
{
	Booleans[d] = b;
}

bool FCardinal::Get(unsigned char d)
{
	return Booleans[d];
}

FCardinal::FCardinal()
{
	Booleans.Init(false, 4);
}
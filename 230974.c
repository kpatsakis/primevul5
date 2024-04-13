static u64 __scale_tsc(u64 ratio, u64 tsc)
{
	u64 mult, frac, _tsc;

	mult  = ratio >> 32;
	frac  = ratio & ((1ULL << 32) - 1);

	_tsc  = tsc;
	_tsc *= mult;
	_tsc += (tsc >> 32) * frac;
	_tsc += ((tsc & ((1ULL << 32) - 1)) * frac) >> 32;

	return _tsc;
}
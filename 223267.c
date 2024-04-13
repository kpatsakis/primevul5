static inline int pri2fac(const int pri)
{
	unsigned fac = pri >> 3;
	return (fac > 23) ? LOG_FAC_INVLD : fac;
}
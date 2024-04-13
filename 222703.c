static inline unsigned long nonhyp_mask(int i)
{
	unsigned int nonhyp_fai = (sclp.hmfai << i * 2) >> 30;

	return 0x0000ffffffffffffUL >> (nonhyp_fai << 4);
}
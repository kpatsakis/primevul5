combine_surrogate_pair(uint32_t uc, uint32_t uc2)
{
	uc -= 0xD800;
	uc *= 0x400;
	uc += uc2 - 0xDC00;
	uc += 0x10000;
	return (uc);
}
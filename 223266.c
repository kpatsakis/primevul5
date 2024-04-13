static inline int pri2sev(const int pri)
{
	return pri & 0x07;
}
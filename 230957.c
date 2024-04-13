static inline u64 get_canonical(u64 la)
{
	return ((int64_t)la << 16) >> 16;
}
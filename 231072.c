static inline bool is_noncanonical_address(u64 la)
{
#ifdef CONFIG_X86_64
	return get_canonical(la) != la;
#else
	return false;
#endif
}
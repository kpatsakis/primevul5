static inline bool kvm_is_error_hva(unsigned long addr)
{
	return addr >= PAGE_OFFSET;
}
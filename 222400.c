static inline bool is_error_pfn(kvm_pfn_t pfn)
{
	return !!(pfn & KVM_PFN_ERR_MASK);
}
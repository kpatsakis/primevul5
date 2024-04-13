static inline bool is_error_noslot_pfn(kvm_pfn_t pfn)
{
	return !!(pfn & KVM_PFN_ERR_NOSLOT_MASK);
}
static inline bool is_noslot_pfn(kvm_pfn_t pfn)
{
	return pfn == KVM_PFN_NOSLOT;
}
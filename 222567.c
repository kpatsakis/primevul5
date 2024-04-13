static inline bool kvm_is_error_gpa(struct kvm *kvm, gpa_t gpa)
{
	unsigned long hva = gfn_to_hva(kvm, gpa_to_gfn(gpa));

	return kvm_is_error_hva(hva);
}
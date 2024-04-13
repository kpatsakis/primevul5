static inline struct page *kvm_vcpu_gpa_to_page(struct kvm_vcpu *vcpu,
						gpa_t gpa)
{
	return kvm_vcpu_gfn_to_page(vcpu, gpa_to_gfn(gpa));
}
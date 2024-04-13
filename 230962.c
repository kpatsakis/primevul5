static inline gpa_t translate_gpa(struct kvm_vcpu *vcpu, gpa_t gpa, u32 access,
				  struct x86_exception *exception)
{
	return gpa;
}
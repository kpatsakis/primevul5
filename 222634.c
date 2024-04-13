static inline struct kvm_memslots *kvm_vcpu_memslots(struct kvm_vcpu *vcpu)
{
	int as_id = kvm_arch_vcpu_memslots_id(vcpu);

	return __kvm_memslots(vcpu->kvm, as_id);
}
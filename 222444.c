static inline int kvm_arch_flush_remote_tlb(struct kvm *kvm)
{
	return -ENOTSUPP;
}
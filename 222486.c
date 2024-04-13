static inline void kvm_arch_free_vm(struct kvm *kvm)
{
	kfree(kvm);
}
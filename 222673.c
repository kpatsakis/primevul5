static inline struct kvm_vcpu *kvm_get_vcpu(struct kvm *kvm, int i)
{
	int num_vcpus = atomic_read(&kvm->online_vcpus);
	i = array_index_nospec(i, num_vcpus);

	/* Pairs with smp_wmb() in kvm_vm_ioctl_create_vcpu.  */
	smp_rmb();
	return kvm->vcpus[i];
}
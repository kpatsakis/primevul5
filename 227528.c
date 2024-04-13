static int kvm_vm_ioctl_reset_dirty_pages(struct kvm *kvm)
{
	int i;
	struct kvm_vcpu *vcpu;
	int cleared = 0;

	if (!kvm->dirty_ring_size)
		return -EINVAL;

	mutex_lock(&kvm->slots_lock);

	kvm_for_each_vcpu(i, vcpu, kvm)
		cleared += kvm_dirty_ring_reset(vcpu->kvm, &vcpu->dirty_ring);

	mutex_unlock(&kvm->slots_lock);

	if (cleared)
		kvm_flush_remote_tlbs(kvm);

	return cleared;
}
int kvm_arch_vcpu_precreate(struct kvm *kvm, unsigned int id)
{
	if (!kvm_is_ucontrol(kvm) && !sca_can_add_vcpu(kvm, id))
		return -EINVAL;
	return 0;
}
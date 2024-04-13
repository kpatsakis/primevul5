static struct kvm_vcpu *get_vcpu_by_vpidx(struct kvm *kvm, u32 vpidx)
{
	struct kvm_vcpu *vcpu = NULL;
	int i;

	if (vpidx >= KVM_MAX_VCPUS)
		return NULL;

	vcpu = kvm_get_vcpu(kvm, vpidx);
	if (vcpu && kvm_hv_get_vpindex(vcpu) == vpidx)
		return vcpu;
	kvm_for_each_vcpu(i, vcpu, kvm)
		if (kvm_hv_get_vpindex(vcpu) == vpidx)
			return vcpu;
	return NULL;
}
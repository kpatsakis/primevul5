static int kvm_s390_cpus_to_pv(struct kvm *kvm, u16 *rc, u16 *rrc)
{
	int i, r = 0;
	u16 dummy;

	struct kvm_vcpu *vcpu;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		mutex_lock(&vcpu->mutex);
		r = kvm_s390_pv_create_cpu(vcpu, rc, rrc);
		mutex_unlock(&vcpu->mutex);
		if (r)
			break;
	}
	if (r)
		kvm_s390_cpus_from_pv(kvm, &dummy, &dummy);
	return r;
}
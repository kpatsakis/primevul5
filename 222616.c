static int sca_can_add_vcpu(struct kvm *kvm, unsigned int id)
{
	int rc;

	if (!kvm_s390_use_sca_entries()) {
		if (id < KVM_MAX_VCPUS)
			return true;
		return false;
	}
	if (id < KVM_S390_BSCA_CPU_SLOTS)
		return true;
	if (!sclp.has_esca || !sclp.has_64bscao)
		return false;

	mutex_lock(&kvm->lock);
	rc = kvm->arch.use_esca ? 0 : sca_switch_to_extended(kvm);
	mutex_unlock(&kvm->lock);

	return rc == 0 && id < KVM_S390_ESCA_CPU_SLOTS;
}
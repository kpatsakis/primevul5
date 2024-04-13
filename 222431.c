static void sca_del_vcpu(struct kvm_vcpu *vcpu)
{
	if (!kvm_s390_use_sca_entries())
		return;
	read_lock(&vcpu->kvm->arch.sca_lock);
	if (vcpu->kvm->arch.use_esca) {
		struct esca_block *sca = vcpu->kvm->arch.sca;

		clear_bit_inv(vcpu->vcpu_id, (unsigned long *) sca->mcn);
		sca->cpu[vcpu->vcpu_id].sda = 0;
	} else {
		struct bsca_block *sca = vcpu->kvm->arch.sca;

		clear_bit_inv(vcpu->vcpu_id, (unsigned long *) &sca->mcn);
		sca->cpu[vcpu->vcpu_id].sda = 0;
	}
	read_unlock(&vcpu->kvm->arch.sca_lock);
}
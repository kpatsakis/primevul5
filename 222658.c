static void sca_add_vcpu(struct kvm_vcpu *vcpu)
{
	if (!kvm_s390_use_sca_entries()) {
		struct bsca_block *sca = vcpu->kvm->arch.sca;

		/* we still need the basic sca for the ipte control */
		vcpu->arch.sie_block->scaoh = (__u32)(((__u64)sca) >> 32);
		vcpu->arch.sie_block->scaol = (__u32)(__u64)sca;
		return;
	}
	read_lock(&vcpu->kvm->arch.sca_lock);
	if (vcpu->kvm->arch.use_esca) {
		struct esca_block *sca = vcpu->kvm->arch.sca;

		sca->cpu[vcpu->vcpu_id].sda = (__u64) vcpu->arch.sie_block;
		vcpu->arch.sie_block->scaoh = (__u32)(((__u64)sca) >> 32);
		vcpu->arch.sie_block->scaol = (__u32)(__u64)sca & ~0x3fU;
		vcpu->arch.sie_block->ecb2 |= ECB2_ESCA;
		set_bit_inv(vcpu->vcpu_id, (unsigned long *) sca->mcn);
	} else {
		struct bsca_block *sca = vcpu->kvm->arch.sca;

		sca->cpu[vcpu->vcpu_id].sda = (__u64) vcpu->arch.sie_block;
		vcpu->arch.sie_block->scaoh = (__u32)(((__u64)sca) >> 32);
		vcpu->arch.sie_block->scaol = (__u32)(__u64)sca;
		set_bit_inv(vcpu->vcpu_id, (unsigned long *) &sca->mcn);
	}
	read_unlock(&vcpu->kvm->arch.sca_lock);
}
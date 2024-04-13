static int sca_switch_to_extended(struct kvm *kvm)
{
	struct bsca_block *old_sca = kvm->arch.sca;
	struct esca_block *new_sca;
	struct kvm_vcpu *vcpu;
	unsigned int vcpu_idx;
	u32 scaol, scaoh;

	if (kvm->arch.use_esca)
		return 0;

	new_sca = alloc_pages_exact(sizeof(*new_sca), GFP_KERNEL|__GFP_ZERO);
	if (!new_sca)
		return -ENOMEM;

	scaoh = (u32)((u64)(new_sca) >> 32);
	scaol = (u32)(u64)(new_sca) & ~0x3fU;

	kvm_s390_vcpu_block_all(kvm);
	write_lock(&kvm->arch.sca_lock);

	sca_copy_b_to_e(new_sca, old_sca);

	kvm_for_each_vcpu(vcpu_idx, vcpu, kvm) {
		vcpu->arch.sie_block->scaoh = scaoh;
		vcpu->arch.sie_block->scaol = scaol;
		vcpu->arch.sie_block->ecb2 |= ECB2_ESCA;
	}
	kvm->arch.sca = new_sca;
	kvm->arch.use_esca = 1;

	write_unlock(&kvm->arch.sca_lock);
	kvm_s390_vcpu_unblock_all(kvm);

	free_page((unsigned long)old_sca);

	VM_EVENT(kvm, 2, "Switched to ESCA (0x%pK -> 0x%pK)",
		 old_sca, kvm->arch.sca);
	return 0;
}
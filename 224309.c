static int nested_svm_load_cr3(struct kvm_vcpu *vcpu, unsigned long cr3,
			       bool nested_npt)
{
	if (cr3 & rsvd_bits(cpuid_maxphyaddr(vcpu), 63))
		return -EINVAL;

	if (!nested_npt && is_pae_paging(vcpu) &&
	    (cr3 != kvm_read_cr3(vcpu) || pdptrs_changed(vcpu))) {
		if (!load_pdptrs(vcpu, vcpu->arch.walk_mmu, cr3))
			return -EINVAL;
	}

	/*
	 * TODO: optimize unconditional TLB flush/MMU sync here and in
	 * kvm_init_shadow_npt_mmu().
	 */
	if (!nested_npt)
		kvm_mmu_new_pgd(vcpu, cr3, false, false);

	vcpu->arch.cr3 = cr3;
	kvm_register_mark_available(vcpu, VCPU_EXREG_CR3);

	kvm_init_mmu(vcpu, false);

	return 0;
}
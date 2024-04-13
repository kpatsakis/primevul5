static int kvm_arch_setup_async_pf(struct kvm_vcpu *vcpu)
{
	hva_t hva;
	struct kvm_arch_async_pf arch;
	int rc;

	if (vcpu->arch.pfault_token == KVM_S390_PFAULT_TOKEN_INVALID)
		return 0;
	if ((vcpu->arch.sie_block->gpsw.mask & vcpu->arch.pfault_select) !=
	    vcpu->arch.pfault_compare)
		return 0;
	if (psw_extint_disabled(vcpu))
		return 0;
	if (kvm_s390_vcpu_has_irq(vcpu, 0))
		return 0;
	if (!(vcpu->arch.sie_block->gcr[0] & CR0_SERVICE_SIGNAL_SUBMASK))
		return 0;
	if (!vcpu->arch.gmap->pfault_enabled)
		return 0;

	hva = gfn_to_hva(vcpu->kvm, gpa_to_gfn(current->thread.gmap_addr));
	hva += current->thread.gmap_addr & ~PAGE_MASK;
	if (read_guest_real(vcpu, vcpu->arch.pfault_token, &arch.pfault_token, 8))
		return 0;

	rc = kvm_setup_async_pf(vcpu, current->thread.gmap_addr, hva, &arch);
	return rc;
}
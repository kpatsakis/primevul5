static int vcpu_post_run(struct kvm_vcpu *vcpu, int exit_reason)
{
	struct mcck_volatile_info *mcck_info;
	struct sie_page *sie_page;

	VCPU_EVENT(vcpu, 6, "exit sie icptcode %d",
		   vcpu->arch.sie_block->icptcode);
	trace_kvm_s390_sie_exit(vcpu, vcpu->arch.sie_block->icptcode);

	if (guestdbg_enabled(vcpu))
		kvm_s390_restore_guest_per_regs(vcpu);

	vcpu->run->s.regs.gprs[14] = vcpu->arch.sie_block->gg14;
	vcpu->run->s.regs.gprs[15] = vcpu->arch.sie_block->gg15;

	if (exit_reason == -EINTR) {
		VCPU_EVENT(vcpu, 3, "%s", "machine check");
		sie_page = container_of(vcpu->arch.sie_block,
					struct sie_page, sie_block);
		mcck_info = &sie_page->mcck_info;
		kvm_s390_reinject_machine_check(vcpu, mcck_info);
		return 0;
	}

	if (vcpu->arch.sie_block->icptcode > 0) {
		int rc = kvm_handle_sie_intercept(vcpu);

		if (rc != -EOPNOTSUPP)
			return rc;
		vcpu->run->exit_reason = KVM_EXIT_S390_SIEIC;
		vcpu->run->s390_sieic.icptcode = vcpu->arch.sie_block->icptcode;
		vcpu->run->s390_sieic.ipa = vcpu->arch.sie_block->ipa;
		vcpu->run->s390_sieic.ipb = vcpu->arch.sie_block->ipb;
		return -EREMOTE;
	} else if (exit_reason != -EFAULT) {
		vcpu->stat.exit_null++;
		return 0;
	} else if (kvm_is_ucontrol(vcpu->kvm)) {
		vcpu->run->exit_reason = KVM_EXIT_S390_UCONTROL;
		vcpu->run->s390_ucontrol.trans_exc_code =
						current->thread.gmap_addr;
		vcpu->run->s390_ucontrol.pgm_code = 0x10;
		return -EREMOTE;
	} else if (current->thread.gmap_pfault) {
		trace_kvm_s390_major_guest_pfault(vcpu);
		current->thread.gmap_pfault = 0;
		if (kvm_arch_setup_async_pf(vcpu))
			return 0;
		return kvm_arch_fault_in_page(vcpu, current->thread.gmap_addr, 1);
	}
	return vcpu_post_run_fault_in_sie(vcpu);
}
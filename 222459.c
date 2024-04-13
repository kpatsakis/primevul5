static int vcpu_pre_run(struct kvm_vcpu *vcpu)
{
	int rc, cpuflags;

	/*
	 * On s390 notifications for arriving pages will be delivered directly
	 * to the guest but the house keeping for completed pfaults is
	 * handled outside the worker.
	 */
	kvm_check_async_pf_completion(vcpu);

	vcpu->arch.sie_block->gg14 = vcpu->run->s.regs.gprs[14];
	vcpu->arch.sie_block->gg15 = vcpu->run->s.regs.gprs[15];

	if (need_resched())
		schedule();

	if (test_cpu_flag(CIF_MCCK_PENDING))
		s390_handle_mcck();

	if (!kvm_is_ucontrol(vcpu->kvm)) {
		rc = kvm_s390_deliver_pending_interrupts(vcpu);
		if (rc)
			return rc;
	}

	rc = kvm_s390_handle_requests(vcpu);
	if (rc)
		return rc;

	if (guestdbg_enabled(vcpu)) {
		kvm_s390_backup_guest_per_regs(vcpu);
		kvm_s390_patch_guest_per_regs(vcpu);
	}

	clear_bit(vcpu->vcpu_id, vcpu->kvm->arch.gisa_int.kicked_mask);

	vcpu->arch.sie_block->icptcode = 0;
	cpuflags = atomic_read(&vcpu->arch.sie_block->cpuflags);
	VCPU_EVENT(vcpu, 6, "entering sie flags %x", cpuflags);
	trace_kvm_s390_sie_enter(vcpu, cpuflags);

	return 0;
}
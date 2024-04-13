static int __vcpu_run(struct kvm_vcpu *vcpu)
{
	int rc, exit_reason;
	struct sie_page *sie_page = (struct sie_page *)vcpu->arch.sie_block;

	/*
	 * We try to hold kvm->srcu during most of vcpu_run (except when run-
	 * ning the guest), so that memslots (and other stuff) are protected
	 */
	vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);

	do {
		rc = vcpu_pre_run(vcpu);
		if (rc)
			break;

		srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);
		/*
		 * As PF_VCPU will be used in fault handler, between
		 * guest_enter and guest_exit should be no uaccess.
		 */
		local_irq_disable();
		guest_enter_irqoff();
		__disable_cpu_timer_accounting(vcpu);
		local_irq_enable();
		if (kvm_s390_pv_cpu_is_protected(vcpu)) {
			memcpy(sie_page->pv_grregs,
			       vcpu->run->s.regs.gprs,
			       sizeof(sie_page->pv_grregs));
		}
		exit_reason = sie64a(vcpu->arch.sie_block,
				     vcpu->run->s.regs.gprs);
		if (kvm_s390_pv_cpu_is_protected(vcpu)) {
			memcpy(vcpu->run->s.regs.gprs,
			       sie_page->pv_grregs,
			       sizeof(sie_page->pv_grregs));
			/*
			 * We're not allowed to inject interrupts on intercepts
			 * that leave the guest state in an "in-between" state
			 * where the next SIE entry will do a continuation.
			 * Fence interrupts in our "internal" PSW.
			 */
			if (vcpu->arch.sie_block->icptcode == ICPT_PV_INSTR ||
			    vcpu->arch.sie_block->icptcode == ICPT_PV_PREF) {
				vcpu->arch.sie_block->gpsw.mask &= ~PSW_INT_MASK;
			}
		}
		local_irq_disable();
		__enable_cpu_timer_accounting(vcpu);
		guest_exit_irqoff();
		local_irq_enable();
		vcpu->srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);

		rc = vcpu_post_run(vcpu, exit_reason);
	} while (!signal_pending(current) && !guestdbg_exit_pending(vcpu) && !rc);

	srcu_read_unlock(&vcpu->kvm->srcu, vcpu->srcu_idx);
	return rc;
}
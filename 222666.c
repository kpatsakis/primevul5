static void sync_regs_fmt2(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	struct runtime_instr_cb *riccb;
	struct gs_cb *gscb;

	riccb = (struct runtime_instr_cb *) &kvm_run->s.regs.riccb;
	gscb = (struct gs_cb *) &kvm_run->s.regs.gscb;
	vcpu->arch.sie_block->gpsw.mask = kvm_run->psw_mask;
	vcpu->arch.sie_block->gpsw.addr = kvm_run->psw_addr;
	if (kvm_run->kvm_dirty_regs & KVM_SYNC_ARCH0) {
		vcpu->arch.sie_block->todpr = kvm_run->s.regs.todpr;
		vcpu->arch.sie_block->pp = kvm_run->s.regs.pp;
		vcpu->arch.sie_block->gbea = kvm_run->s.regs.gbea;
	}
	if (kvm_run->kvm_dirty_regs & KVM_SYNC_PFAULT) {
		vcpu->arch.pfault_token = kvm_run->s.regs.pft;
		vcpu->arch.pfault_select = kvm_run->s.regs.pfs;
		vcpu->arch.pfault_compare = kvm_run->s.regs.pfc;
		if (vcpu->arch.pfault_token == KVM_S390_PFAULT_TOKEN_INVALID)
			kvm_clear_async_pf_completion_queue(vcpu);
	}
	/*
	 * If userspace sets the riccb (e.g. after migration) to a valid state,
	 * we should enable RI here instead of doing the lazy enablement.
	 */
	if ((kvm_run->kvm_dirty_regs & KVM_SYNC_RICCB) &&
	    test_kvm_facility(vcpu->kvm, 64) &&
	    riccb->v &&
	    !(vcpu->arch.sie_block->ecb3 & ECB3_RI)) {
		VCPU_EVENT(vcpu, 3, "%s", "ENABLE: RI (sync_regs)");
		vcpu->arch.sie_block->ecb3 |= ECB3_RI;
	}
	/*
	 * If userspace sets the gscb (e.g. after migration) to non-zero,
	 * we should enable GS here instead of doing the lazy enablement.
	 */
	if ((kvm_run->kvm_dirty_regs & KVM_SYNC_GSCB) &&
	    test_kvm_facility(vcpu->kvm, 133) &&
	    gscb->gssm &&
	    !vcpu->arch.gs_enabled) {
		VCPU_EVENT(vcpu, 3, "%s", "ENABLE: GS (sync_regs)");
		vcpu->arch.sie_block->ecb |= ECB_GS;
		vcpu->arch.sie_block->ecd |= ECD_HOSTREGMGMT;
		vcpu->arch.gs_enabled = 1;
	}
	if ((kvm_run->kvm_dirty_regs & KVM_SYNC_BPBC) &&
	    test_kvm_facility(vcpu->kvm, 82)) {
		vcpu->arch.sie_block->fpf &= ~FPF_BPBC;
		vcpu->arch.sie_block->fpf |= kvm_run->s.regs.bpbc ? FPF_BPBC : 0;
	}
	if (MACHINE_HAS_GS) {
		preempt_disable();
		__ctl_set_bit(2, 4);
		if (current->thread.gs_cb) {
			vcpu->arch.host_gscb = current->thread.gs_cb;
			save_gs_cb(vcpu->arch.host_gscb);
		}
		if (vcpu->arch.gs_enabled) {
			current->thread.gs_cb = (struct gs_cb *)
						&vcpu->run->s.regs.gscb;
			restore_gs_cb(current->thread.gs_cb);
		}
		preempt_enable();
	}
	/* SIE will load etoken directly from SDNX and therefore kvm_run */
}
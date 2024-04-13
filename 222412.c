static void store_regs_fmt2(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	kvm_run->s.regs.todpr = vcpu->arch.sie_block->todpr;
	kvm_run->s.regs.pp = vcpu->arch.sie_block->pp;
	kvm_run->s.regs.gbea = vcpu->arch.sie_block->gbea;
	kvm_run->s.regs.bpbc = (vcpu->arch.sie_block->fpf & FPF_BPBC) == FPF_BPBC;
	if (MACHINE_HAS_GS) {
		__ctl_set_bit(2, 4);
		if (vcpu->arch.gs_enabled)
			save_gs_cb(current->thread.gs_cb);
		preempt_disable();
		current->thread.gs_cb = vcpu->arch.host_gscb;
		restore_gs_cb(vcpu->arch.host_gscb);
		preempt_enable();
		if (!vcpu->arch.host_gscb)
			__ctl_clear_bit(2, 4);
		vcpu->arch.host_gscb = NULL;
	}
	/* SIE will save etoken directly into SDNX and therefore kvm_run */
}
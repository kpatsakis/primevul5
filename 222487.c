static void store_regs(struct kvm_vcpu *vcpu, struct kvm_run *kvm_run)
{
	kvm_run->psw_mask = vcpu->arch.sie_block->gpsw.mask;
	kvm_run->psw_addr = vcpu->arch.sie_block->gpsw.addr;
	kvm_run->s.regs.prefix = kvm_s390_get_prefix(vcpu);
	memcpy(&kvm_run->s.regs.crs, &vcpu->arch.sie_block->gcr, 128);
	kvm_run->s.regs.cputm = kvm_s390_get_cpu_timer(vcpu);
	kvm_run->s.regs.ckc = vcpu->arch.sie_block->ckc;
	kvm_run->s.regs.pft = vcpu->arch.pfault_token;
	kvm_run->s.regs.pfs = vcpu->arch.pfault_select;
	kvm_run->s.regs.pfc = vcpu->arch.pfault_compare;
	save_access_regs(vcpu->run->s.regs.acrs);
	restore_access_regs(vcpu->arch.host_acrs);
	/* Save guest register state */
	save_fpu_regs();
	vcpu->run->s.regs.fpc = current->thread.fpu.fpc;
	/* Restore will be done lazily at return */
	current->thread.fpu.fpc = vcpu->arch.host_fpregs.fpc;
	current->thread.fpu.regs = vcpu->arch.host_fpregs.regs;
	if (likely(!kvm_s390_pv_cpu_is_protected(vcpu)))
		store_regs_fmt2(vcpu, kvm_run);
}
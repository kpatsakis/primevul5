static void kvm_arch_vcpu_ioctl_clear_reset(struct kvm_vcpu *vcpu)
{
	struct kvm_sync_regs *regs = &vcpu->run->s.regs;

	/* Clear reset is a superset of the initial reset */
	kvm_arch_vcpu_ioctl_initial_reset(vcpu);

	memset(&regs->gprs, 0, sizeof(regs->gprs));
	memset(&regs->vrs, 0, sizeof(regs->vrs));
	memset(&regs->acrs, 0, sizeof(regs->acrs));
	memset(&regs->gscb, 0, sizeof(regs->gscb));

	regs->etoken = 0;
	regs->etoken_extension = 0;
}
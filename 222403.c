int kvm_s390_vcpu_store_status(struct kvm_vcpu *vcpu, unsigned long addr)
{
	/*
	 * The guest FPRS and ACRS are in the host FPRS/ACRS due to the lazy
	 * switch in the run ioctl. Let's update our copies before we save
	 * it into the save area
	 */
	save_fpu_regs();
	vcpu->run->s.regs.fpc = current->thread.fpu.fpc;
	save_access_regs(vcpu->run->s.regs.acrs);

	return kvm_s390_store_status_unloaded(vcpu, addr);
}
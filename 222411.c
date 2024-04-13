int kvm_arch_vcpu_ioctl_get_fpu(struct kvm_vcpu *vcpu, struct kvm_fpu *fpu)
{
	vcpu_load(vcpu);

	/* make sure we have the latest values */
	save_fpu_regs();
	if (MACHINE_HAS_VX)
		convert_vx_to_fp((freg_t *) fpu->fprs,
				 (__vector128 *) vcpu->run->s.regs.vrs);
	else
		memcpy(fpu->fprs, vcpu->run->s.regs.fprs, sizeof(fpu->fprs));
	fpu->fpc = vcpu->run->s.regs.fpc;

	vcpu_put(vcpu);
	return 0;
}
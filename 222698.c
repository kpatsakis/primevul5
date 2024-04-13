int kvm_arch_vcpu_ioctl_set_fpu(struct kvm_vcpu *vcpu, struct kvm_fpu *fpu)
{
	int ret = 0;

	vcpu_load(vcpu);

	if (test_fp_ctl(fpu->fpc)) {
		ret = -EINVAL;
		goto out;
	}
	vcpu->run->s.regs.fpc = fpu->fpc;
	if (MACHINE_HAS_VX)
		convert_fp_to_vx((__vector128 *) vcpu->run->s.regs.vrs,
				 (freg_t *) fpu->fprs);
	else
		memcpy(vcpu->run->s.regs.fprs, &fpu->fprs, sizeof(fpu->fprs));

out:
	vcpu_put(vcpu);
	return ret;
}
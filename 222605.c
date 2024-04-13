int kvm_s390_store_status_unloaded(struct kvm_vcpu *vcpu, unsigned long gpa)
{
	unsigned char archmode = 1;
	freg_t fprs[NUM_FPRS];
	unsigned int px;
	u64 clkcomp, cputm;
	int rc;

	px = kvm_s390_get_prefix(vcpu);
	if (gpa == KVM_S390_STORE_STATUS_NOADDR) {
		if (write_guest_abs(vcpu, 163, &archmode, 1))
			return -EFAULT;
		gpa = 0;
	} else if (gpa == KVM_S390_STORE_STATUS_PREFIXED) {
		if (write_guest_real(vcpu, 163, &archmode, 1))
			return -EFAULT;
		gpa = px;
	} else
		gpa -= __LC_FPREGS_SAVE_AREA;

	/* manually convert vector registers if necessary */
	if (MACHINE_HAS_VX) {
		convert_vx_to_fp(fprs, (__vector128 *) vcpu->run->s.regs.vrs);
		rc = write_guest_abs(vcpu, gpa + __LC_FPREGS_SAVE_AREA,
				     fprs, 128);
	} else {
		rc = write_guest_abs(vcpu, gpa + __LC_FPREGS_SAVE_AREA,
				     vcpu->run->s.regs.fprs, 128);
	}
	rc |= write_guest_abs(vcpu, gpa + __LC_GPREGS_SAVE_AREA,
			      vcpu->run->s.regs.gprs, 128);
	rc |= write_guest_abs(vcpu, gpa + __LC_PSW_SAVE_AREA,
			      &vcpu->arch.sie_block->gpsw, 16);
	rc |= write_guest_abs(vcpu, gpa + __LC_PREFIX_SAVE_AREA,
			      &px, 4);
	rc |= write_guest_abs(vcpu, gpa + __LC_FP_CREG_SAVE_AREA,
			      &vcpu->run->s.regs.fpc, 4);
	rc |= write_guest_abs(vcpu, gpa + __LC_TOD_PROGREG_SAVE_AREA,
			      &vcpu->arch.sie_block->todpr, 4);
	cputm = kvm_s390_get_cpu_timer(vcpu);
	rc |= write_guest_abs(vcpu, gpa + __LC_CPU_TIMER_SAVE_AREA,
			      &cputm, 8);
	clkcomp = vcpu->arch.sie_block->ckc >> 8;
	rc |= write_guest_abs(vcpu, gpa + __LC_CLOCK_COMP_SAVE_AREA,
			      &clkcomp, 8);
	rc |= write_guest_abs(vcpu, gpa + __LC_AREGS_SAVE_AREA,
			      &vcpu->run->s.regs.acrs, 64);
	rc |= write_guest_abs(vcpu, gpa + __LC_CREGS_SAVE_AREA,
			      &vcpu->arch.sie_block->gcr, 128);
	return rc ? -EFAULT : 0;
}
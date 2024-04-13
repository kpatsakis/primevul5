static int vcpu_post_run_fault_in_sie(struct kvm_vcpu *vcpu)
{
	struct kvm_s390_pgm_info pgm_info = {
		.code = PGM_ADDRESSING,
	};
	u8 opcode, ilen;
	int rc;

	VCPU_EVENT(vcpu, 3, "%s", "fault in sie instruction");
	trace_kvm_s390_sie_fault(vcpu);

	/*
	 * We want to inject an addressing exception, which is defined as a
	 * suppressing or terminating exception. However, since we came here
	 * by a DAT access exception, the PSW still points to the faulting
	 * instruction since DAT exceptions are nullifying. So we've got
	 * to look up the current opcode to get the length of the instruction
	 * to be able to forward the PSW.
	 */
	rc = read_guest_instr(vcpu, vcpu->arch.sie_block->gpsw.addr, &opcode, 1);
	ilen = insn_length(opcode);
	if (rc < 0) {
		return rc;
	} else if (rc) {
		/* Instruction-Fetching Exceptions - we can't detect the ilen.
		 * Forward by arbitrary ilc, injection will take care of
		 * nullification if necessary.
		 */
		pgm_info = vcpu->arch.pgm;
		ilen = 4;
	}
	pgm_info.flags = ilen | KVM_S390_PGM_FLAGS_ILC_VALID;
	kvm_s390_forward_psw(vcpu, ilen);
	return kvm_s390_inject_prog_irq(vcpu, &pgm_info);
}
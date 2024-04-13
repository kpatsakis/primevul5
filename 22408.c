static int em_rdtsc(struct x86_emulate_ctxt *ctxt)
{
	u64 tsc = 0;

	ctxt->ops->get_msr(ctxt, MSR_IA32_TSC, &tsc);
	ctxt->regs[VCPU_REGS_RAX] = (u32)tsc;
	ctxt->regs[VCPU_REGS_RDX] = tsc >> 32;
	return X86EMUL_CONTINUE;
}
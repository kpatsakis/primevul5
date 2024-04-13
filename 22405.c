static int em_rdmsr(struct x86_emulate_ctxt *ctxt)
{
	u64 msr_data;

	if (ctxt->ops->get_msr(ctxt, ctxt->regs[VCPU_REGS_RCX], &msr_data))
		return emulate_gp(ctxt, 0);

	ctxt->regs[VCPU_REGS_RAX] = (u32)msr_data;
	ctxt->regs[VCPU_REGS_RDX] = msr_data >> 32;
	return X86EMUL_CONTINUE;
}
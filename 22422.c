static int em_rdpmc(struct x86_emulate_ctxt *ctxt)
{
	u64 pmc;

	if (ctxt->ops->read_pmc(ctxt, ctxt->regs[VCPU_REGS_RCX], &pmc))
		return emulate_gp(ctxt, 0);
	ctxt->regs[VCPU_REGS_RAX] = (u32)pmc;
	ctxt->regs[VCPU_REGS_RDX] = pmc >> 32;
	return X86EMUL_CONTINUE;
}
static int em_jcxz(struct x86_emulate_ctxt *ctxt)
{
	if (address_mask(ctxt, ctxt->regs[VCPU_REGS_RCX]) == 0)
		jmp_rel(ctxt, ctxt->src.val);

	return X86EMUL_CONTINUE;
}
static int em_not(struct x86_emulate_ctxt *ctxt)
{
	ctxt->dst.val = ~ctxt->dst.val;
	return X86EMUL_CONTINUE;
}
static int em_mov(struct x86_emulate_ctxt *ctxt)
{
	ctxt->dst.val = ctxt->src.val;
	return X86EMUL_CONTINUE;
}
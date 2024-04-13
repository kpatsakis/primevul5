static int em_movdqu(struct x86_emulate_ctxt *ctxt)
{
	memcpy(&ctxt->dst.vec_val, &ctxt->src.vec_val, ctxt->op_bytes);
	return X86EMUL_CONTINUE;
}
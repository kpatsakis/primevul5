static int em_cmp(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV(ctxt, "cmp");
	/* Disable writeback. */
	ctxt->dst.type = OP_NONE;
	return X86EMUL_CONTINUE;
}
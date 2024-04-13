static int em_and(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV(ctxt, "and");
	return X86EMUL_CONTINUE;
}
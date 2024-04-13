static int em_or(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV(ctxt, "or");
	return X86EMUL_CONTINUE;
}
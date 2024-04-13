static int em_sub(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV(ctxt, "sub");
	return X86EMUL_CONTINUE;
}
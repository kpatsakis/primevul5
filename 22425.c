static int em_xor(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV(ctxt, "xor");
	return X86EMUL_CONTINUE;
}
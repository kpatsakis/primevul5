static int em_sbb(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV(ctxt, "sbb");
	return X86EMUL_CONTINUE;
}
static int em_bts(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV_nobyte(ctxt, "bts");
	return X86EMUL_CONTINUE;
}
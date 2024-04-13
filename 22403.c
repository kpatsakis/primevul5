static int em_btr(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV_nobyte(ctxt, "btr");
	return X86EMUL_CONTINUE;
}
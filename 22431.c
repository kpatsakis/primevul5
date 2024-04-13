static int em_btc(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV_nobyte(ctxt, "btc");
	return X86EMUL_CONTINUE;
}
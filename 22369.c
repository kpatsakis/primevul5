static int em_imul(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV_nobyte(ctxt, "imul");
	return X86EMUL_CONTINUE;
}
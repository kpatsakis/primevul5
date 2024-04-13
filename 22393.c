static int em_neg(struct x86_emulate_ctxt *ctxt)
{
	emulate_1op(ctxt, "neg");
	return X86EMUL_CONTINUE;
}
static int em_adc(struct x86_emulate_ctxt *ctxt)
{
	emulate_2op_SrcV(ctxt, "adc");
	return X86EMUL_CONTINUE;
}
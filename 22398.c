static int em_idiv_ex(struct x86_emulate_ctxt *ctxt)
{
	u8 de = 0;

	emulate_1op_rax_rdx(ctxt, "idiv", de);
	if (de)
		return emulate_de(ctxt);
	return X86EMUL_CONTINUE;
}
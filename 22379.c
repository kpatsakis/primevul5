static int em_mul_ex(struct x86_emulate_ctxt *ctxt)
{
	u8 ex = 0;

	emulate_1op_rax_rdx(ctxt, "mul", ex);
	return X86EMUL_CONTINUE;
}
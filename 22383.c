static int em_bsf(struct x86_emulate_ctxt *ctxt)
{
	u8 zf;

	__asm__ ("bsf %2, %0; setz %1"
		 : "=r"(ctxt->dst.val), "=q"(zf)
		 : "r"(ctxt->src.val));

	ctxt->eflags &= ~X86_EFLAGS_ZF;
	if (zf) {
		ctxt->eflags |= X86_EFLAGS_ZF;
		/* Disable writeback. */
		ctxt->dst.type = OP_NONE;
	}
	return X86EMUL_CONTINUE;
}
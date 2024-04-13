static int em_cmpxchg(struct x86_emulate_ctxt *ctxt)
{
	/* Save real source value, then compare EAX against destination. */
	ctxt->src.orig_val = ctxt->src.val;
	ctxt->src.val = ctxt->regs[VCPU_REGS_RAX];
	emulate_2op_SrcV(ctxt, "cmp");

	if (ctxt->eflags & EFLG_ZF) {
		/* Success: write back to memory. */
		ctxt->dst.val = ctxt->src.orig_val;
	} else {
		/* Failure: write the value we saw to EAX. */
		ctxt->dst.type = OP_REG;
		ctxt->dst.addr.reg = (unsigned long *)&ctxt->regs[VCPU_REGS_RAX];
	}
	return X86EMUL_CONTINUE;
}
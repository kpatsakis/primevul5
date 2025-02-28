static int em_loop(struct x86_emulate_ctxt *ctxt)
{
	register_address_increment(ctxt, &ctxt->regs[VCPU_REGS_RCX], -1);
	if ((address_mask(ctxt, ctxt->regs[VCPU_REGS_RCX]) != 0) &&
	    (ctxt->b == 0xe2 || test_cc(ctxt->b ^ 0x5, ctxt->eflags)))
		jmp_rel(ctxt, ctxt->src.val);

	return X86EMUL_CONTINUE;
}
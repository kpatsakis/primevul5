static int emulate_pop(struct x86_emulate_ctxt *ctxt,
		       void *dest, int len)
{
	int rc;
	struct segmented_address addr;

	addr.ea = register_address(ctxt, ctxt->regs[VCPU_REGS_RSP]);
	addr.seg = VCPU_SREG_SS;
	rc = segmented_read(ctxt, addr, dest, len);
	if (rc != X86EMUL_CONTINUE)
		return rc;

	register_address_increment(ctxt, &ctxt->regs[VCPU_REGS_RSP], len);
	return rc;
}
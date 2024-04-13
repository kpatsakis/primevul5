static int em_push(struct x86_emulate_ctxt *ctxt)
{
	struct segmented_address addr;

	register_address_increment(ctxt, &ctxt->regs[VCPU_REGS_RSP], -ctxt->op_bytes);
	addr.ea = register_address(ctxt, ctxt->regs[VCPU_REGS_RSP]);
	addr.seg = VCPU_SREG_SS;

	/* Disable writeback. */
	ctxt->dst.type = OP_NONE;
	return segmented_write(ctxt, addr, &ctxt->src.val, ctxt->op_bytes);
}
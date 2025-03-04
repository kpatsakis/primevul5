static int em_ret_near_imm(struct x86_emulate_ctxt *ctxt)
{
	int rc;

	ctxt->dst.type = OP_REG;
	ctxt->dst.addr.reg = &ctxt->_eip;
	ctxt->dst.bytes = ctxt->op_bytes;
	rc = emulate_pop(ctxt, &ctxt->dst.val, ctxt->op_bytes);
	if (rc != X86EMUL_CONTINUE)
		return rc;
	register_address_increment(ctxt, &ctxt->regs[VCPU_REGS_RSP], ctxt->src.val);
	return X86EMUL_CONTINUE;
}
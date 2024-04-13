static int em_popa(struct x86_emulate_ctxt *ctxt)
{
	int rc = X86EMUL_CONTINUE;
	int reg = VCPU_REGS_RDI;

	while (reg >= VCPU_REGS_RAX) {
		if (reg == VCPU_REGS_RSP) {
			register_address_increment(ctxt, &ctxt->regs[VCPU_REGS_RSP],
							ctxt->op_bytes);
			--reg;
		}

		rc = emulate_pop(ctxt, &ctxt->regs[reg], ctxt->op_bytes);
		if (rc != X86EMUL_CONTINUE)
			break;
		--reg;
	}
	return rc;
}
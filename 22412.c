static void decode_register_operand(struct x86_emulate_ctxt *ctxt,
				    struct operand *op,
				    int inhibit_bytereg)
{
	unsigned reg = ctxt->modrm_reg;
	int highbyte_regs = ctxt->rex_prefix == 0;

	if (!(ctxt->d & ModRM))
		reg = (ctxt->b & 7) | ((ctxt->rex_prefix & 1) << 3);

	if (ctxt->d & Sse) {
		op->type = OP_XMM;
		op->bytes = 16;
		op->addr.xmm = reg;
		read_sse_reg(ctxt, &op->vec_val, reg);
		return;
	}

	op->type = OP_REG;
	if ((ctxt->d & ByteOp) && !inhibit_bytereg) {
		op->addr.reg = decode_register(reg, ctxt->regs, highbyte_regs);
		op->bytes = 1;
	} else {
		op->addr.reg = decode_register(reg, ctxt->regs, 0);
		op->bytes = ctxt->op_bytes;
	}
	fetch_register_operand(op);
	op->orig_val = op->val;
}
static int decode_operand(struct x86_emulate_ctxt *ctxt, struct operand *op,
			  unsigned d)
{
	int rc = X86EMUL_CONTINUE;

	switch (d) {
	case OpReg:
		decode_register_operand(ctxt, op,
			 op == &ctxt->dst &&
			 ctxt->twobyte && (ctxt->b == 0xb6 || ctxt->b == 0xb7));
		break;
	case OpImmUByte:
		rc = decode_imm(ctxt, op, 1, false);
		break;
	case OpMem:
		ctxt->memop.bytes = (ctxt->d & ByteOp) ? 1 : ctxt->op_bytes;
	mem_common:
		*op = ctxt->memop;
		ctxt->memopp = op;
		if ((ctxt->d & BitOp) && op == &ctxt->dst)
			fetch_bit_operand(ctxt);
		op->orig_val = op->val;
		break;
	case OpMem64:
		ctxt->memop.bytes = 8;
		goto mem_common;
	case OpAcc:
		op->type = OP_REG;
		op->bytes = (ctxt->d & ByteOp) ? 1 : ctxt->op_bytes;
		op->addr.reg = &ctxt->regs[VCPU_REGS_RAX];
		fetch_register_operand(op);
		op->orig_val = op->val;
		break;
	case OpDI:
		op->type = OP_MEM;
		op->bytes = (ctxt->d & ByteOp) ? 1 : ctxt->op_bytes;
		op->addr.mem.ea =
			register_address(ctxt, ctxt->regs[VCPU_REGS_RDI]);
		op->addr.mem.seg = VCPU_SREG_ES;
		op->val = 0;
		break;
	case OpDX:
		op->type = OP_REG;
		op->bytes = 2;
		op->addr.reg = &ctxt->regs[VCPU_REGS_RDX];
		fetch_register_operand(op);
		break;
	case OpCL:
		op->bytes = 1;
		op->val = ctxt->regs[VCPU_REGS_RCX] & 0xff;
		break;
	case OpImmByte:
		rc = decode_imm(ctxt, op, 1, true);
		break;
	case OpOne:
		op->bytes = 1;
		op->val = 1;
		break;
	case OpImm:
		rc = decode_imm(ctxt, op, imm_size(ctxt), true);
		break;
	case OpMem16:
		ctxt->memop.bytes = 2;
		goto mem_common;
	case OpMem32:
		ctxt->memop.bytes = 4;
		goto mem_common;
	case OpImmU16:
		rc = decode_imm(ctxt, op, 2, false);
		break;
	case OpImmU:
		rc = decode_imm(ctxt, op, imm_size(ctxt), false);
		break;
	case OpSI:
		op->type = OP_MEM;
		op->bytes = (ctxt->d & ByteOp) ? 1 : ctxt->op_bytes;
		op->addr.mem.ea =
			register_address(ctxt, ctxt->regs[VCPU_REGS_RSI]);
		op->addr.mem.seg = seg_override(ctxt);
		op->val = 0;
		break;
	case OpImmFAddr:
		op->type = OP_IMM;
		op->addr.mem.ea = ctxt->_eip;
		op->bytes = ctxt->op_bytes + 2;
		insn_fetch_arr(op->valptr, op->bytes, ctxt);
		break;
	case OpMemFAddr:
		ctxt->memop.bytes = ctxt->op_bytes + 2;
		goto mem_common;
	case OpES:
		op->val = VCPU_SREG_ES;
		break;
	case OpCS:
		op->val = VCPU_SREG_CS;
		break;
	case OpSS:
		op->val = VCPU_SREG_SS;
		break;
	case OpDS:
		op->val = VCPU_SREG_DS;
		break;
	case OpFS:
		op->val = VCPU_SREG_FS;
		break;
	case OpGS:
		op->val = VCPU_SREG_GS;
		break;
	case OpImplicit:
		/* Special instructions do their own operand decoding. */
	default:
		op->type = OP_NONE; /* Disable writeback. */
		break;
	}

done:
	return rc;
}
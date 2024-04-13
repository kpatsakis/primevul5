static int decode_modrm(struct x86_emulate_ctxt *ctxt,
			struct operand *op)
{
	u8 sib;
	int index_reg = 0, base_reg = 0, scale;
	int rc = X86EMUL_CONTINUE;
	ulong modrm_ea = 0;

	if (ctxt->rex_prefix) {
		ctxt->modrm_reg = (ctxt->rex_prefix & 4) << 1;	/* REX.R */
		index_reg = (ctxt->rex_prefix & 2) << 2; /* REX.X */
		ctxt->modrm_rm = base_reg = (ctxt->rex_prefix & 1) << 3; /* REG.B */
	}

	ctxt->modrm = insn_fetch(u8, ctxt);
	ctxt->modrm_mod |= (ctxt->modrm & 0xc0) >> 6;
	ctxt->modrm_reg |= (ctxt->modrm & 0x38) >> 3;
	ctxt->modrm_rm |= (ctxt->modrm & 0x07);
	ctxt->modrm_seg = VCPU_SREG_DS;

	if (ctxt->modrm_mod == 3) {
		op->type = OP_REG;
		op->bytes = (ctxt->d & ByteOp) ? 1 : ctxt->op_bytes;
		op->addr.reg = decode_register(ctxt->modrm_rm,
					       ctxt->regs, ctxt->d & ByteOp);
		if (ctxt->d & Sse) {
			op->type = OP_XMM;
			op->bytes = 16;
			op->addr.xmm = ctxt->modrm_rm;
			read_sse_reg(ctxt, &op->vec_val, ctxt->modrm_rm);
			return rc;
		}
		fetch_register_operand(op);
		return rc;
	}

	op->type = OP_MEM;

	if (ctxt->ad_bytes == 2) {
		unsigned bx = ctxt->regs[VCPU_REGS_RBX];
		unsigned bp = ctxt->regs[VCPU_REGS_RBP];
		unsigned si = ctxt->regs[VCPU_REGS_RSI];
		unsigned di = ctxt->regs[VCPU_REGS_RDI];

		/* 16-bit ModR/M decode. */
		switch (ctxt->modrm_mod) {
		case 0:
			if (ctxt->modrm_rm == 6)
				modrm_ea += insn_fetch(u16, ctxt);
			break;
		case 1:
			modrm_ea += insn_fetch(s8, ctxt);
			break;
		case 2:
			modrm_ea += insn_fetch(u16, ctxt);
			break;
		}
		switch (ctxt->modrm_rm) {
		case 0:
			modrm_ea += bx + si;
			break;
		case 1:
			modrm_ea += bx + di;
			break;
		case 2:
			modrm_ea += bp + si;
			break;
		case 3:
			modrm_ea += bp + di;
			break;
		case 4:
			modrm_ea += si;
			break;
		case 5:
			modrm_ea += di;
			break;
		case 6:
			if (ctxt->modrm_mod != 0)
				modrm_ea += bp;
			break;
		case 7:
			modrm_ea += bx;
			break;
		}
		if (ctxt->modrm_rm == 2 || ctxt->modrm_rm == 3 ||
		    (ctxt->modrm_rm == 6 && ctxt->modrm_mod != 0))
			ctxt->modrm_seg = VCPU_SREG_SS;
		modrm_ea = (u16)modrm_ea;
	} else {
		/* 32/64-bit ModR/M decode. */
		if ((ctxt->modrm_rm & 7) == 4) {
			sib = insn_fetch(u8, ctxt);
			index_reg |= (sib >> 3) & 7;
			base_reg |= sib & 7;
			scale = sib >> 6;

			if ((base_reg & 7) == 5 && ctxt->modrm_mod == 0)
				modrm_ea += insn_fetch(s32, ctxt);
			else
				modrm_ea += ctxt->regs[base_reg];
			if (index_reg != 4)
				modrm_ea += ctxt->regs[index_reg] << scale;
		} else if ((ctxt->modrm_rm & 7) == 5 && ctxt->modrm_mod == 0) {
			if (ctxt->mode == X86EMUL_MODE_PROT64)
				ctxt->rip_relative = 1;
		} else
			modrm_ea += ctxt->regs[ctxt->modrm_rm];
		switch (ctxt->modrm_mod) {
		case 0:
			if (ctxt->modrm_rm == 5)
				modrm_ea += insn_fetch(s32, ctxt);
			break;
		case 1:
			modrm_ea += insn_fetch(s8, ctxt);
			break;
		case 2:
			modrm_ea += insn_fetch(s32, ctxt);
			break;
		}
	}
	op->addr.mem.ea = modrm_ea;
done:
	return rc;
}
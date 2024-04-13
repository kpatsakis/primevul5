int x86_decode_insn(struct x86_emulate_ctxt *ctxt, void *insn, int insn_len)
{
	int rc = X86EMUL_CONTINUE;
	int mode = ctxt->mode;
	int def_op_bytes, def_ad_bytes, goffset, simd_prefix;
	bool op_prefix = false;
	struct opcode opcode;

	ctxt->memop.type = OP_NONE;
	ctxt->memopp = NULL;
	ctxt->_eip = ctxt->eip;
	ctxt->fetch.start = ctxt->_eip;
	ctxt->fetch.end = ctxt->fetch.start + insn_len;
	if (insn_len > 0)
		memcpy(ctxt->fetch.data, insn, insn_len);

	switch (mode) {
	case X86EMUL_MODE_REAL:
	case X86EMUL_MODE_VM86:
	case X86EMUL_MODE_PROT16:
		def_op_bytes = def_ad_bytes = 2;
		break;
	case X86EMUL_MODE_PROT32:
		def_op_bytes = def_ad_bytes = 4;
		break;
#ifdef CONFIG_X86_64
	case X86EMUL_MODE_PROT64:
		def_op_bytes = 4;
		def_ad_bytes = 8;
		break;
#endif
	default:
		return EMULATION_FAILED;
	}

	ctxt->op_bytes = def_op_bytes;
	ctxt->ad_bytes = def_ad_bytes;

	/* Legacy prefixes. */
	for (;;) {
		switch (ctxt->b = insn_fetch(u8, ctxt)) {
		case 0x66:	/* operand-size override */
			op_prefix = true;
			/* switch between 2/4 bytes */
			ctxt->op_bytes = def_op_bytes ^ 6;
			break;
		case 0x67:	/* address-size override */
			if (mode == X86EMUL_MODE_PROT64)
				/* switch between 4/8 bytes */
				ctxt->ad_bytes = def_ad_bytes ^ 12;
			else
				/* switch between 2/4 bytes */
				ctxt->ad_bytes = def_ad_bytes ^ 6;
			break;
		case 0x26:	/* ES override */
		case 0x2e:	/* CS override */
		case 0x36:	/* SS override */
		case 0x3e:	/* DS override */
			set_seg_override(ctxt, (ctxt->b >> 3) & 3);
			break;
		case 0x64:	/* FS override */
		case 0x65:	/* GS override */
			set_seg_override(ctxt, ctxt->b & 7);
			break;
		case 0x40 ... 0x4f: /* REX */
			if (mode != X86EMUL_MODE_PROT64)
				goto done_prefixes;
			ctxt->rex_prefix = ctxt->b;
			continue;
		case 0xf0:	/* LOCK */
			ctxt->lock_prefix = 1;
			break;
		case 0xf2:	/* REPNE/REPNZ */
		case 0xf3:	/* REP/REPE/REPZ */
			ctxt->rep_prefix = ctxt->b;
			break;
		default:
			goto done_prefixes;
		}

		/* Any legacy prefix after a REX prefix nullifies its effect. */

		ctxt->rex_prefix = 0;
	}

done_prefixes:

	/* REX prefix. */
	if (ctxt->rex_prefix & 8)
		ctxt->op_bytes = 8;	/* REX.W */

	/* Opcode byte(s). */
	opcode = opcode_table[ctxt->b];
	/* Two-byte opcode? */
	if (ctxt->b == 0x0f) {
		ctxt->twobyte = 1;
		ctxt->b = insn_fetch(u8, ctxt);
		opcode = twobyte_table[ctxt->b];
	}
	ctxt->d = opcode.flags;

	while (ctxt->d & GroupMask) {
		switch (ctxt->d & GroupMask) {
		case Group:
			ctxt->modrm = insn_fetch(u8, ctxt);
			--ctxt->_eip;
			goffset = (ctxt->modrm >> 3) & 7;
			opcode = opcode.u.group[goffset];
			break;
		case GroupDual:
			ctxt->modrm = insn_fetch(u8, ctxt);
			--ctxt->_eip;
			goffset = (ctxt->modrm >> 3) & 7;
			if ((ctxt->modrm >> 6) == 3)
				opcode = opcode.u.gdual->mod3[goffset];
			else
				opcode = opcode.u.gdual->mod012[goffset];
			break;
		case RMExt:
			goffset = ctxt->modrm & 7;
			opcode = opcode.u.group[goffset];
			break;
		case Prefix:
			if (ctxt->rep_prefix && op_prefix)
				return EMULATION_FAILED;
			simd_prefix = op_prefix ? 0x66 : ctxt->rep_prefix;
			switch (simd_prefix) {
			case 0x00: opcode = opcode.u.gprefix->pfx_no; break;
			case 0x66: opcode = opcode.u.gprefix->pfx_66; break;
			case 0xf2: opcode = opcode.u.gprefix->pfx_f2; break;
			case 0xf3: opcode = opcode.u.gprefix->pfx_f3; break;
			}
			break;
		default:
			return EMULATION_FAILED;
		}

		ctxt->d &= ~(u64)GroupMask;
		ctxt->d |= opcode.flags;
	}

	ctxt->execute = opcode.u.execute;
	ctxt->check_perm = opcode.check_perm;
	ctxt->intercept = opcode.intercept;

	/* Unrecognised? */
	if (ctxt->d == 0 || (ctxt->d & Undefined))
		return EMULATION_FAILED;

	if (!(ctxt->d & VendorSpecific) && ctxt->only_vendor_specific_insn)
		return EMULATION_FAILED;

	if (mode == X86EMUL_MODE_PROT64 && (ctxt->d & Stack))
		ctxt->op_bytes = 8;

	if (ctxt->d & Op3264) {
		if (mode == X86EMUL_MODE_PROT64)
			ctxt->op_bytes = 8;
		else
			ctxt->op_bytes = 4;
	}

	if (ctxt->d & Sse)
		ctxt->op_bytes = 16;

	/* ModRM and SIB bytes. */
	if (ctxt->d & ModRM) {
		rc = decode_modrm(ctxt, &ctxt->memop);
		if (!ctxt->has_seg_override)
			set_seg_override(ctxt, ctxt->modrm_seg);
	} else if (ctxt->d & MemAbs)
		rc = decode_abs(ctxt, &ctxt->memop);
	if (rc != X86EMUL_CONTINUE)
		goto done;

	if (!ctxt->has_seg_override)
		set_seg_override(ctxt, VCPU_SREG_DS);

	ctxt->memop.addr.mem.seg = seg_override(ctxt);

	if (ctxt->memop.type == OP_MEM && ctxt->ad_bytes != 8)
		ctxt->memop.addr.mem.ea = (u32)ctxt->memop.addr.mem.ea;

	/*
	 * Decode and fetch the source operand: register, memory
	 * or immediate.
	 */
	rc = decode_operand(ctxt, &ctxt->src, (ctxt->d >> SrcShift) & OpMask);
	if (rc != X86EMUL_CONTINUE)
		goto done;

	/*
	 * Decode and fetch the second source operand: register, memory
	 * or immediate.
	 */
	rc = decode_operand(ctxt, &ctxt->src2, (ctxt->d >> Src2Shift) & OpMask);
	if (rc != X86EMUL_CONTINUE)
		goto done;

	/* Decode and fetch the destination operand: register or memory. */
	rc = decode_operand(ctxt, &ctxt->dst, (ctxt->d >> DstShift) & OpMask);

done:
	if (ctxt->memopp && ctxt->memopp->type == OP_MEM && ctxt->rip_relative)
		ctxt->memopp->addr.mem.ea += ctxt->_eip;

	return (rc != X86EMUL_CONTINUE) ? EMULATION_FAILED : EMULATION_OK;
}
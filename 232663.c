static void emit_stx(u8 **pprog, u32 size, u32 dst_reg, u32 src_reg, int off)
{
	u8 *prog = *pprog;
	int cnt = 0;

	switch (size) {
	case BPF_B:
		/* Emit 'mov byte ptr [rax + off], al' */
		if (is_ereg(dst_reg) || is_ereg_8l(src_reg))
			/* Add extra byte for eregs or SIL,DIL,BPL in src_reg */
			EMIT2(add_2mod(0x40, dst_reg, src_reg), 0x88);
		else
			EMIT1(0x88);
		break;
	case BPF_H:
		if (is_ereg(dst_reg) || is_ereg(src_reg))
			EMIT3(0x66, add_2mod(0x40, dst_reg, src_reg), 0x89);
		else
			EMIT2(0x66, 0x89);
		break;
	case BPF_W:
		if (is_ereg(dst_reg) || is_ereg(src_reg))
			EMIT2(add_2mod(0x40, dst_reg, src_reg), 0x89);
		else
			EMIT1(0x89);
		break;
	case BPF_DW:
		EMIT2(add_2mod(0x48, dst_reg, src_reg), 0x89);
		break;
	}
	emit_insn_suffix(&prog, dst_reg, src_reg, off);
	*pprog = prog;
}
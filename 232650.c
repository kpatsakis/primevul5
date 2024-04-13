static void emit_insn_suffix(u8 **pprog, u32 ptr_reg, u32 val_reg, int off)
{
	u8 *prog = *pprog;
	int cnt = 0;

	if (is_imm8(off)) {
		/* 1-byte signed displacement.
		 *
		 * If off == 0 we could skip this and save one extra byte, but
		 * special case of x86 R13 which always needs an offset is not
		 * worth the hassle
		 */
		EMIT2(add_2reg(0x40, ptr_reg, val_reg), off);
	} else {
		/* 4-byte signed displacement */
		EMIT1_off32(add_2reg(0x80, ptr_reg, val_reg), off);
	}
	*pprog = prog;
}
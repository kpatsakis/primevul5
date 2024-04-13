static void maybe_emit_mod(u8 **pprog, u32 dst_reg, u32 src_reg, bool is64)
{
	u8 *prog = *pprog;
	int cnt = 0;

	if (is64)
		EMIT1(add_2mod(0x48, dst_reg, src_reg));
	else if (is_ereg(dst_reg) || is_ereg(src_reg))
		EMIT1(add_2mod(0x40, dst_reg, src_reg));
	*pprog = prog;
}
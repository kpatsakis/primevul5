static void emit_ldx(u8 **pprog, u32 size, u32 dst_reg, u32 src_reg, int off)
{
	u8 *prog = *pprog;
	int cnt = 0;

	switch (size) {
	case BPF_B:
		/* Emit 'movzx rax, byte ptr [rax + off]' */
		EMIT3(add_2mod(0x48, src_reg, dst_reg), 0x0F, 0xB6);
		break;
	case BPF_H:
		/* Emit 'movzx rax, word ptr [rax + off]' */
		EMIT3(add_2mod(0x48, src_reg, dst_reg), 0x0F, 0xB7);
		break;
	case BPF_W:
		/* Emit 'mov eax, dword ptr [rax+0x14]' */
		if (is_ereg(dst_reg) || is_ereg(src_reg))
			EMIT2(add_2mod(0x40, src_reg, dst_reg), 0x8B);
		else
			EMIT1(0x8B);
		break;
	case BPF_DW:
		/* Emit 'mov rax, qword ptr [rax+0x14]' */
		EMIT2(add_2mod(0x48, src_reg, dst_reg), 0x8B);
		break;
	}
	emit_insn_suffix(&prog, src_reg, dst_reg, off);
	*pprog = prog;
}
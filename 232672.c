static int invoke_bpf_mod_ret(const struct btf_func_model *m, u8 **pprog,
			      struct bpf_tramp_progs *tp, int stack_size,
			      u8 **branches)
{
	u8 *prog = *pprog;
	int i, cnt = 0;

	/* The first fmod_ret program will receive a garbage return value.
	 * Set this to 0 to avoid confusing the program.
	 */
	emit_mov_imm32(&prog, false, BPF_REG_0, 0);
	emit_stx(&prog, BPF_DW, BPF_REG_FP, BPF_REG_0, -8);
	for (i = 0; i < tp->nr_progs; i++) {
		if (invoke_bpf_prog(m, &prog, tp->progs[i], stack_size, true))
			return -EINVAL;

		/* mod_ret prog stored return value into [rbp - 8]. Emit:
		 * if (*(u64 *)(rbp - 8) !=  0)
		 *	goto do_fexit;
		 */
		/* cmp QWORD PTR [rbp - 0x8], 0x0 */
		EMIT4(0x48, 0x83, 0x7d, 0xf8); EMIT1(0x00);

		/* Save the location of the branch and Generate 6 nops
		 * (4 bytes for an offset and 2 bytes for the jump) These nops
		 * are replaced with a conditional jump once do_fexit (i.e. the
		 * start of the fexit invocation) is finalized.
		 */
		branches[i] = prog;
		emit_nops(&prog, 4 + 2);
	}

	*pprog = prog;
	return 0;
}
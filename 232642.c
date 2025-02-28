static void save_regs(const struct btf_func_model *m, u8 **prog, int nr_args,
		      int stack_size)
{
	int i;
	/* Store function arguments to stack.
	 * For a function that accepts two pointers the sequence will be:
	 * mov QWORD PTR [rbp-0x10],rdi
	 * mov QWORD PTR [rbp-0x8],rsi
	 */
	for (i = 0; i < min(nr_args, 6); i++)
		emit_stx(prog, bytes_to_bpf_size(m->arg_size[i]),
			 BPF_REG_FP,
			 i == 5 ? X86_REG_R9 : BPF_REG_1 + i,
			 -(stack_size - i * 8));
}
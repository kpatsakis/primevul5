static int invoke_bpf(const struct btf_func_model *m, u8 **pprog,
		      struct bpf_tramp_progs *tp, int stack_size)
{
	int i;
	u8 *prog = *pprog;

	for (i = 0; i < tp->nr_progs; i++) {
		if (invoke_bpf_prog(m, &prog, tp->progs[i], stack_size, false))
			return -EINVAL;
	}
	*pprog = prog;
	return 0;
}
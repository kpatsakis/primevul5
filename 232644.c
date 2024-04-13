int arch_prepare_bpf_dispatcher(void *image, s64 *funcs, int num_funcs)
{
	u8 *prog = image;

	sort(funcs, num_funcs, sizeof(funcs[0]), cmp_ips, NULL);
	return emit_bpf_dispatcher(&prog, 0, num_funcs - 1, funcs);
}
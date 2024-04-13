static int __init load_umd(void)
{
	int err;

	err = umd_load_blob(&umd_ops.info, &bpf_preload_umd_start,
			    &bpf_preload_umd_end - &bpf_preload_umd_start);
	if (err)
		return err;
	bpf_preload_ops = &umd_ops;
	return err;
}
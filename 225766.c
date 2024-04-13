static int preload(struct bpf_preload_info *obj)
{
	int magic = BPF_PRELOAD_START;
	loff_t pos = 0;
	int i, err;
	ssize_t n;

	err = fork_usermode_driver(&umd_ops.info);
	if (err)
		return err;

	/* send the start magic to let UMD proceed with loading BPF progs */
	n = kernel_write(umd_ops.info.pipe_to_umh,
			 &magic, sizeof(magic), &pos);
	if (n != sizeof(magic))
		return -EPIPE;

	/* receive bpf_link IDs and names from UMD */
	pos = 0;
	for (i = 0; i < BPF_PRELOAD_LINKS; i++) {
		n = kernel_read(umd_ops.info.pipe_from_umh,
				&obj[i], sizeof(*obj), &pos);
		if (n != sizeof(*obj))
			return -EPIPE;
	}
	return 0;
}
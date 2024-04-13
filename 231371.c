static void *saved_tgids_start(struct seq_file *m, loff_t *pos)
{
	void *v;
	loff_t l = 0;

	if (!tgid_map)
		return NULL;

	v = &tgid_map[0];
	while (l <= *pos) {
		v = saved_tgids_next(m, v, &l);
		if (!v)
			return NULL;
	}

	return v;
}
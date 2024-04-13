static void v4l_print_enc_idx(const void *arg, bool write_only)
{
	const struct v4l2_enc_idx *p = arg;

	pr_cont("entries=%d, entries_cap=%d\n",
			p->entries, p->entries_cap);
}
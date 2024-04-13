static int fuse_show_options(struct seq_file *m, struct dentry *root)
{
	struct super_block *sb = root->d_sb;
	struct fuse_conn *fc = get_fuse_conn_super(sb);

	if (fc->legacy_opts_show) {
		seq_printf(m, ",user_id=%u",
			   from_kuid_munged(fc->user_ns, fc->user_id));
		seq_printf(m, ",group_id=%u",
			   from_kgid_munged(fc->user_ns, fc->group_id));
		if (fc->default_permissions)
			seq_puts(m, ",default_permissions");
		if (fc->allow_other)
			seq_puts(m, ",allow_other");
		if (fc->max_read != ~0)
			seq_printf(m, ",max_read=%u", fc->max_read);
		if (sb->s_bdev && sb->s_blocksize != FUSE_DEFAULT_BLKSIZE)
			seq_printf(m, ",blksize=%lu", sb->s_blocksize);
	}
#ifdef CONFIG_FUSE_DAX
	if (fc->dax)
		seq_puts(m, ",dax");
#endif

	return 0;
}
static struct buffer_head *ext4_find_entry(struct inode *dir,
					   const struct qstr *d_name,
					   struct ext4_dir_entry_2 **res_dir,
					   int *inlined)
{
	int err;
	struct ext4_filename fname;
	struct buffer_head *bh;

	err = ext4_fname_setup_filename(dir, d_name, 1, &fname);
	if (err == -ENOENT)
		return NULL;
	if (err)
		return ERR_PTR(err);

	bh = __ext4_find_entry(dir, &fname, res_dir, inlined);

	ext4_fname_free_filename(&fname);
	return bh;
}
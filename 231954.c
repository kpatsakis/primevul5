static inline int search_dirblock(struct buffer_head *bh,
				  struct inode *dir,
				  struct ext4_filename *fname,
				  unsigned int offset,
				  struct ext4_dir_entry_2 **res_dir)
{
	return ext4_search_dir(bh, bh->b_data, dir->i_sb->s_blocksize, dir,
			       fname, offset, res_dir);
}
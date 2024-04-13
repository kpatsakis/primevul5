void ext4_initialize_dirent_tail(struct buffer_head *bh,
				 unsigned int blocksize)
{
	struct ext4_dir_entry_tail *t = EXT4_DIRENT_TAIL(bh->b_data, blocksize);

	memset(t, 0, sizeof(struct ext4_dir_entry_tail));
	t->det_rec_len = ext4_rec_len_to_disk(
			sizeof(struct ext4_dir_entry_tail), blocksize);
	t->det_reserved_ft = EXT4_FT_DIR_CSUM;
}
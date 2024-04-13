static int ext4_setent(handle_t *handle, struct ext4_renament *ent,
		       unsigned ino, unsigned file_type)
{
	int retval, retval2;

	BUFFER_TRACE(ent->bh, "get write access");
	retval = ext4_journal_get_write_access(handle, ent->bh);
	if (retval)
		return retval;
	ent->de->inode = cpu_to_le32(ino);
	if (ext4_has_feature_filetype(ent->dir->i_sb))
		ent->de->file_type = file_type;
	inode_inc_iversion(ent->dir);
	ent->dir->i_ctime = ent->dir->i_mtime =
		current_time(ent->dir);
	retval = ext4_mark_inode_dirty(handle, ent->dir);
	BUFFER_TRACE(ent->bh, "call ext4_handle_dirty_metadata");
	if (!ent->inlined) {
		retval2 = ext4_handle_dirty_dirblock(handle, ent->dir, ent->bh);
		if (unlikely(retval2)) {
			ext4_std_error(ent->dir->i_sb, retval2);
			return retval2;
		}
	}
	brelse(ent->bh);
	ent->bh = NULL;

	return retval;
}
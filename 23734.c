static loff_t fuse_file_llseek(struct file *file, loff_t offset, int origin)
{
	loff_t retval;
	struct inode *inode = file->f_path.dentry->d_inode;

	mutex_lock(&inode->i_mutex);
	switch (origin) {
	case SEEK_END:
		retval = fuse_update_attributes(inode, NULL, file, NULL);
		if (retval)
			goto exit;
		offset += i_size_read(inode);
		break;
	case SEEK_CUR:
		offset += file->f_pos;
	}
	retval = -EINVAL;
	if (offset >= 0 && offset <= inode->i_sb->s_maxbytes) {
		if (offset != file->f_pos) {
			file->f_pos = offset;
			file->f_version = 0;
		}
		retval = offset;
	}
exit:
	mutex_unlock(&inode->i_mutex);
	return retval;
}
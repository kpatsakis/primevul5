
static void io_fixed_file_set(struct io_fixed_file *file_slot, struct file *file)
{
	unsigned long file_ptr = (unsigned long) file;

	if (__io_file_supports_async(file, READ))
		file_ptr |= FFS_ASYNC_READ;
	if (__io_file_supports_async(file, WRITE))
		file_ptr |= FFS_ASYNC_WRITE;
	if (S_ISREG(file_inode(file)->i_mode))
		file_ptr |= FFS_ISREG;
	file_slot->file_ptr = file_ptr;
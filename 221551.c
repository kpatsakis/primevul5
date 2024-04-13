static void kiocb_end_write(struct io_kiocb *req)
{
	/*
	 * Tell lockdep we inherited freeze protection from submission
	 * thread.
	 */
	if (req->flags & REQ_F_ISREG) {
		struct super_block *sb = file_inode(req->file)->i_sb;

		__sb_writers_acquired(sb, SB_FREEZE_WRITE);
		sb_end_write(sb);
	}
}
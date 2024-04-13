static ssize_t fuse_send_write_pages(struct fuse_io_args *ia,
				     struct kiocb *iocb, struct inode *inode,
				     loff_t pos, size_t count)
{
	struct fuse_args_pages *ap = &ia->ap;
	struct file *file = iocb->ki_filp;
	struct fuse_file *ff = file->private_data;
	struct fuse_mount *fm = ff->fm;
	unsigned int offset, i;
	int err;

	for (i = 0; i < ap->num_pages; i++)
		fuse_wait_on_page_writeback(inode, ap->pages[i]->index);

	fuse_write_args_fill(ia, ff, pos, count);
	ia->write.in.flags = fuse_write_flags(iocb);
	if (fm->fc->handle_killpriv_v2 && !capable(CAP_FSETID))
		ia->write.in.write_flags |= FUSE_WRITE_KILL_SUIDGID;

	err = fuse_simple_request(fm, &ap->args);
	if (!err && ia->write.out.size > count)
		err = -EIO;

	offset = ap->descs[0].offset;
	count = ia->write.out.size;
	for (i = 0; i < ap->num_pages; i++) {
		struct page *page = ap->pages[i];

		if (!err && !offset && count >= PAGE_SIZE)
			SetPageUptodate(page);

		if (count > PAGE_SIZE - offset)
			count -= PAGE_SIZE - offset;
		else
			count = 0;
		offset = 0;

		unlock_page(page);
		put_page(page);
	}

	return err;
}
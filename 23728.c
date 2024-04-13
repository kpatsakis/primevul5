static int fuse_get_user_pages(struct fuse_req *req, const char __user *buf,
			       size_t *nbytesp, int write)
{
	size_t nbytes = *nbytesp;
	unsigned long user_addr = (unsigned long) buf;
	unsigned offset = user_addr & ~PAGE_MASK;
	int npages;

	/* Special case for kernel I/O: can copy directly into the buffer */
	if (segment_eq(get_fs(), KERNEL_DS)) {
		if (write)
			req->in.args[1].value = (void *) user_addr;
		else
			req->out.args[0].value = (void *) user_addr;

		return 0;
	}

	nbytes = min_t(size_t, nbytes, FUSE_MAX_PAGES_PER_REQ << PAGE_SHIFT);
	npages = (nbytes + offset + PAGE_SIZE - 1) >> PAGE_SHIFT;
	npages = clamp(npages, 1, FUSE_MAX_PAGES_PER_REQ);
	down_read(&current->mm->mmap_sem);
	npages = get_user_pages(current, current->mm, user_addr, npages, !write,
				0, req->pages, NULL);
	up_read(&current->mm->mmap_sem);
	if (npages < 0)
		return npages;

	req->num_pages = npages;
	req->page_offset = offset;

	if (write)
		req->in.argpages = 1;
	else
		req->out.argpages = 1;

	nbytes = (req->num_pages << PAGE_SHIFT) - req->page_offset;
	*nbytesp = min(*nbytesp, nbytes);

	return 0;
}
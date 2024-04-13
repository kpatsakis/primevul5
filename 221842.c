long fuse_do_ioctl(struct file *file, unsigned int cmd, unsigned long arg,
		   unsigned int flags)
{
	struct fuse_file *ff = file->private_data;
	struct fuse_mount *fm = ff->fm;
	struct fuse_ioctl_in inarg = {
		.fh = ff->fh,
		.cmd = cmd,
		.arg = arg,
		.flags = flags
	};
	struct fuse_ioctl_out outarg;
	struct iovec *iov_page = NULL;
	struct iovec *in_iov = NULL, *out_iov = NULL;
	unsigned int in_iovs = 0, out_iovs = 0, max_pages;
	size_t in_size, out_size, c;
	ssize_t transferred;
	int err, i;
	struct iov_iter ii;
	struct fuse_args_pages ap = {};

#if BITS_PER_LONG == 32
	inarg.flags |= FUSE_IOCTL_32BIT;
#else
	if (flags & FUSE_IOCTL_COMPAT) {
		inarg.flags |= FUSE_IOCTL_32BIT;
#ifdef CONFIG_X86_X32
		if (in_x32_syscall())
			inarg.flags |= FUSE_IOCTL_COMPAT_X32;
#endif
	}
#endif

	/* assume all the iovs returned by client always fits in a page */
	BUILD_BUG_ON(sizeof(struct fuse_ioctl_iovec) * FUSE_IOCTL_MAX_IOV > PAGE_SIZE);

	err = -ENOMEM;
	ap.pages = fuse_pages_alloc(fm->fc->max_pages, GFP_KERNEL, &ap.descs);
	iov_page = (struct iovec *) __get_free_page(GFP_KERNEL);
	if (!ap.pages || !iov_page)
		goto out;

	fuse_page_descs_length_init(ap.descs, 0, fm->fc->max_pages);

	/*
	 * If restricted, initialize IO parameters as encoded in @cmd.
	 * RETRY from server is not allowed.
	 */
	if (!(flags & FUSE_IOCTL_UNRESTRICTED)) {
		struct iovec *iov = iov_page;

		iov->iov_base = (void __user *)arg;

		switch (cmd) {
		case FS_IOC_GETFLAGS:
		case FS_IOC_SETFLAGS:
			iov->iov_len = sizeof(int);
			break;
		default:
			iov->iov_len = _IOC_SIZE(cmd);
			break;
		}

		if (_IOC_DIR(cmd) & _IOC_WRITE) {
			in_iov = iov;
			in_iovs = 1;
		}

		if (_IOC_DIR(cmd) & _IOC_READ) {
			out_iov = iov;
			out_iovs = 1;
		}
	}

 retry:
	inarg.in_size = in_size = iov_length(in_iov, in_iovs);
	inarg.out_size = out_size = iov_length(out_iov, out_iovs);

	/*
	 * Out data can be used either for actual out data or iovs,
	 * make sure there always is at least one page.
	 */
	out_size = max_t(size_t, out_size, PAGE_SIZE);
	max_pages = DIV_ROUND_UP(max(in_size, out_size), PAGE_SIZE);

	/* make sure there are enough buffer pages and init request with them */
	err = -ENOMEM;
	if (max_pages > fm->fc->max_pages)
		goto out;
	while (ap.num_pages < max_pages) {
		ap.pages[ap.num_pages] = alloc_page(GFP_KERNEL | __GFP_HIGHMEM);
		if (!ap.pages[ap.num_pages])
			goto out;
		ap.num_pages++;
	}


	/* okay, let's send it to the client */
	ap.args.opcode = FUSE_IOCTL;
	ap.args.nodeid = ff->nodeid;
	ap.args.in_numargs = 1;
	ap.args.in_args[0].size = sizeof(inarg);
	ap.args.in_args[0].value = &inarg;
	if (in_size) {
		ap.args.in_numargs++;
		ap.args.in_args[1].size = in_size;
		ap.args.in_pages = true;

		err = -EFAULT;
		iov_iter_init(&ii, WRITE, in_iov, in_iovs, in_size);
		for (i = 0; iov_iter_count(&ii) && !WARN_ON(i >= ap.num_pages); i++) {
			c = copy_page_from_iter(ap.pages[i], 0, PAGE_SIZE, &ii);
			if (c != PAGE_SIZE && iov_iter_count(&ii))
				goto out;
		}
	}

	ap.args.out_numargs = 2;
	ap.args.out_args[0].size = sizeof(outarg);
	ap.args.out_args[0].value = &outarg;
	ap.args.out_args[1].size = out_size;
	ap.args.out_pages = true;
	ap.args.out_argvar = true;

	transferred = fuse_simple_request(fm, &ap.args);
	err = transferred;
	if (transferred < 0)
		goto out;

	/* did it ask for retry? */
	if (outarg.flags & FUSE_IOCTL_RETRY) {
		void *vaddr;

		/* no retry if in restricted mode */
		err = -EIO;
		if (!(flags & FUSE_IOCTL_UNRESTRICTED))
			goto out;

		in_iovs = outarg.in_iovs;
		out_iovs = outarg.out_iovs;

		/*
		 * Make sure things are in boundary, separate checks
		 * are to protect against overflow.
		 */
		err = -ENOMEM;
		if (in_iovs > FUSE_IOCTL_MAX_IOV ||
		    out_iovs > FUSE_IOCTL_MAX_IOV ||
		    in_iovs + out_iovs > FUSE_IOCTL_MAX_IOV)
			goto out;

		vaddr = kmap_atomic(ap.pages[0]);
		err = fuse_copy_ioctl_iovec(fm->fc, iov_page, vaddr,
					    transferred, in_iovs + out_iovs,
					    (flags & FUSE_IOCTL_COMPAT) != 0);
		kunmap_atomic(vaddr);
		if (err)
			goto out;

		in_iov = iov_page;
		out_iov = in_iov + in_iovs;

		err = fuse_verify_ioctl_iov(fm->fc, in_iov, in_iovs);
		if (err)
			goto out;

		err = fuse_verify_ioctl_iov(fm->fc, out_iov, out_iovs);
		if (err)
			goto out;

		goto retry;
	}

	err = -EIO;
	if (transferred > inarg.out_size)
		goto out;

	err = -EFAULT;
	iov_iter_init(&ii, READ, out_iov, out_iovs, transferred);
	for (i = 0; iov_iter_count(&ii) && !WARN_ON(i >= ap.num_pages); i++) {
		c = copy_page_to_iter(ap.pages[i], 0, PAGE_SIZE, &ii);
		if (c != PAGE_SIZE && iov_iter_count(&ii))
			goto out;
	}
	err = 0;
 out:
	free_page((unsigned long) iov_page);
	while (ap.num_pages)
		__free_page(ap.pages[--ap.num_pages]);
	kfree(ap.pages);

	return err ? err : outarg.result;
}
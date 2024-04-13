
static int io_sqe_files_register(struct io_ring_ctx *ctx, void __user *arg,
				 unsigned nr_args, u64 __user *tags)
{
	__s32 __user *fds = (__s32 __user *) arg;
	struct file *file;
	int fd, ret;
	unsigned i;
	struct io_rsrc_data *file_data;

	if (ctx->file_data)
		return -EBUSY;
	if (!nr_args)
		return -EINVAL;
	if (nr_args > IORING_MAX_FIXED_FILES)
		return -EMFILE;
	ret = io_rsrc_node_switch_start(ctx);
	if (ret)
		return ret;

	file_data = io_rsrc_data_alloc(ctx, io_rsrc_file_put, nr_args);
	if (!file_data)
		return -ENOMEM;
	ctx->file_data = file_data;
	ret = -ENOMEM;
	if (!io_alloc_file_tables(&ctx->file_table, nr_args))
		goto out_free;

	for (i = 0; i < nr_args; i++, ctx->nr_user_files++) {
		u64 tag = 0;

		if ((tags && copy_from_user(&tag, &tags[i], sizeof(tag))) ||
		    copy_from_user(&fd, &fds[i], sizeof(fd))) {
			ret = -EFAULT;
			goto out_fput;
		}
		/* allow sparse sets */
		if (fd == -1) {
			ret = -EINVAL;
			if (unlikely(tag))
				goto out_fput;
			continue;
		}

		file = fget(fd);
		ret = -EBADF;
		if (unlikely(!file))
			goto out_fput;

		/*
		 * Don't allow io_uring instances to be registered. If UNIX
		 * isn't enabled, then this causes a reference cycle and this
		 * instance can never get freed. If UNIX is enabled we'll
		 * handle it just fine, but there's still no point in allowing
		 * a ring fd as it doesn't support regular read/write anyway.
		 */
		if (file->f_op == &io_uring_fops) {
			fput(file);
			goto out_fput;
		}
		ctx->file_data->tags[i] = tag;
		io_fixed_file_set(io_fixed_file_slot(&ctx->file_table, i), file);
	}

	ret = io_sqe_files_scm(ctx);
	if (ret) {
		__io_sqe_files_unregister(ctx);
		return ret;
	}

	io_rsrc_node_switch(ctx, NULL);
	return ret;
out_fput:
	for (i = 0; i < ctx->nr_user_files; i++) {
		file = io_file_from_index(ctx, i);
		if (file)
			fput(file);
	}
	io_free_file_tables(&ctx->file_table, nr_args);
	ctx->nr_user_files = 0;
out_free:
	io_rsrc_data_free(ctx->file_data);
	ctx->file_data = NULL;
	return ret;
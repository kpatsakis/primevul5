
static int __io_sqe_files_update(struct io_ring_ctx *ctx,
				 struct io_uring_rsrc_update2 *up,
				 unsigned nr_args)
{
	u64 __user *tags = u64_to_user_ptr(up->tags);
	__s32 __user *fds = u64_to_user_ptr(up->data);
	struct io_rsrc_data *data = ctx->file_data;
	struct io_fixed_file *file_slot;
	struct file *file;
	int fd, i, err = 0;
	unsigned int done;
	bool needs_switch = false;

	if (!ctx->file_data)
		return -ENXIO;
	if (up->offset + nr_args > ctx->nr_user_files)
		return -EINVAL;

	for (done = 0; done < nr_args; done++) {
		u64 tag = 0;

		if ((tags && copy_from_user(&tag, &tags[done], sizeof(tag))) ||
		    copy_from_user(&fd, &fds[done], sizeof(fd))) {
			err = -EFAULT;
			break;
		}
		if ((fd == IORING_REGISTER_FILES_SKIP || fd == -1) && tag) {
			err = -EINVAL;
			break;
		}
		if (fd == IORING_REGISTER_FILES_SKIP)
			continue;

		i = array_index_nospec(up->offset + done, ctx->nr_user_files);
		file_slot = io_fixed_file_slot(&ctx->file_table, i);

		if (file_slot->file_ptr) {
			file = (struct file *)(file_slot->file_ptr & FFS_MASK);
			err = io_queue_rsrc_removal(data, up->offset + done,
						    ctx->rsrc_node, file);
			if (err)
				break;
			file_slot->file_ptr = 0;
			needs_switch = true;
		}
		if (fd != -1) {
			file = fget(fd);
			if (!file) {
				err = -EBADF;
				break;
			}
			/*
			 * Don't allow io_uring instances to be registered. If
			 * UNIX isn't enabled, then this causes a reference
			 * cycle and this instance can never get freed. If UNIX
			 * is enabled we'll handle it just fine, but there's
			 * still no point in allowing a ring fd as it doesn't
			 * support regular read/write anyway.
			 */
			if (file->f_op == &io_uring_fops) {
				fput(file);
				err = -EBADF;
				break;
			}
			data->tags[up->offset + done] = tag;
			io_fixed_file_set(file_slot, file);
			err = io_sqe_file_register(ctx, file, i);
			if (err) {
				file_slot->file_ptr = 0;
				fput(file);
				break;
			}
		}
	}

	if (needs_switch)
		io_rsrc_node_switch(ctx, data);
	return done ? done : err;
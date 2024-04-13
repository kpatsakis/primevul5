static int __io_sqe_files_update(struct io_ring_ctx *ctx,
				 struct io_uring_files_update *up,
				 unsigned nr_args)
{
	struct fixed_file_data *data = ctx->file_data;
	struct fixed_file_ref_node *ref_node;
	struct file *file;
	__s32 __user *fds;
	int fd, i, err;
	__u32 done;
	bool needs_switch = false;

	if (check_add_overflow(up->offset, nr_args, &done))
		return -EOVERFLOW;
	if (done > ctx->nr_user_files)
		return -EINVAL;

	ref_node = alloc_fixed_file_ref_node(ctx);
	if (IS_ERR(ref_node))
		return PTR_ERR(ref_node);

	done = 0;
	fds = u64_to_user_ptr(up->fds);
	while (nr_args) {
		struct fixed_file_table *table;
		unsigned index;

		err = 0;
		if (copy_from_user(&fd, &fds[done], sizeof(fd))) {
			err = -EFAULT;
			break;
		}
		i = array_index_nospec(up->offset, ctx->nr_user_files);
		table = &ctx->file_data->table[i >> IORING_FILE_TABLE_SHIFT];
		index = i & IORING_FILE_TABLE_MASK;
		if (table->files[index]) {
			file = io_file_from_index(ctx, index);
			err = io_queue_file_removal(data, file);
			if (err)
				break;
			table->files[index] = NULL;
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
			table->files[index] = file;
			err = io_sqe_file_register(ctx, file, i);
			if (err) {
				fput(file);
				break;
			}
		}
		nr_args--;
		done++;
		up->offset++;
	}

	if (needs_switch) {
		percpu_ref_kill(data->cur_refs);
		spin_lock(&data->lock);
		list_add(&ref_node->node, &data->ref_list);
		data->cur_refs = &ref_node->refs;
		spin_unlock(&data->lock);
		percpu_ref_get(&ctx->file_data->refs);
	} else
		destroy_fixed_file_ref_node(ref_node);

	return done ? done : err;
}